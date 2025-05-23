// [WriteFile Name=RasterRgbRenderer, Category=Layers]
// [Legal]
// Copyright 2017 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "RasterRgbRenderer.h"

// ArcGIS Maps SDK headers
#include "Basemap.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapViewTypes.h"
#include "MinMaxStretchParameters.h"
#include "PercentClipStretchParameters.h"
#include "RGBRenderer.h"
#include "Raster.h"
#include "RasterLayer.h"
#include "StandardDeviationStretchParameters.h"

// Qt headers
#include <QStandardPaths>
#include <QtCore/qglobal.h>

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
  QString defaultDataPath()
  {
    QString dataPath;

  #ifdef Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  #else
    dataPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
  #endif

    return dataPath;
  }
} // namespace

RasterRgbRenderer::RasterRgbRenderer(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_dataPath(defaultDataPath() + "/ArcGIS/Runtime/Data/raster")
{
}

RasterRgbRenderer::~RasterRgbRenderer() = default;

void RasterRgbRenderer::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<RasterRgbRenderer>("Esri.Samples", 1, 0, "RasterRgbRendererSample");
}

void RasterRgbRenderer::componentComplete()
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

void RasterRgbRenderer::applyMinMax(double min0, double min1, double min2, double max0, double max1, double max2)
{
  MinMaxStretchParameters stretchParams(QList<double>{min0, min1, min2}, QList<double>{max0, max1, max2});
  RGBRenderer* renderer = new RGBRenderer(stretchParams, this);
  m_rasterLayer->setRenderer(renderer);
}

void RasterRgbRenderer::applyPercentClip(double min, double max)
{
  PercentClipStretchParameters stretchParams(min, max);
  RGBRenderer* renderer = new RGBRenderer(stretchParams, this);
  m_rasterLayer->setRenderer(renderer);
}

void RasterRgbRenderer::applyStandardDeviation(double factor)
{
  StandardDeviationStretchParameters stretchParams(factor);
  RGBRenderer* renderer = new RGBRenderer(stretchParams, this);
  m_rasterLayer->setRenderer(renderer);
}
