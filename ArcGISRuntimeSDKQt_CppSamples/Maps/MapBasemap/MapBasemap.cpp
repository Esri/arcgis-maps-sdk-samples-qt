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

#include "MapBasemap.h"
#include "Map.h"
#include "MapView.h"
#include "Basemap.h"
#include "ArcGISTiledLayer.h"
#include <QDir>
#include <QUrl>
#include <QVBoxLayout>

using namespace Esri::ArcGISRuntime;

MapBasemap::MapBasemap(QWidget* parent) :
  QWidget(parent),
  m_map(nullptr),
  m_mapView(nullptr),
  m_basemap(nullptr)
{
    ArcGISTiledLayer* layer = new ArcGISTiledLayer(QUrl("http://services.arcgisonline.com/arcgis/rest/services/World_Imagery/MapServer"), this);

    m_basemap = new Basemap(layer, this);

    m_map = new Map(m_basemap, this);
    m_mapView = new MapView(m_map, this);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}

MapBasemap::~MapBasemap()
{
}
