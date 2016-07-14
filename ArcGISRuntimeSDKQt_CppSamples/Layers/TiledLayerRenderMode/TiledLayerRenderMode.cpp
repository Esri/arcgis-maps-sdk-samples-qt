// [WriteFile Name=TiledLayerRenderMode, Category=Layers]
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

#include "TiledLayerRenderMode.h"

#include "ArcGISTiledLayer.h"
#include "Basemap.h"
#include "Map.h"
#include "MapQuickView.h"
#include "Point.h"
#include "SpatialReference.h"
#include "Viewpoint.h"

#include <QUrl>

using namespace Esri::ArcGISRuntime;

TiledLayerRenderMode::TiledLayerRenderMode(QQuickItem* parent) :
    QQuickItem(parent),
    m_map(nullptr),
    m_mapView(nullptr),
    m_tiledLayer(nullptr)
{
}

TiledLayerRenderMode::~TiledLayerRenderMode()
{
}

void TiledLayerRenderMode::componentComplete()
{
    QQuickItem::componentComplete();

    // find QML MapView component
    m_mapView = findChild<MapQuickView*>("mapView");
    m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

    // create ArcGISTiledLayer Basemap with initial RenderMode Aesthetics
    m_tiledLayer = new ArcGISTiledLayer(QUrl("http://services.arcgisonline.com/arcgis/rest/services/World_Topo_Map/MapServer"), this);
    m_tiledLayer->setRenderMode(RenderMode::Aesthetics);
    Basemap* basemap = new Basemap(m_tiledLayer, this);

    // create a map using the Basemap
    m_map = new Map(basemap, this);

    // set initial viewpoint
    m_map->setInitialViewpoint(Viewpoint(Point(-13630860, 4548447, SpatialReference(102100)), 86677));

    // Set map to map view
    m_mapView->setMap(m_map);
}

void TiledLayerRenderMode::changeRenderMode()
{
    if (m_tiledLayer->renderMode() == RenderMode::Aesthetics)
        m_tiledLayer->setRenderMode(RenderMode::Scale);
    else
        m_tiledLayer->setRenderMode(RenderMode::Aesthetics);
}
