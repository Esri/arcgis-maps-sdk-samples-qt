// [WriteFile Name=ArcGISMapImageLayerUrl, Category=Layers]
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

#include "ArcGISMapImageLayerUrl.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "ArcGISMapImageLayer.h"
#include <QUrl>

using namespace Esri::ArcGISRuntime;

ArcGISMapImageLayerUrl::ArcGISMapImageLayerUrl(QQuickItem* parent) :
  QQuickItem(parent)
{
}

ArcGISMapImageLayerUrl::~ArcGISMapImageLayerUrl() = default;

void ArcGISMapImageLayerUrl::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ArcGISMapImageLayerUrl>("Esri.Samples", 1, 0, "ArcGISMapImageLayerUrlSample");
}

void ArcGISMapImageLayerUrl::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // create a new map image layer
  ArcGISMapImageLayer* mapImageLayer = new ArcGISMapImageLayer(QUrl("https://sampleserver5.arcgisonline.com/arcgis/rest/services/Elevation/WorldElevations/MapServer"), this);

  // create a new basemap from the layer
  Basemap* basemap = new Basemap(mapImageLayer, this);

  // create a new map instance
  m_map = new Map(basemap, this);

  // set map on the map view
  m_mapView->setMap(m_map);
}

