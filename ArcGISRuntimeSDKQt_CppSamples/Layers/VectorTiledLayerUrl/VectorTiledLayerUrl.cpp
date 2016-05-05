// [WriteFile Name=VectorTiledLayerUrl, Category=Maps]
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
    QQuickItem(parent),
    m_map(nullptr),
    m_mapView(nullptr)
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
    // create a vector tiled basemap
    ArcGISVectorTiledLayer* vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl("http://www.arcgis.com/home/item.html?id=00cd8e843bae49b3a040423e5d65416b"));
    Basemap* basemap = new Basemap(vectorTiledLayer, this);
    // create a new map instance
    m_map = new Map(basemap, this);
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
            vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl("http://www.arcgis.com/home/item.html?id=00cd8e843bae49b3a040423e5d65416b"));
        else if (basemap == "Streets")
            vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl("http://www.arcgis.com/home/item.html?id=3b8814f6ddbd485cae67e8018992246e"));
        else if (basemap == "Night")
            vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl("http://www.arcgis.com/home/item.html?id=f96366254a564adda1dc468b447ed956"));
        else if (basemap == "Dark Gray")
            vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl("http://www.arcgis.com/home/item.html?id=3e3099d7302f4d99bc6f916dcc07ed59"));
        Basemap* basemap = new Basemap(vectorTiledLayer, this);
        m_map->setBasemap(basemap);
    }
}

