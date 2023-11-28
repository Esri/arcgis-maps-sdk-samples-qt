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
#include "SpatialReference.h"

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
  const QString templateUrl = "https://server.arcgisonline.com/arcgis/rest/services/Ocean/World_Ocean_Base/MapServer/tile/{level}/{row}/{col}.jpg";
  const QString attributionText = "Map tiles by <a href=\"https://livingatlas.arcgis.com\">ArcGIS Living Atlas of the World</a>, "
                                  "under <a href=\"https://www.esri.com/en-us/legal/terms/full-master-agreement\">Esri Master License "
                                  "Agreement</a>. Data by Esri, Garmin, GEBCO, NOAA NGDC, and other contributors.";

  // Create the WebTiledLayer with a template URL
  WebTiledLayer* webTiledLayer = new WebTiledLayer(templateUrl, this);
  webTiledLayer->setAttribution(attributionText);

  // Create a basemap from the WebTiledLayer
  Basemap* basemap = new Basemap(webTiledLayer, this);

  // Create a map using the WebTiledLayer basemap
  m_map = new Map(basemap, this);

  // Set map to map view
  m_mapView->setMap(m_map);
}
