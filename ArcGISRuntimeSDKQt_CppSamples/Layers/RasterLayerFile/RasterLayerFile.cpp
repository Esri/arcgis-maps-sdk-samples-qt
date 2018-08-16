// [WriteFile Name=RasterLayerFile, Category=Analysis]
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

#include "RasterLayerFile.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Raster.h"
#include "RasterLayer.h"
#include "Basemap.h"

#include <QQmlProperty>
#include <QUrl>

using namespace Esri::ArcGISRuntime;

RasterLayerFile::RasterLayerFile(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

RasterLayerFile::~RasterLayerFile()
{
}

void RasterLayerFile::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<RasterLayerFile>("Esri.Samples", 1, 0, "RasterLayerFileSample");
}

void RasterLayerFile::componentComplete()
{
  QQuickItem::componentComplete();

  QString dataPath = QQmlProperty::read(this, "dataPath").toString();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  Basemap* basemap = Basemap::imagery(this);
  m_map = new Map(basemap, this);
  m_mapView->setMap(m_map);

  // Create a map using a raster layer
  createAndAddRasterLayer(dataPath + "Shasta.tif");
}

void RasterLayerFile::createAndAddRasterLayer(QUrl rasterUrl)
{
  QString dataPath = rasterUrl.toLocalFile();
  //! [RasterLayerFile cpp new raster layer]
  Raster* raster = new Raster(dataPath, this);
  RasterLayer* rasterLayer = new RasterLayer(raster, this);
  //! [RasterLayerFile cpp new raster layer]

  connect(rasterLayer, &RasterLayer::doneLoading, this, [this, rasterLayer](Error loadError)
  {
    if (!loadError.isEmpty())
      return;

    m_mapView->setViewpointCenter(rasterLayer->fullExtent().center(), 80000);
  });

  m_map->operationalLayers()->clear();
  m_map->operationalLayers()->append(rasterLayer);
}
