// [WriteFile Name=RasterStretchRenderer, Category=Layers]
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

#include "RasterStretchRenderer.h"

#include "Map.h"
#include "MapQuickView.h"
#include "MinMaxStretchParameters.h"
#include "PercentClipStretchParameters.h"
#include "Raster.h"
#include "RasterLayer.h"
#include "StretchRenderer.h"
#include "StandardDeviationStretchParameters.h"

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

RasterStretchRenderer::RasterStretchRenderer(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_dataPath(defaultDataPath() + "/ArcGIS/Runtime/Data/raster")
{
}

RasterStretchRenderer::~RasterStretchRenderer() = default;

void RasterStretchRenderer::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<RasterStretchRenderer>("Esri.Samples", 1, 0, "RasterStretchRendererSample");
}

void RasterStretchRenderer::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create the raster and raster layer
  Raster* raster = new Raster(m_dataPath + "/Shasta.tif", this);
  m_rasterLayer = new RasterLayer(raster, this);

  // Add the raster to the map
  Basemap* basemap = new Basemap(m_rasterLayer, this);
  Map* map = new Map(basemap, this);
  m_mapView->setMap(map);
}

//! [RasterStretchRenderer cpp set renderers]
void RasterStretchRenderer::applyMinMax(double min, double max)
{
  MinMaxStretchParameters stretchParams(QList<double>{min}, QList<double>{max});
  QList<double> gammas;
  bool estimateStats = true;
  StretchRenderer* renderer = new StretchRenderer(stretchParams, gammas, estimateStats, PresetColorRampType::None, this);

  m_rasterLayer->setRenderer(renderer);
}

void RasterStretchRenderer::applyPercentClip(double min, double max)
{
  PercentClipStretchParameters stretchParams(min, max);
  QList<double> gammas;
  bool estimateStats = true;
  StretchRenderer* renderer = new StretchRenderer(stretchParams, gammas, estimateStats, PresetColorRampType::None, this);

  m_rasterLayer->setRenderer(renderer);
}

void RasterStretchRenderer::applyStandardDeviation(double factor)
{
  StandardDeviationStretchParameters stretchParams(factor);
  QList<double> gammas;
  bool estimateStats = true;
  StretchRenderer* renderer = new StretchRenderer(stretchParams, gammas, estimateStats, PresetColorRampType::None, this);

  m_rasterLayer->setRenderer(renderer);
}
//! [RasterStretchRenderer cpp set renderers]
