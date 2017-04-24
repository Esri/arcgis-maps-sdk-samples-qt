// [WriteFile Name=RasterLayerService, Category=Layers]
// [Legal]
// Copyright 2017 Esri.

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

#include "RasterLayerService.h"

#include "Map.h"
#include "MapQuickView.h"
#include "RasterLayer.h"
#include "ImageServiceRaster.h"

#include <QQmlProperty>

using namespace Esri::ArcGISRuntime;

RasterLayerService::RasterLayerService(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

RasterLayerService::~RasterLayerService()
{
}

void RasterLayerService::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<RasterLayerService>("Esri.Samples", 1, 0, "RasterLayerServiceSample");
}

void RasterLayerService::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // create an image service raster
  ImageServiceRaster* imageServiceRaster = new ImageServiceRaster(QUrl("http://sampleserver6.arcgisonline.com/arcgis/rest/services/NLCDLandCover2001/ImageServer"), this);
  // create a raster layer using the image service raster
  m_rasterLayer = new RasterLayer(imageServiceRaster, this);
  // add the raster layer to the map's operational layers
  m_map = new Map(new Basemap(m_rasterLayer, this), this);
  m_mapView->setMap(m_map);
}
