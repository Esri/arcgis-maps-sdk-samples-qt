// [WriteFile Name=ArcGISTiledLayerUrl, Category=Layers]
// [Legal]
// Copyright 2015 Esri.

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

#include "ArcGISTiledLayerUrl.h"
#include "Map.h"
#include "MapGraphicsView.h"
#include "Basemap.h"
#include "ArcGISTiledLayer.h"
#include <QUrl>
#include <QVBoxLayout>

using namespace Esri::ArcGISRuntime;

ArcGISTiledLayerUrl::ArcGISTiledLayerUrl(QWidget* parent) :
  QWidget(parent)
{
    // create the URL pointing to the tiled map service
    QUrl tiledLayerUrl("https://services.arcgisonline.com/arcgis/rest/services/NatGeo_World_Map/MapServer");

    // construct the ArcGISTiledLayer using the URL
    m_tiledLayer = new ArcGISTiledLayer(tiledLayerUrl, this);

    // create a Basemap and pass in the ArcGISTiledLayer
    m_basemap = new Basemap(m_tiledLayer, this);

    // create a Map by passing in the Basemap
    m_map = new Map(m_basemap, this);

    // add the Map to a MapGraphicsView
    m_mapView = new MapGraphicsView(m_map, this);

    // setup the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}

ArcGISTiledLayerUrl::~ArcGISTiledLayerUrl() = default;
