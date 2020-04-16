// [WriteFile Name=TraceUtilityNetwork, Category=UtilityNetwork]
// [Legal]
// Copyright 2019 Esri.

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

#include "TraceUtilityNetwork.h"

#include "ArcGISFeatureListModel.h"
#include "FeatureLayer.h"
#include "GeometryEngine.h"
#include "Map.h"
#include "MapQuickView.h"
#include "ServiceFeatureTable.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleRenderer.h"
#include "UniqueValueRenderer.h"
#include "UtilityAssetGroup.h"
#include "UtilityAssetType.h"
#include "UtilityDomainNetwork.h"
#include "UtilityElement.h"
#include "UtilityElementTraceResult.h"
#include "UtilityNetwork.h"
#include "UtilityNetworkDefinition.h"
#include "UtilityNetworkSource.h"
#include "UtilityNetworkTypes.h"
#include "UtilityTerminalConfiguration.h"
#include "UtilityTier.h"
#include "UtilityTraceParameters.h"
#include "UtilityTraceResultListModel.h"

using namespace Esri::ArcGISRuntime;

TraceUtilityNetwork::TraceUtilityNetwork(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::streetsNightVector(this), this)),
  m_startingSymbol(new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Cross, QColor(Qt::green), 20, this)),
  m_barrierSymbol(new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::X, QColor(Qt::red), 20, this)),
  m_mediumVoltageSymbol(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(Qt::darkCyan), 3, this)),
  m_lowVoltageSymbol(new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, QColor(Qt::darkCyan), 3, this)),
  m_graphicParent(new QObject())
{
  m_map->setInitialViewpoint(Viewpoint(Envelope(-9813547.35557238, 5129980.36635111, -9813185.0602376, 5130215.41254146, SpatialReference::webMercator())));

  m_deviceFeatureTable = new ServiceFeatureTable(QUrl("https://sampleserver7.arcgisonline.com/arcgis/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer/100"), this);
  m_deviceLayer = new FeatureLayer(m_deviceFeatureTable, this);
  connect(m_deviceLayer, &FeatureLayer::selectFeaturesCompleted, this, [this](QUuid)
  {
    m_busy = false;
    emit busyChanged();
  });

  m_lineFeatureTable = new ServiceFeatureTable(QUrl("https://sampleserver7.arcgisonline.com/arcgis/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer/115"), this);
  m_lineLayer = new FeatureLayer(m_lineFeatureTable, this);

  // create unique renderer
  m_uniqueValueRenderer = new UniqueValueRenderer(this);
  m_uniqueValueRenderer->setFieldNames(QStringList("ASSETGROUP"));
  UniqueValue* mediumVoltageUniqueValue = createUniqueValue(QString("Medium Voltage"), m_mediumVoltageSymbol, 5);
  UniqueValue* lowVoltageUniqueValue = createUniqueValue(QString("Low Voltage"), m_lowVoltageSymbol, 3);

  // append to UniqueValueRenderer
  m_uniqueValueRenderer->uniqueValues()->append(mediumVoltageUniqueValue);
  m_uniqueValueRenderer->uniqueValues()->append(lowVoltageUniqueValue);

  // set unique value renderer to the line layer
  m_lineLayer->setRenderer(m_uniqueValueRenderer);

  // Add electric distribution lines and electric devices layers
  m_map->operationalLayers()->append(m_lineLayer);
  m_map->operationalLayers()->append(m_deviceLayer);

  connect(m_map, &Map::doneLoading, this, [this](Error e)
  {
    if (!e.isEmpty())
    {
      m_dialogText = QString(e.message() + " - " + e.additionalMessage());
      emit dialogVisibleChanged();
      return;
    }

    m_graphicsOverlay = new GraphicsOverlay(this);
    m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

    m_utilityNetwork = new UtilityNetwork(m_serviceUrl, m_map, this);

    connect(m_utilityNetwork, &UtilityNetwork::errorOccurred, [this](Error e)
    {
      if (e.isEmpty())
        return;

      m_dialogText = QString(e.message() + " - " + e.additionalMessage());
      emit dialogTextChanged();
    });

    // select the features returned from the trace
    connect(m_utilityNetwork, &UtilityNetwork::traceCompleted, this, &TraceUtilityNetwork::onTraceCompleted);

    connect(m_utilityNetwork, &UtilityNetwork::doneLoading, [this](Error e)
    {
      m_busy = false;
      emit busyChanged();

      if (!e.isEmpty())
      {
        m_dialogText = QString(e.message() + " - " + e.additionalMessage());
        emit dialogTextChanged();
        return;
      }

      connectSignals();
    });

    m_utilityNetwork->load();
    m_busy = true;
    emit busyChanged();
  });
}

