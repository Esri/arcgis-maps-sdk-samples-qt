// [WriteFile Name=Hillshade_Renderer, Category=Layers]
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

#include "Hillshade_Renderer.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Raster.h"
#include "RasterLayer.h"
#include "Basemap.h"
#include "HillshadeRenderer.h"

#include <QQmlProperty>
#include <QUrl>

using namespace Esri::ArcGISRuntime;

Hillshade_Renderer::Hillshade_Renderer(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_mapView(nullptr)
{
}

Hillshade_Renderer::~Hillshade_Renderer()
{
}

void Hillshade_Renderer::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create the raster and raster layer
  QString dataPath = QUrl(QQmlProperty::read(this, "dataPath").toString()).toLocalFile();
  Raster* raster = new Raster(dataPath + "/srtm.tiff", this);
  RasterLayer* rasterLayer = new RasterLayer(raster, this);

  // Add the raster to the map
  Basemap* basemap = new Basemap(rasterLayer, this);
  Map* map = new Map(basemap, this);

  // zoom to the new layer once loaded
  connect(map, &Map::loadStatusChanged, this, [this](LoadStatus loadStatus)
  {
    if (loadStatus == LoadStatus::Loaded)
    {
      m_mapView->setViewpointScale(754479);
    }
  });
  m_mapView->setMap(map);

  //! [HillshadeRenderer apply to layer snippet]
  // Apply the hillshade renderer to the raster layer
  HillshadeRenderer* hillshadeRenderer = new HillshadeRenderer(45.0, 315.0, 0.000016, SlopeType::None, 1.0, 1.0, 8, this);
  rasterLayer->setRenderer(hillshadeRenderer);
  //! [HillshadeRenderer apply to layer snippet]
}
