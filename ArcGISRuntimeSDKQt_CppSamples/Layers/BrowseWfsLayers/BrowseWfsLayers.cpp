// [WriteFile Name=BrowseWfsLayers, Category=Layers]
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

#include "BrowseWfsLayers.h"

#include "Map.h"
#include "MapQuickView.h"
#include "WfsFeatureTable.h"
#include "FeatureLayer.h"
#include "QueryParameters.h"
#include "SimpleRenderer.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "WfsService.h"
#include "WfsLayerInfo.h"

using namespace Esri::ArcGISRuntime;

BrowseWfsLayers::BrowseWfsLayers(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::topographic(this), this))
{
  // create WFS Service
  m_wfsService = new WfsService(QUrl("https://dservices2.arcgis.com/ZQgQTuoyBrtmoGdP/arcgis/services/Seattle_Downtown_Features/WFSServer?service=wfs&request=getcapabilities"),this);

  // once WFS Service is loaded, populate QStringList with layer names for ComboBox
  connect(m_wfsService, &WfsService::doneLoading, this, [this](Error e)
  {
    if (!e.isEmpty())
      return;

      m_wfsLayersInfoList = m_wfsService->serviceInfo().layerInfos();
      for (const WfsLayerInfo& i : m_wfsLayersInfoList)
        m_layerInfoTitleList.append(i.title());

      emit layerInfoTitleListChanged();
  });
  m_wfsService->load();
}

BrowseWfsLayers::~BrowseWfsLayers() = default;

void BrowseWfsLayers::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<BrowseWfsLayers>("Esri.Samples", 1, 0, "BrowseWfsLayersSample");
}

MapQuickView* BrowseWfsLayers::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void BrowseWfsLayers::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}

void BrowseWfsLayers::createWfsFeatureTable(int index, bool swap)
{
  if (m_wfsLayersInfoList.isEmpty())
    return;

  // if swapAxisOrder button is selected swap the axis otherwise reset to default(OgcAxisOrder::NoSwap or false)
  if (swap)
    m_swapAxis = !m_swapAxis;
  else
    m_swapAxis = false;

  // enable busy indicator
  m_loadingIndicator = true;
  emit isLoadingChanged();

  // clear previous layer
  m_map->operationalLayers()->clear();
  // create WFS Feature Table from selected layer
  m_wfsFeatureTable = new WfsFeatureTable(m_wfsLayersInfoList[index], this);

  // Set feature request mode to manual - only manual is supported at v100.5.
  // In this mode, you must manually populate the table - panning and zooming
  // won't request features automatically.
  m_wfsFeatureTable->setFeatureRequestMode(FeatureRequestMode::ManualCache);

  if (m_swapAxis)
    m_wfsFeatureTable->setAxisOrder(OgcAxisOrder::Swap);
  else
    m_wfsFeatureTable->setAxisOrder(OgcAxisOrder::NoSwap);

  // once WFS Feature Table is loaded, populate the table and add the layer to the map
  connect(m_wfsFeatureTable, &WfsFeatureTable::doneLoading, this, [this](Error e)
  {
    if(!e.isEmpty())
      return;

    populateWfsFeatureTable();
  });
  m_wfsFeatureTable->load();
}

void BrowseWfsLayers::populateWfsFeatureTable()
{
  if (!m_mapView || !m_wfsFeatureTable)
    return;

  // create query parameters
  QueryParameters params;
  params.setGeometry(m_mapView->visibleArea().extent());
  params.setSpatialRelationship(SpatialRelationship::Intersects);

  // query the service
  constexpr bool clearCache = false;
  const QStringList outFields {"*"};
  connect(m_wfsFeatureTable, &WfsFeatureTable::populateFromServiceCompleted, this, [this]()
  {
      addFeatureLayerToMap();
      m_mapView->setViewpointGeometry(m_wfsFeatureTable->extent());
  });
  m_wfsFeatureTable->populateFromService(params, clearCache, outFields);
}

void BrowseWfsLayers::addFeatureLayerToMap()
{
  // create feature layer from the feature table
  FeatureLayer* featureLayer = new FeatureLayer(m_wfsFeatureTable, this);
  SimpleRenderer* sr = nullptr;
  SimpleMarkerSymbol* sms = nullptr;
  SimpleLineSymbol* sls = nullptr;

  // appropriate symbology for each corresponding geometry type
  switch (m_wfsFeatureTable->geometryType()) {
  case GeometryType::Point:
    sms = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("green"), 3.0f, this);
    sr = new SimpleRenderer(sms, this);
    break;
  case GeometryType::Polygon:
    sls = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("blue"), 3.0f, this);
    sr = new SimpleRenderer(sls, this);
    break;
  case GeometryType::Polyline:
    sls = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("red"), 3.0f, this);
    sr = new SimpleRenderer(sls, this);
    break;
  default:
    qDebug() << "Error! No Renderer defined for this GeometryType";
    return;
  }
  // apply renderer to layer
  featureLayer->setRenderer(sr);

  // add the layer to the map
  m_map->operationalLayers()->append(featureLayer);
  // disable busy indicator
  m_loadingIndicator = false;
  emit isLoadingChanged();
}
