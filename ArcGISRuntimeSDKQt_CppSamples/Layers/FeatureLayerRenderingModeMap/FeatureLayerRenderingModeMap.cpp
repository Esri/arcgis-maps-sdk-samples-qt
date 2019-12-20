// [WriteFile Name=FeatureLayerRenderingModeMap, Category=Layers]
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

#include "FeatureLayerRenderingModeMap.h"

#include "ArcGISTiledElevationSource.h"
#include "Map.h"
#include "MapQuickView.h"
#include "LoadSettings.h"
#include "ServiceFeatureTable.h"
#include "FeatureLayer.h"
#include "Point.h"
#include "SpatialReference.h"

#include <QString>
#include <QStringList>
#include <QTimer>

using namespace Esri::ArcGISRuntime;

FeatureLayerRenderingModeMap::FeatureLayerRenderingModeMap(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void FeatureLayerRenderingModeMap::init()
{
  // Register classes for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<FeatureLayerRenderingModeMap>("Esri.Samples", 1, 0, "FeatureLayerRenderingModeMapSample");
}

void FeatureLayerRenderingModeMap::componentComplete()
{
  QQuickItem::componentComplete();

  // Create a map for static rendering
  m_topMapView = findChild<MapQuickView*>("topMapView");
  Map* topMap = new Map(this);
  topMap->loadSettings()->setPreferredPointFeatureRenderingMode(FeatureRenderingMode::Static);
  topMap->loadSettings()->setPreferredPolygonFeatureRenderingMode(FeatureRenderingMode::Static);
  topMap->loadSettings()->setPreferredPolylineFeatureRenderingMode(FeatureRenderingMode::Static);
  addFeatureLayers(topMap);
  m_topMapView->setMap(topMap);

  // Create a map for dynamic rendering
  m_bottomMapView = findChild<MapQuickView*>("bottomMapView");
  Map* bottomMap = new Map(this);
  bottomMap->loadSettings()->setPreferredPointFeatureRenderingMode(FeatureRenderingMode::Dynamic);
  bottomMap->loadSettings()->setPreferredPolygonFeatureRenderingMode(FeatureRenderingMode::Dynamic);
  bottomMap->loadSettings()->setPreferredPolylineFeatureRenderingMode(FeatureRenderingMode::Dynamic);
  addFeatureLayers(bottomMap);
  m_bottomMapView->setMap(bottomMap);

  // Create Zoom Out Camera Viewpoint
  const Point centerPt(-118.37, 34.46, SpatialReference::wgs84());
  const double outScale = 650000;
  const double outRotation = 0;
  m_zoomOutViewpoint = Viewpoint(centerPt, outScale, outRotation);

  // Create Zoom In Camera Viewpoint
  const double inScale = 50000;
  const double inRotation = 90;
  m_zoomInViewpoint = Viewpoint(centerPt, inScale, inRotation);

  // Set initial viewpoint
  m_topMapView->setViewpointAndWait(m_zoomOutViewpoint);
  m_bottomMapView->setViewpointAndWait(m_zoomOutViewpoint);

  // Create Timer
  m_timer = new QTimer(this);
  m_timer->setInterval(7000);
  connect(m_timer, &QTimer::timeout, this, &FeatureLayerRenderingModeMap::animate);
}

void FeatureLayerRenderingModeMap::addFeatureLayers(Map* map)
{
  const QStringList layerIds = {"0", "8", "9"};
  for (const QString& layerId : layerIds)
  {
    QString featureServiceUrl = QString("%1/%2").arg(m_featureServiceUrl, layerId);
    ServiceFeatureTable* featureTable = new ServiceFeatureTable(featureServiceUrl, this);
    FeatureLayer* featureLayer = new FeatureLayer(featureTable, this);
    map->operationalLayers()->append(featureLayer);
  }
}

// update the Camera when the timer triggers
void FeatureLayerRenderingModeMap::animate()
{
  Viewpoint viewpoint;
  if (m_isZoomedOut)
    viewpoint = Viewpoint(m_zoomInViewpoint);
  else
    viewpoint = Viewpoint(m_zoomOutViewpoint);

  m_bottomMapView->setViewpoint(viewpoint, 5.0f);
  m_topMapView->setViewpoint(viewpoint, 5.0f);
  m_isZoomedOut = !m_isZoomedOut;
}

void FeatureLayerRenderingModeMap::startAnimation()
{
  if (!m_timer)
    return;

  animate();
  m_timer->start(7000);
}

void FeatureLayerRenderingModeMap::stopAnimation()
{
  if (!m_timer)
    return;

  m_timer->stop();
}
