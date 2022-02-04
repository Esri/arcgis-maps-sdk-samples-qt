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

#include "ArcGISFeature.h"
#include "CodedValueDomain.h"
#include "ContingentValue.h"
#include "ContingentCodedValue.h"
#include "ContingentRangeValue.h"
#include "ContingentValuesDefinition.h"
#include "ContingentValuesResult.h"
#include "Domain.h"
#include "FeatureLayer.h"
#include "Field.h"
#include "FieldGroup.h"
#include "Geodatabase.h"
#include "GeodatabaseFeatureTable.h"
#include "GeometryEngine.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MobileMapPackage.h"
#include "RangeDomain.h"
#include "ServiceFeatureTable.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleRenderer.h"

#include <QDir>
#include <QtCore/qglobal.h>

#ifdef Q_OS_IOS
#include <QStandardPaths>
#endif // Q_OS_IOS

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
QString defaultDataPath()
{
  QString dataPath;

#ifdef Q_OS_ANDROID
  dataPath = "/sdcard";
#elif defined Q_OS_IOS
  dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
  dataPath = QDir::homePath();
#endif

  return dataPath;
}
}

ContingentValues::ContingentValues(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISTerrain, this))
{
  const QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/geodatabase/ContingentValuesBirdNests.geodatabase";
  m_geodatabase = new Geodatabase(dataPath, this);

  setActivityValues(QVariantMap{{"Occupied", "OCCUPIED"}, {"Unoccupied", "UNOCCUPIED"}});
  setProtectionValues(QVariantMap{{"Endangered", "ENDANGERED"}, {"Not endangered", "NOT_ENDANGERED"}, {"N/A", "NA"}});
  //m_bufferSizeValues = {0, 120};
}

ContingentValues::~ContingentValues() = default;

void ContingentValues::updateField(QString field, QVariant value)
{
  if (field == "Status")
    m_featureAttributes.insert(field, m_activityValues.value(value.toString()));
  else if (field == "Protection")
    m_featureAttributes.insert(field, m_protectionValues.value(value.toString()));
  else if (field == "BufferSize")
    m_featureAttributes.insert(field, value);
  else
    qWarning() << field << value << "not found in any of the data dictionaries";

  m_selectedFeature->attributes()->setAttributesMap(m_featureAttributes);
}

void ContingentValues::setProtectionValues(QVariantMap protectionValues)
{
  m_protectionValues = protectionValues;
  emit featureValuesChanged();
}

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

// Set the view (created in QML)
void ContingentValues::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;

  m_mapView->setMap(m_map);

  m_graphicsOverlay = new GraphicsOverlay(this);
  Symbol* bufferSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::ForwardDiagonal, Qt::red, new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::black, 2, this), this);
  m_graphicsOverlay->setRenderer(new SimpleRenderer(bufferSymbol, this));

  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  m_mapView->setViewpoint(Viewpoint(Point(-13236000, 4081200), 8822));

  createConnections();
  m_geodatabase->load();

  emit mapViewChanged();
}

QVariantList ContingentValues::getContingentValues(QString field, QString fieldGroupName)
{
  if (m_contingentValuesDefinition->loadStatus() != LoadStatus::Loaded)
    return {};

  if (!m_selectedFeature)
    createNewEmptyFeature();

  QVariantList contingentValuesNamesList = {};

  ContingentValuesResult* contingentValuesResult = m_gdbFeatureTable->contingentValues(m_selectedFeature, field);

  for (ContingentValue* contingentValue : contingentValuesResult->contingentValuesByFieldGroup(this).value(fieldGroupName))
  {
    if (contingentValue->contingentValueType() == ContingentValueType::ContingentCodedValue)
    {
      ContingentCodedValue* contingentCodedValue = static_cast<ContingentCodedValue*>(contingentValue);
      if (contingentCodedValue)
        contingentValuesNamesList.append(contingentCodedValue->codedValue().name());
    }
    else if (contingentValue->contingentValueType() == ContingentValueType::ContingentRangeValue)
    {
      ContingentRangeValue* contingentRangeValue = static_cast<ContingentRangeValue*>(contingentValue);
      if (contingentRangeValue)
      {
        contingentValuesNamesList.append({contingentRangeValue->minValue()});
        contingentValuesNamesList.append({contingentRangeValue->maxValue()});
        break;
      }
    }
  }

  return contingentValuesNamesList;
}

