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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "Hillshade_Renderer.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Raster.h"
#include "RasterLayer.h"
#include "Basemap.h"
#include "HillshadeRenderer.h"

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

Hillshade_Renderer::Hillshade_Renderer(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

Hillshade_Renderer::~Hillshade_Renderer() = default;

void Hillshade_Renderer::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<Hillshade_Renderer>("Esri.Samples", 1, 0, "Hillshade_RendererSample");
}

void Hillshade_Renderer::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create the raster and raster layer
  const QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/raster";
  Raster* raster = new Raster(dataPath + "/srtm.tiff", this);
  m_rasterLayer = new RasterLayer(raster, this);

  // Add the raster to the map
  Basemap* basemap = new Basemap(m_rasterLayer, this);
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
  constexpr double altitude = 45.0;
  constexpr double azimuth = 315.0;
  constexpr double zFactor = 0.000016;
  constexpr SlopeType slopeType = SlopeType::None;
  constexpr double pixelSizeFactor = 1.0;
  constexpr double pixelSizePower = 1.0;
  constexpr int outputBitDepth = 8;
  HillshadeRenderer* hillshadeRenderer = new HillshadeRenderer(altitude, azimuth, zFactor, slopeType, pixelSizeFactor, pixelSizePower, outputBitDepth, this);
  m_rasterLayer->setRenderer(hillshadeRenderer);
  //! [HillshadeRenderer apply to layer snippet]
}

void Hillshade_Renderer::applyHillshadeRenderer(double altitude, double azimuth, int slope)
{
  // create the new renderer
  SlopeType slopeType = static_cast<SlopeType>(slope);
  HillshadeRenderer* hillshadeRenderer = new HillshadeRenderer(altitude, azimuth, 0.000016, slopeType, 1.0, 1.0, 8, this);

  // set the renderer on the layer
  m_rasterLayer->setRenderer(hillshadeRenderer);
}
