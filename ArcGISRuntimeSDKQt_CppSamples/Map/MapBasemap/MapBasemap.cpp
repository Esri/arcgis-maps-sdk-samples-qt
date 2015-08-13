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

MapBasemap::MapBasemap() :
  QWidget(nullptr),
  m_map(nullptr),
  m_mapView(nullptr),
  m_basemap(nullptr)
{
    Esri::ArcGISRuntime::ArcGISTiledLayer* layer = new Esri::ArcGISRuntime::ArcGISTiledLayer(QUrl("http://services.arcgisonline.com/arcgis/rest/services/World_Imagery/MapServer"), this);

    m_basemap = new Esri::ArcGISRuntime::Basemap(layer, this);

    m_map = new Esri::ArcGISRuntime::Map(m_basemap, this);
    m_mapView = new Esri::ArcGISRuntime::MapView(m_map, this);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}

MapBasemap::~MapBasemap()
{
}