void ContingentValues::createNewEmptyFeature()
{
  if (m_gdbFeatureTable->loadStatus() != LoadStatus::Loaded)
    return;

  m_selectedFeature = static_cast<ArcGISFeature*>(m_gdbFeatureTable->createFeature(this));
  m_featureAttributes = m_selectedFeature->attributes()->attributesMap();
}

void ContingentValues::createConnections()
{
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent mouseEvent)
  {
    if (!m_nestsLayer)
      return;

    m_selectedFeature = nullptr;
    m_nestsLayer->clearSelection();
    m_featureAttributesPaneXY = {(double)mouseEvent.x(), (double)mouseEvent.y()};

    m_selectedFeature = static_cast<ArcGISFeature*>(m_gdbFeatureTable->createFeature(this));

    setFeatureAttributes(m_selectedFeature->attributes()->attributesMap());
    setFeatureAttributesPaneVisibe(true);
  });

  connect(m_geodatabase, &Geodatabase::doneLoading, this, [this]()
  {
    m_gdbFeatureTable = m_geodatabase->geodatabaseFeatureTable("BirdNests");
    if (!m_gdbFeatureTable)
      return;

    connect(m_gdbFeatureTable, &GeodatabaseFeatureTable::doneLoading, this, [this]
    {
      m_contingentValuesDefinition = m_gdbFeatureTable->contingentValuesDefinition();

      m_contingentValuesDefinition->load();

      m_nestsLayer = new FeatureLayer(m_gdbFeatureTable, this);
      m_map->operationalLayers()->append(m_nestsLayer);

      bufferFeatures();

    });

    connect(m_gdbFeatureTable, &GeodatabaseFeatureTable::queryFeaturesCompleted, this, [this](QUuid, FeatureQueryResult* results)
    {
      FeatureIterator iterator = results->iterator();

      while (iterator.hasNext())
      {
        Feature* feature = iterator.next(this);
        const double bufferDistance = feature->attributes()->attributeValue("BufferSize").toDouble();
        Polygon buffer = GeometryEngine::buffer(feature->geometry(), bufferDistance);
        m_mapView->graphicsOverlays()->at(0)->graphics()->append(new Graphic(buffer, this));
      }
    });

    connect(m_gdbFeatureTable, &GeodatabaseFeatureTable::errorOccurred, this, [](Error e)
    {
      qWarning() << e.message() << e.additionalMessage();
    });

    m_gdbFeatureTable->load();
  });
}

bool ContingentValues::validateContingentValues()
{
  if (m_geodatabase->loadStatus() != LoadStatus::Loaded ||
      m_gdbFeatureTable->loadStatus() != LoadStatus::Loaded ||
      m_contingentValuesDefinition->loadStatus() != LoadStatus::Loaded)
    return false;

  QList<ContingencyConstraintViolation*> constraintViolations = m_gdbFeatureTable->validateContingencyConstraints(m_selectedFeature);
  if (constraintViolations.isEmpty())
    return true;

  return false;
}

void ContingentValues::setFeatureAttributes(QVariantMap featureAttributes)
{
  m_featureAttributes = featureAttributes;
  emit featureAttributesChanged();
}

QVariantMap ContingentValues::featureAttributes() const
{
  return m_featureAttributes;
}

void ContingentValues::bufferFeatures()
{
  if (!m_gdbFeatureTable || m_gdbFeatureTable->loadStatus() != LoadStatus::Loaded)
    return;

  m_graphicsOverlay->graphics()->clear();

  QueryParameters params;
  params.setWhereClause("BufferSize > 0");
  m_gdbFeatureTable->queryFeatures(params);
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

void ContingentValues::setActivityValues(QVariantMap activityValues)
{
  m_activityValues = activityValues;
  emit featureAttributesChanged();
}

QList<double> ContingentValues::featureAttributesPaneXY() const
{
  return m_featureAttributesPaneXY;
}
QVariantMap ContingentValues::activityValues() const
{
  return m_activityValues;
}

QVariantMap ContingentValues::protectionValues() const
{
  return m_protectionValues;
}

void ContingentValues::createNewNest()
{
  Feature* newNest = m_gdbFeatureTable->createFeature(this);
  newNest->attributes()->setAttributesMap(m_featureAttributes);
  newNest->setGeometry(Point(m_mapView->screenToLocation(m_featureAttributesPaneXY.at(0), m_featureAttributesPaneXY.at(1))));

  m_gdbFeatureTable->addFeature(newNest);

  bufferFeatures();
}
