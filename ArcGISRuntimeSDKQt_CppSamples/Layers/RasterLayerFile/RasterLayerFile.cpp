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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "RasterLayerFile.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Raster.h"
#include "RasterLayer.h"
#include "Basemap.h"

#include <QUrl>
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

RasterLayerFile::RasterLayerFile(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

RasterLayerFile::~RasterLayerFile() = default;

void RasterLayerFile::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<RasterLayerFile>("Esri.Samples", 1, 0, "RasterLayerFileSample");
}

void RasterLayerFile::componentComplete()
{
  QQuickItem::componentComplete();

  const QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/raster/";

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  Basemap* basemap = Basemap::imagery(this);
  m_map = new Map(basemap, this);
  m_mapView->setMap(m_map);

  // Create a map using a raster layer
  createAndAddRasterLayer(dataPath + "Shasta.tif");
}

void RasterLayerFile::createAndAddRasterLayer(QString dataPath)
{
  // correct data path if contains file:/ prefix
  if (dataPath.contains("file:/"))
    dataPath = QString(QUrl(dataPath).toLocalFile());

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
