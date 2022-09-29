// [WriteFile Name=ContingentValues, Category=EditData]
// [Legal]
// Copyright 2022 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "ContingentValues.h"

#include "ArcGISVectorTiledLayer.h"
#include "CodedValueDomain.h"
#include "ContingentCodedValue.h"
#include "ContingentRangeValue.h"
#include "ContingentValuesDefinition.h"
#include "ContingentValuesResult.h"
#include "FeatureLayer.h"
#include "Geodatabase.h"
#include "GeodatabaseFeatureTable.h"
#include "GeometryEngine.h"
#include "Map.h"
#include "MapQuickView.h"
#include "SimpleRenderer.h"
#include "TaskWatcher.h"
#include "GraphicsOverlayListModel.h"
#include "SymbolTypes.h"
#include "LayerListModel.h"
#include "ArcGISFeature.h"
#include "MapTypes.h"
#include "GeodatabaseTypes.h"
#include "AttributeListModel.h"
#include "GraphicListModel.h"
#include "QueryParameters.h"
#include "FeatureIterator.h"
#include "FeatureQueryResult.h"
#include "Point.h"
#include "Basemap.h"
#include "Viewpoint.h"
#include "GraphicsOverlay.h"
#include "SimpleLineSymbol.h"
#include "SimpleFillSymbol.h"
#include "Polygon.h"
#include "Graphic.h"

#include <QUuid>

#if defined Q_OS_ANDROID || defined Q_OS_IOS
#include <QStandardPaths>
#else
#include <QDir>
#endif

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
QString defaultDataPath()
{
  QString dataPath;

#ifdef Q_OS_ANDROID
  dataPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
#elif defined Q_OS_IOS
  dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
  dataPath = QDir::homePath();
#endif

  return dataPath;
}
}

ContingentValues::ContingentValues(QObject* parent /* = nullptr */):
  QObject(parent)
{
  // Load the basemap from a vector tile package
  const QString vtpkDataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/vtpk/FillmoreTopographicMap.vtpk";
  ArcGISVectorTiledLayer* fillmoreVTPK = new ArcGISVectorTiledLayer(QUrl::fromLocalFile(vtpkDataPath), this);
  Basemap* fillmoreBasemap = new Basemap(fillmoreVTPK, this);
  m_map = new Map(fillmoreBasemap, this);

  // Load the geodatabase from a mobile geodatabase
  const QString gdbDataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/geodatabase/ContingentValuesBirdNests.geodatabase";
  m_geodatabase = new Geodatabase(gdbDataPath, this);

  // The coded value domains in this sample are hardcoded for simplicity, but can be retrieved from the GeodatabaseFeatureTable's Field's Domains.
  m_statusValues = QVariantMap{{"Occupied", "OCCUPIED"}, {"Unoccupied", "UNOCCUPIED"}};
  m_protectionValues = QVariantMap{{"Endangered", "ENDANGERED"}, {"Not endangered", "NOT_ENDANGERED"}, {"N/A", "NA"}};
}

ContingentValues::~ContingentValues() = default;

void ContingentValues::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ContingentValues>("Esri.Samples", 1, 0, "ContingentValuesSample");
}

MapQuickView* ContingentValues::mapView() const
{
  return m_mapView;
}

// Set the view with a graphics overlay
void ContingentValues::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;

  m_mapView->setMap(m_map);
  m_mapView->setViewpoint(Viewpoint(Point(-13236000, 4081200), 8822));

  // Create the graphics overlay with which to display the nest buffer exclusion areas
  m_graphicsOverlay = new GraphicsOverlay(this);
  Symbol* bufferSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::ForwardDiagonal, Qt::red, new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::black, 2, this), this);
  m_graphicsOverlay->setRenderer(new SimpleRenderer(bufferSymbol, this));
  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  createConnections();
  m_geodatabase->load();

  emit mapViewChanged();
}

// Load the Geodatabase, GeodatabaseFeatureTable, and the ContingentValuesDefinition
void ContingentValues::createConnections()
{
  connect(m_geodatabase, &Geodatabase::doneLoading, this, [this]()
  {
    // Retrieve the "BirdNests" geodatabaseFeatureTable which contains the ContingentValuesDefinition, among other important member variables and functions
    m_gdbFeatureTable = m_geodatabase->geodatabaseFeatureTable("BirdNests");

    if (!m_gdbFeatureTable)
      return;

    connect(m_gdbFeatureTable, &GeodatabaseFeatureTable::doneLoading, this, [this]
    {
      // Load the ContingentValuesDefinition to enable access to the GeodatabaseFeatureTable's ContingentValues data such as FieldGroups and ContingentValuesResults
      m_gdbFeatureTable->contingentValuesDefinition()->load();

      // Load and display the mobile geodatabase's predefined bird nests on the map
      FeatureLayer* nestLayer = new FeatureLayer(m_gdbFeatureTable, this);
      m_map->operationalLayers()->append(nestLayer);

      queryAndBufferFeatures();
    });

    connect(m_gdbFeatureTable, &GeodatabaseFeatureTable::queryFeaturesCompleted, this, &ContingentValues::bufferFeaturesFromQueryResults);

    m_gdbFeatureTable->load();
  });

  connect(m_mapView, &MapQuickView::mouseClicked, this, &ContingentValues::createNewEmptyFeature);
}

