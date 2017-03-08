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

void VectorTiledLayerUrl::componentComplete()
{
    QQuickItem::componentComplete();

    // find QML MapView component
    m_mapView = findChild<MapQuickView*>("mapView");

    //! [display vector tiled layer]
    // create a vector tiled basemap
    ArcGISVectorTiledLayer* vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl("http://www.arcgis.com/home/item.html?id=dcbbba0edf094eaa81af19298b9c6247"));
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

void VectorTiledLayerUrl::changeBasemap(QString basemap)
{
    if (m_map->loadStatus() == LoadStatus::Loaded)
    {
        ArcGISVectorTiledLayer* vectorTiledLayer = nullptr;
        if (basemap == "Navigation")
            vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl("http://www.arcgis.com/home/item.html?id=dcbbba0edf094eaa81af19298b9c6247"));
        else if (basemap == "Streets")
            vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl("http://www.arcgis.com/home/item.html?id=4e1133c28ac04cca97693cf336cd49ad"));
        else if (basemap == "Night")
            vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl("http://www.arcgis.com/home/item.html?id=bf79e422e9454565ae0cbe9553cf6471"));
        else if (basemap == "Dark Gray")
            vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl("http://www.arcgis.com/home/item.html?id=850db44b9eb845d3bd42b19e8aa7a024"));
        Basemap* basemap = new Basemap(vectorTiledLayer, this);
        m_map->setBasemap(basemap);
    }
}

