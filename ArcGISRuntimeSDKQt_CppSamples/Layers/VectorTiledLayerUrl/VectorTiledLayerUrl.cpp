// [WriteFile Name=VectorTiledLayerUrl, Category=Layers]
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

#include "VectorTiledLayerUrl.h"

#include <QUrl>

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "ArcGISVectorTiledLayer.h"
#include "Point.h"
#include "Viewpoint.h"

using namespace Esri::ArcGISRuntime;

VectorTiledLayerUrl::VectorTiledLayerUrl(QQuickItem* parent) :
  QQuickItem(parent)
{
}

VectorTiledLayerUrl::~VectorTiledLayerUrl()
{
}

void VectorTiledLayerUrl::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<VectorTiledLayerUrl>("Esri.Samples", 1, 0, "VectorTiledLayerUrlSample");
}

void VectorTiledLayerUrl::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  //! [display vector tiled layer]
  // create a vector tiled basemap
  ArcGISVectorTiledLayer* vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl("http://www.arcgis.com/home/item.html?id=e19e9330bf08490ca8353d76b5e2e658"), this);
  Basemap* basemap = new Basemap(vectorTiledLayer, this);
  // create a new map instance
  m_map = new Map(basemap, this);
  //! [display vector tiled layer]

  // set viewpoint
  Point ptLocation(-80.18, 25.778135, SpatialReference(4236));
  m_map->setInitialViewpoint(Viewpoint(ptLocation, 150000));
  // set map on the map view
  m_mapView->setMap(m_map);
}

void VectorTiledLayerUrl::changeBasemap(const QString& basemap)
{
  if (m_map->loadStatus() == LoadStatus::Loaded)
  {
    ArcGISVectorTiledLayer* vectorTiledLayer = nullptr;
    if (basemap == "Streets")
      vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl("http://www.arcgis.com/home/item.html?id=a60a37a27cc140ddad15f919cd5a69f2"));
    else if (basemap == "Night")
      vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl("http://www.arcgis.com/home/item.html?id=92c551c9f07b4147846aae273e822714"));
    else if (basemap == "Dark Gray")
      vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl("http://www.arcgis.com/home/item.html?id=5ad3948260a147a993ef4865e3fad476"));
    else
      vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl("http://www.arcgis.com/home/item.html?id=e19e9330bf08490ca8353d76b5e2e658"));
    Basemap* basemap = new Basemap(vectorTiledLayer, this);
    m_map->setBasemap(basemap);
  }
}