// When the user clicks or taps on the map, instantiate a new feature and show the attribute form interface
void ContingentValues::createNewEmptyFeature(QMouseEvent& mouseEvent)
{
  // Create a new empty feature to define attributes for
  m_newFeature = static_cast<ArcGISFeature*>(m_gdbFeatureTable->createFeature({}, m_mapView->screenToLocation(mouseEvent.pos().x(), mouseEvent.pos().y()), this));
  m_gdbFeatureTable->addFeature(m_newFeature);

  // Show the attribute form interface
  setFeatureAttributesPaneVisibe(true);
}

// Get a list of a feature's valid contingent values for field within a participating contingent value field group
QVariantList ContingentValues::getContingentValues(QString field, QString fieldGroupName)
{
  if (m_gdbFeatureTable->contingentValuesDefinition()->loadStatus() != LoadStatus::Loaded)
    return {};

  // Create an empty list with which to return the valid contingent values
  QVariantList contingentValuesNamesList = {};

  // Instantiate a dictionary containing all possible values for a field in the context of the contingent field groups it participates in
  ContingentValuesResult* contingentValuesResult = m_gdbFeatureTable->contingentValues(m_newFeature, field);

  // Loop through the contingent values. The QML UI is hardcoded to expect a list of contingent coded value names or a minimum and maximum value from a contingent range value
  for (ContingentValue* contingentValue : contingentValuesResult->contingentValuesByFieldGroup(this).value(fieldGroupName))
  {
    // Contingent coded values are contingent values defined from a coded value domain.
    // There are often multiple results returned by the ContingentValuesResult
    if (contingentValue->contingentValueType() == ContingentValueType::ContingentCodedValue)
    {
      ContingentCodedValue* contingentCodedValue = static_cast<ContingentCodedValue*>(contingentValue);
      if (contingentCodedValue)
        contingentValuesNamesList.append(contingentCodedValue->codedValue().name());
    }
    // Contingent range values constrain a range value domain
    // the ContingentValuesResult for a field defined by a range value domain often contains only one entry with a minimum and maximum value
    else if (contingentValue->contingentValueType() == ContingentValueType::ContingentRangeValue)
    {
      ContingentRangeValue* contingentRangeValue = static_cast<ContingentRangeValue*>(contingentValue);
      if (contingentRangeValue)
      {
        contingentValuesNamesList.append({contingentRangeValue->minValue()});
        contingentValuesNamesList.append({contingentRangeValue->maxValue()});
      }
    }
  }

  return contingentValuesNamesList;
}

// Validates if the sample's new feature has any constraint violations
bool ContingentValues::validateContingentValues()
{
  if (m_gdbFeatureTable->contingentValuesDefinition()->loadStatus() != LoadStatus::Loaded || !m_newFeature)
    return false;
  // Returns a list of field groups whose contingencies are in violation as well as the type of violation
  QList<ContingencyConstraintViolation*> constraintViolations = m_gdbFeatureTable->validateContingencyConstraints(m_newFeature);

  // If the list is empty, there are no violations and the attribute map satisfies all contingencies
  if (constraintViolations.isEmpty())
    return true;

  return false;
}

// Save the sample's new feature to the map's geodatabse feature table
void ContingentValues::createNewNest()
{
  // Once the attribute map is filled and validated, save the feature to the geodatabase feature table
  m_gdbFeatureTable->updateFeature(m_newFeature);

  queryAndBufferFeatures();
}

void ContingentValues::discardFeature()
{
  m_gdbFeatureTable->deleteFeature(m_newFeature);
}

// Update a specific field with a new value in the new feature's attribute map
void ContingentValues::updateField(QString field, QVariant value)
{
  if (field == "Status")
    m_newFeature->attributes()->replaceAttribute(field, m_statusValues.value(value.toString()));
  else if (field == "Protection")
    m_newFeature->attributes()->replaceAttribute(field, m_protectionValues.value(value.toString()));
  else if (field == "BufferSize")
    m_newFeature->attributes()->replaceAttribute(field, value.toInt());
  else
    qWarning() << field << "not found in any of the data dictionaries";
}

// The following two functions create a buffer around nest features based on their BufferSize value
// They are included to demonstrate the sample use case

// Kicks off a query feature task to return all features with buffer sizes greater than zero
void ContingentValues::queryAndBufferFeatures()
{
  if (!m_gdbFeatureTable || m_gdbFeatureTable->loadStatus() != LoadStatus::Loaded)
    return;

  m_graphicsOverlay->graphics()->clear();

  QueryParameters params;
  params.setWhereClause("BufferSize > 0");
  m_gdbFeatureTable->queryFeatures(params);
}

// Buffers all features from the preceeding feature query using the BufferSize field value, create graphics with the results, and adds them to the graphics overlay
void ContingentValues::bufferFeaturesFromQueryResults(QUuid, FeatureQueryResult* results)
{
  FeatureIterator iterator = results->iterator();

  while (iterator.hasNext())
  {
    Feature* feature = iterator.next(this);
    const double bufferDistance = feature->attributes()->attributeValue("BufferSize").toDouble();
    Polygon buffer = GeometryEngine::buffer(feature->geometry(), bufferDistance);
    m_mapView->graphicsOverlays()->at(0)->graphics()->append(new Graphic(buffer, this));
  }
}

bool ContingentValues::featureAttributesPaneVisibe() const
{
  return m_featureAttributesPaneVisible;
}

void ContingentValues::setFeatureAttributesPaneVisibe(bool showFeatureAttributesPane)
{
  m_featureAttributesPaneVisible = showFeatureAttributesPane;
  emit featureAttributesPaneVisibeChanged();
}

QVariantMap ContingentValues::statusValues() const
{
  return m_statusValues;
}
