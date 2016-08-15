// [WriteFile Name=OfflineGeocode, Category=Search]
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

#include "OfflineGeocode.h"

#include "ArcGISTiledLayer.h"
#include "Map.h"
#include "MapQuickView.h"

#include <QQmlProperty>

using namespace Esri::ArcGISRuntime;

OfflineGeocode::OfflineGeocode(QQuickItem* parent /* = nullptr */):
    QQuickItem(parent),
    m_map(nullptr),
    m_mapView(nullptr),
    m_tiledLayer(nullptr)
{
}

OfflineGeocode::~OfflineGeocode()
{
}

void OfflineGeocode::componentComplete()
{
    QQuickItem::componentComplete();

    // find QML MapView component
    m_mapView = findChild<MapQuickView*>("mapView");
    m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

    m_dataPath = QQmlProperty::read(this, "dataPath").toString();

    // create a basemap using a tiled layer
    TileCache* tileCache = new TileCache(m_dataPath + "/tpk/streetmap_SD.tpk");
    m_tiledLayer = new ArcGISTiledLayer(tileCache, this);

    Basemap* basemap = new Basemap(this);
    basemap->baseLayers()->append(m_tiledLayer);

    // create map using basemap
    m_map = new Map(basemap);
    m_map->setInitialViewpoint(Viewpoint(Point(-13042254.715252, 3857970.236806, SpatialReference(3857)), 2e4));

    // Set map to map view
    m_mapView->setMap(m_map);
}
