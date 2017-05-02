// [WriteFile Name=RasterRenderingRule, Category=Layers]
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

#include "RasterRenderingRule.h"

#include "Map.h"
#include "MapQuickView.h"
#include "RasterLayer.h"
#include "Basemap.h"
#include "ArcGISTiledLayer.h"
#include "ImageServiceRaster.h"
#include "RenderingRule.h"
#include "RenderingRuleInfo.h"

#include <QQmlProperty>
#include <QJsonObject>
#include <QJsonDocument>

using namespace Esri::ArcGISRuntime;

RasterRenderingRule::RasterRenderingRule(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

RasterRenderingRule::~RasterRenderingRule()
{
}

void RasterRenderingRule::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<RasterRenderingRule>("Esri.Samples", 1, 0, "RasterRenderingRuleSample");
}

void RasterRenderingRule::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  m_map = new Map(Basemap::streets(this), this);
  m_mapView->setMap(m_map);

  // create an image service raster
  m_imageServiceRaster = new ImageServiceRaster(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/CharlotteLAS/ImageServer"), this);

  // zoom to the raster's extent once it's loaded
  connect(m_imageServiceRaster, &ImageServiceRaster::doneLoading, this, [this]()
  {
      m_mapView->setViewpoint(Viewpoint(m_imageServiceRaster->serviceInfo().fullExtent()));

      QList<RenderingRuleInfo> renderingRuleInfos = m_imageServiceRaster->serviceInfo().renderingRuleInfos();
      if (renderingRuleInfos.length() > 0)
      {
        for (const RenderingRuleInfo& renderingRuleInfo : renderingRuleInfos)
        {
          m_renderingRuleNames << renderingRuleInfo.name();
        }
        emit renderingRuleNamesChanged();
      }
  });

  // create a raster layer using the image service raster
  m_rasterLayer = new RasterLayer(m_imageServiceRaster, this);
  // add the raster layer to the map's operational layers
  m_map->operationalLayers()->append(m_rasterLayer);
}

void RasterRenderingRule::applyRenderingRule(int index)
{
  RenderingRuleInfo renderingRuleInfo = m_imageServiceRaster->serviceInfo().renderingRuleInfos().at(index);
  RenderingRule* renderingRule = new RenderingRule(renderingRuleInfo, this);
  ImageServiceRaster* isr = new ImageServiceRaster(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/CharlotteLAS/ImageServer"), this);
  isr->setRenderingRule(renderingRule);
  RasterLayer* rasterLayer = new RasterLayer(isr, this);
  m_map->operationalLayers()->clear();
  m_map->operationalLayers()->append(rasterLayer);
}
