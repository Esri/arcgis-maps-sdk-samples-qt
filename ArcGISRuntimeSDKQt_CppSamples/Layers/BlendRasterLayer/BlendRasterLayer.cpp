// [WriteFile Name=BlendRasterLayer, Category=Layers]
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

#include "BlendRasterLayer.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Raster.h"
#include "RasterLayer.h"
#include "BlendRenderer.h"

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

BlendRasterLayer::BlendRasterLayer(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_dataPath(defaultDataPath() + "/ArcGIS/Runtime/Data/raster")
{
}

BlendRasterLayer::~BlendRasterLayer() = default;

void BlendRasterLayer::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<BlendRasterLayer>("Esri.Samples", 1, 0, "BlendRasterLayerSample");
}

void BlendRasterLayer::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create the raster and raster layer
  Raster* raster = new Raster(m_dataPath + "/Shasta.tif", this);
  m_rasterLayer = new RasterLayer(raster, this);

  m_elevationRaster = new Raster(m_dataPath + "/Shasta_Elevation.tif", this);
  m_rasterLayerColorRamp = new RasterLayer(m_elevationRaster, this);

  // Add the raster to the map
  m_basemap = new Basemap(m_rasterLayer, this);
  m_basemapColorRamp = new Basemap(m_rasterLayerColorRamp, this);
  m_map = new Map(m_basemap, this);
  m_mapView->setMap(m_map);
}

void BlendRasterLayer::applyRenderSettings(double altitude, double azimuth, int slopeTypeVal, int colorRampTypeVal)
{
  PresetColorRampType colorRampType = static_cast<PresetColorRampType>(colorRampTypeVal);
  RasterLayer* rasterLyr = rasterLayer(colorRampType != PresetColorRampType::None);
  if (!rasterLyr)
    return;

  QList<double> outputMinValues{9};
  QList<double> outputMaxValues{255};
  QList<double> sourceMinValues;
  QList<double> sourceMaxValues;
  QList<double> noDataValues;
  QList<double> gammas;
  double zFactor = 1.;
  double pixelSizeFactor = 1.;
  double pixelSizePower = 1.;
  int outputBitDepth = 8;
  ColorRamp* colorRamp = ColorRamp::create(colorRampType, 800, this);
  SlopeType slopeType = static_cast<SlopeType>(slopeTypeVal);

  BlendRenderer* renderer = new BlendRenderer(m_elevationRaster,
                                              outputMinValues,
                                              outputMaxValues,
                                              sourceMinValues,
                                              sourceMaxValues,
                                              noDataValues,
                                              gammas,
                                              colorRamp,
                                              altitude,
                                              azimuth,
                                              zFactor,
                                              slopeType,
                                              pixelSizeFactor,
                                              pixelSizePower,
                                              outputBitDepth,
                                              this);

  rasterLyr->setRenderer(renderer);
}

RasterLayer* BlendRasterLayer::rasterLayer(bool useColorRamp)
{
  m_map->setBasemap(useColorRamp ? m_basemapColorRamp : m_basemap);
  return useColorRamp ? m_rasterLayerColorRamp : m_rasterLayer;
}