void TraceUtilityNetwork::connectSignals()
{
  // identify layers on mouse click
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    if (m_map->loadStatus() != LoadStatus::Loaded)
      return;

    constexpr double tolerance = 10.0;
    constexpr bool returnPopups = false;
    m_clickPoint = m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y());
    m_mapView->identifyLayers(mouseEvent.x(), mouseEvent.y(), tolerance, returnPopups);
  });

  // handle the identify results
  connect(m_mapView, &MapQuickView::identifyLayersCompleted, this, &TraceUtilityNetwork::onIdentifyLayersCompleted);
}

TraceUtilityNetwork::~TraceUtilityNetwork() = default;

void TraceUtilityNetwork::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<TraceUtilityNetwork>("Esri.Samples", 1, 0, "TraceUtilityNetworkSample");
}

MapQuickView* TraceUtilityNetwork::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void TraceUtilityNetwork::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}

void TraceUtilityNetwork::multiTerminalIndex(int index)
{
  if (m_terminals.isEmpty())
    return;

  if (!m_feature)
    return;

  UtilityElement* element = m_utilityNetwork->createElementWithArcGISFeature(m_feature, m_terminals[index]);
  updateTraceParams(element);
}

void TraceUtilityNetwork::updateTraceParams(UtilityElement* element)
{
  if (m_startingLocationsEnabled)
  {
    m_startingLocations.append(element);
    Graphic* traceLocation = new Graphic(m_clickPoint, m_startingSymbol, m_graphicParent.get());
    m_graphicsOverlay->graphics()->append(traceLocation);
  }
  else
  {
    m_barriers.append(element);
    Graphic* traceLocation = new Graphic(m_clickPoint, m_barrierSymbol, m_graphicParent.get());
    m_graphicsOverlay->graphics()->append(traceLocation);
  }
}

void TraceUtilityNetwork::trace(int index)
{
  m_busy = true;
  emit busyChanged();

  delete m_traceParams;

  switch (index)
  {
    case 0:
      m_traceParams = new UtilityTraceParameters(UtilityTraceType::Connected, {}, this);
      break;
    case 1:
      m_traceParams = new UtilityTraceParameters(UtilityTraceType::Subnetwork, {}, this);
      break;
    case 2:
      m_traceParams = new UtilityTraceParameters(UtilityTraceType::Upstream, {}, this);
      break;
    case 3:
      m_traceParams = new UtilityTraceParameters(UtilityTraceType::Downstream, {}, this);
      break;
    default:
      return;
  }

  if (m_mediumVoltageTier)
    m_traceParams->setTraceConfiguration(m_mediumVoltageTier->traceConfiguration());

  m_traceParams->setStartingLocations(m_startingLocations);
  m_traceParams->setBarriers(m_barriers);
  // Perform a connected trace on the utility network
  m_utilityNetwork->trace(m_traceParams);
}

void TraceUtilityNetwork::reset()
{
  m_startingLocations.clear();
  m_barriers.clear();
  if (m_traceParams)
  {
    m_traceParams->setStartingLocations(m_startingLocations);
    m_traceParams->setBarriers(m_barriers);
  }

  m_graphicsOverlay->graphics()->clear();
  m_graphicParent.reset(new QObject());

  for (Layer* layer : *m_map->operationalLayers())
  {
    FeatureLayer* featureLayer = dynamic_cast<FeatureLayer*>(layer);
    if (!featureLayer)
      return;

    featureLayer->clearSelection();
  }
}

