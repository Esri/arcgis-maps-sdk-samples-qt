// [WriteFile Name=Web_Tiled_Layer, Category=Layers]
// [Legal]
// Copyright 2017 Esri.

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

#include "Web_Tiled_Layer.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "WebTiledLayer.h"
#include "Viewpoint.h"
#include "Point.h"

#include <QStringList>

using namespace Esri::ArcGISRuntime;

Web_Tiled_Layer::Web_Tiled_Layer(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

Web_Tiled_Layer::~Web_Tiled_Layer() = default;

void Web_Tiled_Layer::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<Web_Tiled_Layer>("Esri.Samples", 1, 0, "Web_Tiled_LayerSample");
}

void Web_Tiled_Layer::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Set up the tiled layer parameters
  const QString templateUrl = "https://stamen-tiles-{subDomain}.a.ssl.fastly.net/terrain/{level}/{col}/{row}.png";
  const QStringList subDomains { "a", "b", "c", "d" };
  const QString attributionText = "Map tiles by <a href=\"https://stamen.com\">Stamen Design</a>, "
                                  "under <a href=\"https://creativecommons.org/licenses/by/3.0\">CC BY 3.0</a>. "
                                  "Data by <a href=\"https://openstreetmap.org\">OpenStreetMap</a>, "
                                  "under <a href=\"https://www.openstreetmap.org/copyright\">ODbL</a>.";

  // Create the WebTiledLayer with a template URL, sub domains, and copyright information
  WebTiledLayer* webTiledLayer = new WebTiledLayer(templateUrl, subDomains, this);
  webTiledLayer->setAttribution(attributionText);

  // Create a basemap from the WebTiledLayer
  Basemap* basemap = new Basemap(webTiledLayer, this);

  // Create a map using the WebTiledLayer basemap
  m_map = new Map(basemap, this);

  // Set an initial viewpoint
  Point pt(-13167861.0, 4382202.0, SpatialReference::webMercator());
  Viewpoint vp(pt, 50000.0);
  m_map->setInitialViewpoint(vp);

  // Set map to map view
  m_mapView->setMap(m_map);
}
