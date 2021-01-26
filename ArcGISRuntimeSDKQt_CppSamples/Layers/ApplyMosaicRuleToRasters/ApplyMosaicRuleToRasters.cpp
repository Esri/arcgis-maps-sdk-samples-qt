// [WriteFile Name=ApplyMosaicRuleToRasters, Category=Layers]
// [Legal]
// Copyright 2020 Esri.

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

#include "ApplyMosaicRuleToRasters.h"

#include "ImageServiceRaster.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MosaicRule.h"
#include "RasterLayer.h"

using namespace Esri::ArcGISRuntime;

ApplyMosaicRuleToRasters::ApplyMosaicRuleToRasters(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISTopographic, this)),
  m_mosaicRule(new MosaicRule(this))
{
}

ApplyMosaicRuleToRasters::~ApplyMosaicRuleToRasters() = default;

void ApplyMosaicRuleToRasters::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ApplyMosaicRuleToRasters>("Esri.Samples", 1, 0, "ApplyMosaicRuleToRastersSample");
}

MapQuickView* ApplyMosaicRuleToRasters::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ApplyMosaicRuleToRasters::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  // Create image service raster from image server
  m_imageServiceRaster = new ImageServiceRaster(QUrl("https://sampleserver7.arcgisonline.com/arcgis/rest/services/amberg_germany/ImageServer"), this);

  // Set Mosaic Rule if none exists on the image service raster
  if (!m_imageServiceRaster->mosaicRule())
    m_imageServiceRaster->setMosaicRule(m_mosaicRule);

  // Create a raster layer form the image service raster
  m_rasterLayer = new RasterLayer(m_imageServiceRaster, this);

  // Once loaded change the viewpoint the the center of the raster layers full extent
  connect(m_rasterLayer, &RasterLayer::doneLoading, this, [this](Error e)
  {
    if (e.isEmpty())
    {
      m_rasterLoaded = true;
      emit rasterLoadedChanged();
      m_mapView->setViewpointCenter(m_rasterLayer->fullExtent().center(), 25000.0);
    }
  });

  m_map->operationalLayers()->append(m_rasterLayer);
  emit mapViewChanged();
}

void ApplyMosaicRuleToRasters::applyRasterRule(const QString& ruleString)
{
  // Reset to clear previous mosaic rule parameters
  ApplyMosaicRuleToRasters::resetMosaicRule();

  if (ruleString == "None")
  {
    m_mosaicRule->setMosaicMethod(MosaicMethod::None);
  }
  else if (ruleString == "NorthWest")
  {
    m_mosaicRule->setMosaicMethod(MosaicMethod::Northwest);
    m_mosaicRule->setMosaicOperation(MosaicOperation::First);
  }
  else if (ruleString == "Center")
  {
    m_mosaicRule->setMosaicMethod(MosaicMethod::Center);
    m_mosaicRule->setMosaicOperation(MosaicOperation::Blend);
  }
  else if (ruleString == "ByAttribute")
  {
    m_mosaicRule->setMosaicMethod(MosaicMethod::Attribute);
    m_mosaicRule->setSortField("OBJECTID");

  }
  else if (ruleString  == "LockRaster")
  {
    m_mosaicRule->setMosaicMethod(MosaicMethod::LockRaster);
    m_mosaicRule->setLockRasterIds(QList<qint64>{1,7,12});
  }
  m_imageServiceRaster->setMosaicRule(m_mosaicRule);
}

// Helper function to reset the mosaic rule
void ApplyMosaicRuleToRasters::resetMosaicRule()
{
  delete m_mosaicRule;
  m_mosaicRule = new MosaicRule(this);
}
