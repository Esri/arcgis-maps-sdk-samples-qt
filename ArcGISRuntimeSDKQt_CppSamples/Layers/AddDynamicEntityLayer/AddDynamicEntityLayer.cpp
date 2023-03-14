// COPYRIGHT 2023 ESRI
// TRADE SECRETS: ESRI PROPRIETARY AND CONFIDENTIAL
// Unpublished material - all rights reserved under the
// Copyright Laws of the United States and applicable international
// laws, treaties, and conventions.
//
// For additional information, contact:
// Environmental Systems Research Institute, Inc.
// Attn: Contracts and Legal Services Department
// 380 New York Street
// Redlands, California, 92373
// USA
//
// email: contracts@esri.com
/// \file AddDynamicEntityLayer.cpp

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

#include "AddDynamicEntityLayer.h"

#include "ArcGISStreamService.h"
#include "ArcGISStreamServiceFilter.h"
#include "DynamicEntityDataSourcePurgeOptions.h"
#include "DynamicEntityLayer.h"
#include "Envelope.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "SimpleLineSymbol.h"
#include "SpatialReference.h"
#include "SymbolTypes.h"
#include "TrackDisplayProperties.h"
#include "Viewpoint.h"

using namespace Esri::ArcGISRuntime;

namespace
{
// This envelope is a limited region around Sandy, Utah. It will be the extent used by the `DynamicEntityFilter`.
auto utahSandyEnvelope = Envelope(
      Point(-112.110052, 40.718083, SpatialReference::wgs84()),
      Point(-111.814782, 40.535247, SpatialReference::wgs84())
      );
}

AddDynamicEntityLayer::AddDynamicEntityLayer(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISDarkGray, this))
{
  QUrl streamServiceUrl("https://realtimegis2016.esri.com:6443/arcgis/rest/services/SandyVehicles/StreamServer");
  auto dynamicEntityDataSource = new ArcGISStreamService(streamServiceUrl, this);
  auto streamServiceFilter = new ArcGISStreamServiceFilter(this);
  streamServiceFilter->setGeometry(utahSandyEnvelope);
  streamServiceFilter->setWhereClause("speed > 0");
  dynamicEntityDataSource->setFilter(streamServiceFilter);
  dynamicEntityDataSource->purgeOptions()->setMaximumDuration(0.0);
  dynamicEntityDataSource->purgeOptions()->setMaximumObservations(0);
  dynamicEntityDataSource->purgeOptions()->setMaximumObservationsPerTrack(0);

  auto dynamicEntityLayer = new DynamicEntityLayer(dynamicEntityDataSource, this);

  auto trackDisplayProperties = dynamicEntityLayer->trackDisplayProperties();

  trackDisplayProperties->setShowTrackLine(true);

  dynamicEntityLayer->trackDisplayProperties()->setShowTrackLine(true);
  dynamicEntityLayer->trackDisplayProperties()->setShowPreviousObservations(true);
  dynamicEntityLayer->trackDisplayProperties()->setMaximumObservations(100);
  dynamicEntityLayer->trackDisplayProperties()->setMaximumDuration(0);

  m_map->operationalLayers()->append(dynamicEntityLayer);

}

AddDynamicEntityLayer::~AddDynamicEntityLayer() = default;

void AddDynamicEntityLayer::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<AddDynamicEntityLayer>("Esri.Samples", 1, 0, "AddDynamicEntityLayerSample");
}

MapQuickView* AddDynamicEntityLayer::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void AddDynamicEntityLayer::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  auto m_borderOverlay = new GraphicsOverlay(this);

  auto borderGraphic = new Graphic(utahSandyEnvelope, new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, Qt::red, 2, this), this);

  m_borderOverlay->graphics()->append(borderGraphic);

  m_mapView->graphicsOverlays()->append(m_borderOverlay);

  m_mapView->setViewpointAndWait(Viewpoint(utahSandyEnvelope));

  emit mapViewChanged();
}
