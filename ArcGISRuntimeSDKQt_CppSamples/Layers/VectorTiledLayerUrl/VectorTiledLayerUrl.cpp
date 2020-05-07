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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

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

VectorTiledLayerUrl::~VectorTiledLayerUrl() = default;

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
  ArcGISVectorTiledLayer* vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl("https://www.arcgis.com/home/item.html?id=7675d44bb1e4428aa2c30a9b68f97822"), this);
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
  if (m_map && m_map->loadStatus() == LoadStatus::Loaded)
  {
    ArcGISVectorTiledLayer* vectorTiledLayer = nullptr;
    if (basemap == "Mid-Century")
      vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl("https://www.arcgis.com/home/item.html?id=7675d44bb1e4428aa2c30a9b68f97822"), this);
    else if (basemap == "Colored Pencil")
      vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl("https://www.arcgis.com/home/item.html?id=4cf7e1fb9f254dcda9c8fbadb15cf0f8"), this);
    else if (basemap == "Newspaper")
      vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl("https://www.arcgis.com/home/item.html?id=dfb04de5f3144a80bc3f9f336228d24a"), this);
    else if (basemap == "Nova")
      vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl("https://www.arcgis.com/home/item.html?id=75f4dfdff19e445395653121a95a85db"), this);
    else if (basemap == "World Street Map (Night)")
      vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl("https://www.arcgis.com/home/item.html?id=86f556a2d1fd468181855a35e344567f"), this);
    else
      vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl("https://www.arcgis.com/home/item.html?id=7675d44bb1e4428aa2c30a9b68f97822"), this);
    Basemap* basemap = new Basemap(vectorTiledLayer, this);
    m_map->setBasemap(basemap);
  }
}