void TraceUtilityNetwork::onIdentifyLayersCompleted(QUuid, const QList<IdentifyLayerResult*>& results)
{
  if (results.isEmpty())
  {
    m_dialogText = QString("Could not identify location.");
    emit dialogTextChanged();
    m_dialogVisible = true;
    emit dialogVisibleChanged();
    return;
  }

  // Get domain network
  const UtilityDomainNetwork* domainNetwork = m_utilityNetwork->definition()->domainNetwork("ElectricDistribution");
  m_mediumVoltageTier = domainNetwork->tier("Medium Voltage Radial");

  const IdentifyLayerResult* result = results[0];
  m_feature = static_cast<ArcGISFeature*>(result->geoElements()[0]);
  UtilityElement* element = nullptr;
  const UtilityNetworkSource* networkSource = m_utilityNetwork->definition()->networkSource(m_feature->featureTable()->tableName());

  if (networkSource->sourceType() == UtilityNetworkSourceType::Junction)
  {
    m_junctionSelected = true;
    emit junctionSelectedChanged();

    const QString assetGroupFieldName = static_cast<ArcGISFeatureTable*>(m_feature->featureTable())->subtypeField();
    const int assetGroupCode = m_feature->attributes()->attributeValue(assetGroupFieldName).toInt();
    UtilityAssetGroup* assetGroup = nullptr;

    for (UtilityAssetGroup* group : networkSource->assetGroups())
    {
      if (group->code() == assetGroupCode)
      {
        assetGroup = group;
        break;
      }
    }

    const int assetTypeCode = m_feature->attributes()->attributeValue("assettype").toInt();

    UtilityAssetType* assetType = nullptr;
    for (UtilityAssetType* type : assetGroup->assetTypes())
    {
      if (type->code() == assetTypeCode)
      {
        assetType = type;
        break;
      }
    }

    if (!assetType)
      return;

    m_terminals = assetType->terminalConfiguration()->terminals();

    if (m_terminals.size() > 1)
    {
      m_terminalDialogVisisble = true;
      emit terminalDialogVisisbleChanged();
      return;
    }
    else if (m_terminals.size() == 1)
    {
      element = m_utilityNetwork->createElementWithArcGISFeature(m_feature, m_terminals[0]);
    }
    else
    {
      return;
    }

  }
  else if (networkSource->sourceType() == UtilityNetworkSourceType::Edge)
  {
    m_junctionSelected = false;
    emit junctionSelectedChanged();

    element = m_utilityNetwork->createElementWithArcGISFeature(m_feature, nullptr, this);

    // Compute how far tapped location is along the edge feature.
    if (m_feature->geometry().geometryType() == GeometryType::Polyline)
    {
      const Polyline line = GeometryEngine::removeZ(m_feature->geometry());
      // Set how far the element is along the edge.
      element->setFractionAlongEdge(GeometryEngine::fractionAlong(line, m_clickPoint, -1));
      m_fractionAlongEdge = element->fractionAlongEdge();
      emit fractionAlongEdgeChanged();
    }
  }
  else
  {
    return;
  }

  updateTraceParams(element);
}

void TraceUtilityNetwork::onTraceCompleted()
{
  m_dialogVisible = true;
  emit dialogVisibleChanged();

  if (m_utilityNetwork->traceResult()->isEmpty())
  {
    m_busy = false;
    emit busyChanged();
    return;
  }

  m_dialogText = QString("Trace completed.");
  emit dialogTextChanged();

  UtilityTraceResult* result = m_utilityNetwork->traceResult()->at(0);

  const QList<UtilityElement*> elements = static_cast<UtilityElementTraceResult*>(result)->elements(this);

  QueryParameters deviceParams;
  QueryParameters lineParams;
  QList<qint64> deviceObjIds;
  QList<qint64> lineObjIds;

  for (UtilityElement* item : elements)
  {
    if (item->networkSource()->name() == "Electric Distribution Device")
    {
      deviceObjIds.append(item->objectId());
    }
    else if (item->networkSource()->name() == "Electric Distribution Line")
    {
      lineObjIds.append(item->objectId());
    }
  }

  deviceParams.setObjectIds(deviceObjIds);
  lineParams.setObjectIds(lineObjIds);

  m_deviceLayer->selectFeatures(deviceParams, SelectionMode::Add);
  m_lineLayer->selectFeatures(lineParams, SelectionMode::Add);
}

UniqueValue* TraceUtilityNetwork::createUniqueValue(const QString& label, Esri::ArcGISRuntime::Symbol* fillSymbol, int value)
{
  // add state's attribute value for field "STATE_ABBR" to QVariantList
  QVariantList labelValue;
  labelValue.append(value);

  // set value for a State to be rendered. (label, description, attribute value list, symbol, parent)
  UniqueValue* uniqueValue = new UniqueValue(label, "", labelValue, fillSymbol, this);

  // return Unique value created
  return uniqueValue;
}
