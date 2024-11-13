// [WriteFile Name=ArcGISTiledLayerUrl, Category=Layers]
// [Legal]
// Copyright 2016 Esri.

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

#include "ArcGISTiledLayerUrl.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "ArcGISTiledLayer.h"
#include <QUrl>

using namespace Esri::ArcGISRuntime;

ArcGISTiledLayerUrl::ArcGISTiledLayerUrl(QQuickItem* parent) :
  QQuickItem(parent)
{
}

ArcGISTiledLayerUrl::~ArcGISTiledLayerUrl() = default;

void ArcGISTiledLayerUrl::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ArcGISTiledLayerUrl>("Esri.Samples", 1, 0, "ArcGISTiledLayerUrlSample");
}

void ArcGISTiledLayerUrl::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  //! [display tiled layer from tiled map service]
  // create a new tiled layer
  ArcGISTiledLayer* tiledLayer = new ArcGISTiledLayer(QUrl("https://services.arcgisonline.com/arcgis/rest/services/World_Topo_Map/MapServer"), this);
  // create a new basemap instance with the tiled layer
  Basemap* basemap = new Basemap(tiledLayer, this);
  // create a new map instance
  m_map = new Map(basemap, this);
  // set map on the map view
  m_mapView->setMap(m_map);
  //! [display tiled layer from tiled map service]
}
