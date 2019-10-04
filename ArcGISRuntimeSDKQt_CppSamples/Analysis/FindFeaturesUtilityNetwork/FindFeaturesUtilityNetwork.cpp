// [WriteFile Name=FindFeaturesUtilityNetwork, Category=Analysis]
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

#include "FindFeaturesUtilityNetwork.h"

#include "Map.h"
#include "MapQuickView.h"
#include "ServiceFeatureTable.h"
#include "FeatureLayer.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleRenderer.h"
#include "UtilityElement.h"
#include "UtilityElementTraceResult.h"
#include "UtilityNetwork.h"
#include "UtilityNetworkDefinition.h"
#include "UtilityNetworkSource.h"
#include "UtilityNetworkTypes.h"
#include "UtilityTraceParameters.h"
#include "UtilityTraceResultListModel.h"
#include "UtilityAssetGroup.h"
#include "UtilityAssetType.h"
#include "UtilityTerminalConfiguration.h"
#include "ArcGISFeatureListModel.h"
#include "GeometryEngine.h"

using namespace Esri::ArcGISRuntime;

FindFeaturesUtilityNetwork::FindFeaturesUtilityNetwork(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::streetsNightVector(this), this)),
  m_startingSymbol(new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Cross, QColor(Qt::green), 20, this)),
  m_barrierSymbol(new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::X, QColor(Qt::red), 20, this)),
  m_lineSymbol(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(Qt::darkCyan), 3, this)),
  m_graphicParent(new QObject())

{
  m_map->setInitialViewpoint(Viewpoint(Envelope(-9813547.35557238, 5129980.36635111, -9813185.0602376, 5130215.41254146, SpatialReference::webMercator())));

  m_deviceFeatureTable = new ServiceFeatureTable(QUrl("https://sampleserver7.arcgisonline.com/arcgis/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer/100"), this);
  m_deviceLayer = new FeatureLayer(m_deviceFeatureTable, this);
  connect(m_deviceLayer, &FeatureLayer::m_Completed, this, [this](QUuid)
  {
    m_busy = false;
    emit busyChanged();
  });

  m_lineFeatureTable = new ServiceFeatureTable(QUrl("https://sampleserver7.arcgisonline.com/arcgis/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer/115"), this);
  m_lineLayer = new FeatureLayer(m_lineFeatureTable, this);
  m_lineLayer->setRenderer(new SimpleRenderer(m_lineSymbol, this));

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
    m_traceParams = new UtilityTraceParameters(UtilityTraceType::Connected, {}, this);

    // select the features returned from the trace
    connect(m_utilityNetwork, &UtilityNetwork::traceCompleted, this, &FindFeaturesUtilityNetwork::onTraceComplete);

    connect(m_utilityNetwork, &UtilityNetwork::doneLoading, [this](Error e)
    {
      m_busy = false;
      emit busyChanged();

      if (!e.isEmpty())
      {
        m_dialogText = QString(e.message() + " - " + e.additionalMessage());
        emit dialogVisibleChanged();
        return;
      }

      connectSignals();
    });

    m_utilityNetwork->load();
    m_busy = true;
    emit busyChanged();
  });
}

void FindFeaturesUtilityNetwork::connectSignals()
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
  connect(m_mapView, &MapQuickView::identifyLayersCompleted, this, &FindFeaturesUtilityNetwork::onIdentifyLayersCompleted);
}

FindFeaturesUtilityNetwork::~FindFeaturesUtilityNetwork() = default;

void FindFeaturesUtilityNetwork::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<FindFeaturesUtilityNetwork>("Esri.Samples", 1, 0, "FindFeaturesUtilityNetworkSample");
}

MapQuickView* FindFeaturesUtilityNetwork::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void FindFeaturesUtilityNetwork::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}

void FindFeaturesUtilityNetwork::multiTerminalIndex(int index)
{
  if (m_terminals.isEmpty())
    return;

  if (!m_feature)
    return;

  UtilityElement* element = m_utilityNetwork->createElementWithArcGISFeature(m_feature, m_terminals[index]);
  updateTraceParams(element);
}

void FindFeaturesUtilityNetwork::updateTraceParams(UtilityElement* element)
{
  if (m_startingLocationsEnabled)
  {
    m_startingLocations.append(element);
    m_traceParams->setStartingLocations(m_startingLocations);
    Graphic* traceLocation = new Graphic(m_clickPoint, m_startingSymbol, m_graphicParent.get());
    m_graphicsOverlay->graphics()->append(traceLocation);
  }
  else
  {
    m_barriers.append(element);
    m_traceParams->setBarriers(m_barriers);
    Graphic* traceLocation = new Graphic(m_clickPoint, m_barrierSymbol, m_graphicParent.get());
    m_graphicsOverlay->graphics()->append(traceLocation);
  }
}

void FindFeaturesUtilityNetwork::trace()
{
  m_busy = true;
  emit busyChanged();
  // Perform a connected trace on the utility network
  m_utilityNetwork->trace(m_traceParams);
}

void FindFeaturesUtilityNetwork::reset()
{
  m_startingLocations.clear();
  m_traceParams->setStartingLocations(m_startingLocations);
  m_barriers.clear();
  m_traceParams->setBarriers(m_barriers);
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

void FindFeaturesUtilityNetwork::onIdentifyLayersCompleted(QUuid, const QList<IdentifyLayerResult*>& results)
{
  if (results.isEmpty())
  {
    m_dialogText = QString("Could not identify location.");
    emit dialogTextChanged();
    m_dialogVisible = true;
    emit dialogVisibleChanged();
    return;
  }

  const IdentifyLayerResult* result = results[0];
  m_feature = static_cast<ArcGISFeature*>(result->geoElements()[0]);
  UtilityElement* element = nullptr;
  const UtilityNetworkSource* networkSource = m_utilityNetwork->definition()->networkSource(m_feature->featureTable()->tableName());

  if (networkSource->sourceType() == UtilityNetworkSourceType::Junction)
  {
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
    element = m_utilityNetwork->createElementWithArcGISFeature(m_feature, nullptr, this);

    // Compute how far tapped location is along the edge feature.
    if (m_feature->geometry().geometryType() == GeometryType::Polyline)
    {
      const Polyline line = GeometryEngine::removeZ(m_feature->geometry());
      // Set how far the element is along the edge.
      element->setFractionAlongEdge(GeometryEngine::fractionAlong(line, m_clickPoint, -1));
    }
  }
  else
  {
    return;
  }

  updateTraceParams(element);
}

void FindFeaturesUtilityNetwork::onTraceComplete()
{
  m_dialogVisible = true;
  emit dialogVisibleChanged();

  if (m_utilityNetwork->traceResult()->isEmpty())
  {
    m_busy = false;
    m_dialogText = QString("Trace complete with no results.");
    emit dialogTextChanged();
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
