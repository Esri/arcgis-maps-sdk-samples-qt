// [WriteFile Name=RasterFunctionService, Category=Layers]
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "RasterFunctionService.h"

#include "Map.h"
#include "MapQuickView.h"
#include "RasterLayer.h"
#include "Basemap.h"
#include "ImageServiceRaster.h"
#include "RasterFunction.h"

#include <QDir>
#include <QtCore/qglobal.h>

#ifdef Q_OS_IOS
#include <QStandardPaths>
#endif // Q_OS_IOS

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
  QString defaultDataPath()
  {
    QString dataPath;

  #ifdef Q_OS_ANDROID
    dataPath = "/sdcard";
  #elif defined Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  #else
    dataPath = QDir::homePath();
  #endif

    return dataPath;
  }
} // namespace

RasterFunctionService::RasterFunctionService(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void RasterFunctionService::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<RasterFunctionService>("Esri.Samples", 1, 0, "RasterFunctionServiceSample");
}

void RasterFunctionService::componentComplete()
{
  QQuickItem::componentComplete();

  m_dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/raster";

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // create a new tiled layer to add a basemap
  m_map = new Map(Basemap::streets(this), this);
  m_mapView->setMap(m_map);

  // create an image service raster
  m_imageServiceRaster = new ImageServiceRaster(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/NLCDLandCover2001/ImageServer"), this);
  // zoom to the raster's extent once it's loaded
  connect(m_imageServiceRaster, &ImageServiceRaster::doneLoading, this, [this]()
  {
    constexpr double scale = 50000000.;
    Viewpoint vpCenter = Viewpoint(m_imageServiceRaster->serviceInfo().fullExtent().center(), scale);
    m_mapView->setViewpoint(vpCenter);
  });

  // create a raster layer using the image service raster
  m_rasterLayer = new RasterLayer(m_imageServiceRaster, this);
  // add the raster layer to the map's operational layers
  m_map->operationalLayers()->append(m_rasterLayer);
}

void RasterFunctionService::applyRasterFunction()
{
  if (!QFile::exists(m_dataPath + "/hillshade_simplified.json"))
  {
    qDebug() << "JSON file for the raster function does not exist.";
    return;
  }

  //! [ImageServiceRaster Apply a raster function]
  RasterFunction* rasterFunction = new RasterFunction(m_dataPath + "/hillshade_simplified.json", this);
  RasterFunctionArguments* arguments = rasterFunction->arguments();
  arguments->setRaster("raster", m_imageServiceRaster);

  // create a new raster with the raster function and add to the operational layer
  Raster* raster = new Raster(rasterFunction, this);
  RasterLayer* rasterLayer = new RasterLayer(raster, this);
  //! [ImageServiceRaster Apply a raster function]

  m_map->operationalLayers()->clear();
  //! [ImageServiceRaster cpp add raster operational]
  m_map->operationalLayers()->append(rasterLayer);
  //! [ImageServiceRaster cpp add raster operational]
}
