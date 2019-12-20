// [WriteFile Name=FeatureLayerChangeRenderer, Category=Features]
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

#include "FeatureLayerChangeRenderer.h"

#include "Map.h"
#include "MapQuickView.h"
#include "FeatureLayer.h"
#include "Basemap.h"
#include "SpatialReference.h"
#include "ServiceFeatureTable.h"
#include "Viewpoint.h"
#include "Envelope.h"
#include "SimpleLineSymbol.h"
#include "SimpleRenderer.h"
#include <QColor>
#include <QUrl>

using namespace Esri::ArcGISRuntime;

FeatureLayerChangeRenderer::FeatureLayerChangeRenderer(QQuickItem* parent) :
  QQuickItem(parent)
{
}

FeatureLayerChangeRenderer::~FeatureLayerChangeRenderer() = default;

void FeatureLayerChangeRenderer::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<FeatureLayerChangeRenderer>("Esri.Samples", 1, 0, "FeatureLayerChangeRendererSample");
}

void FeatureLayerChangeRenderer::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create a map using the topographic basemap
  m_map = new Map(Basemap::topographic(this), this);
  m_map->setInitialViewpoint(Viewpoint(Envelope(-13075816.4047166, 4014771.46954516, -13073005.6797177, 4016869.78617381, SpatialReference(102100))));

  // Set map to map view
  m_mapView->setMap(m_map);

  // create the feature table
  ServiceFeatureTable* featureTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/PoolPermits/FeatureServer/0"), this);
  // create the feature layer using the feature table
  m_featureLayer = new FeatureLayer(featureTable, this);

  connect(m_featureLayer, &FeatureLayer::loadStatusChanged, this, [this](LoadStatus loadStatus)
  {
    loadStatus == LoadStatus::Loaded ? m_initialized = true : m_initialized = false;
    emit layerInitializedChanged();
  });

  // add the feature layer to the map
  m_map->operationalLayers()->append(m_featureLayer);
}

bool FeatureLayerChangeRenderer::layerInitialized() const
{
  return m_initialized;
}

void FeatureLayerChangeRenderer::changeRenderer()
{
  // create a line symbol
  SimpleLineSymbol* sls = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(0, 0, 255, 204), 2.0f, this);
  // create a renderer with the symbol above
  SimpleRenderer* simpleRenderer = new SimpleRenderer(sls, this);
  // change the renderer with the renderer created above
  m_featureLayer->setRenderer(simpleRenderer);
}
