// [WriteFile Name=FeatureLayerRenderingModeScene, Category=Layers]
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

#include "FeatureLayerRenderingModeScene.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "LoadSettings.h"
#include "ServiceFeatureTable.h"
#include "FeatureLayer.h"
#include "Point.h"
#include "SpatialReference.h"
#include "Camera.h"

#include <QString>
#include <QStringList>
#include <QTimer>

using namespace Esri::ArcGISRuntime;

FeatureLayerRenderingModeScene::FeatureLayerRenderingModeScene(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void FeatureLayerRenderingModeScene::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<FeatureLayerRenderingModeScene>("Esri.Samples", 1, 0, "FeatureLayerRenderingModeSceneSample");
}

void FeatureLayerRenderingModeScene::componentComplete()
{
  QQuickItem::componentComplete();

  // Create a scene for static rendering
  m_topSceneView = findChild<SceneQuickView*>("topSceneView");
  Scene* topScene = new Scene(this);
  topScene->loadSettings()->setPreferredPointFeatureRenderingMode(FeatureRenderingMode::Static);
  topScene->loadSettings()->setPreferredPolygonFeatureRenderingMode(FeatureRenderingMode::Static);
  topScene->loadSettings()->setPreferredPolylineFeatureRenderingMode(FeatureRenderingMode::Static);
  addFeatureLayers(topScene);
  m_topSceneView->setArcGISScene(topScene);

  // Create a scene for dynamic rendering
  m_bottomSceneView = findChild<SceneQuickView*>("bottomSceneView");
  Scene* bottomScene = new Scene(this);
  bottomScene->loadSettings()->setPreferredPointFeatureRenderingMode(FeatureRenderingMode::Dynamic);
  bottomScene->loadSettings()->setPreferredPolygonFeatureRenderingMode(FeatureRenderingMode::Dynamic);
  bottomScene->loadSettings()->setPreferredPolylineFeatureRenderingMode(FeatureRenderingMode::Dynamic);
  addFeatureLayers(bottomScene);
  m_bottomSceneView->setArcGISScene(bottomScene);

  // Create Zoom Out Camera Viewpoint
  const Point outPoint(-118.37, 34.46, SpatialReference::wgs84());
  const double outDistance = 42000.0;
  const double outHeading = 0.0;
  const double outPitch = 0.0;
  const double outRoll = 0.0;
  m_zoomOutCamera = Camera(outPoint, outDistance, outHeading, outPitch, outRoll);

  // Create Zoom In Camera Viewpoint
  const Point inPoint(-118.45, 34.395, SpatialReference::wgs84());
  const double inDistance = 2500.0;
  const double inHeading = 90.0;
  const double inPitch = 75.0;
  const double inRoll = 0.0;
  m_zoomInCamera = Camera(inPoint, inDistance, inHeading, inPitch, inRoll);

  // Set initial viewpoint
  m_topSceneView->setViewpointCameraAndWait(m_zoomOutCamera);
  m_bottomSceneView->setViewpointCameraAndWait(m_zoomOutCamera);

  // Create Timer
  m_timer = new QTimer(this);
  m_timer->setInterval(7000);
  connect(m_timer, &QTimer::timeout, this, &FeatureLayerRenderingModeScene::animate);
}

void FeatureLayerRenderingModeScene::addFeatureLayers(Scene* scene)
{
  const QStringList layerIds = {"0", "8", "9"};
  for (const QString& layerId : layerIds)
  {
    QString featureServiceUrl = QString("%1/%2").arg(m_featureServiceUrl, layerId);
    ServiceFeatureTable* featureTable = new ServiceFeatureTable(featureServiceUrl, this);
    FeatureLayer* featureLayer = new FeatureLayer(featureTable, this);
    scene->operationalLayers()->append(featureLayer);
  }
}

// update the Camera when the timer triggers
void FeatureLayerRenderingModeScene::animate()
{
  Camera camera;
  if (m_isZoomedOut)
    camera = Camera(m_zoomInCamera);
  else
    camera = Camera(m_zoomOutCamera);

  m_bottomSceneView->setViewpointCamera(camera, 5.0f);
  m_topSceneView->setViewpointCamera(camera, 5.0f);
  m_isZoomedOut = !m_isZoomedOut;
}

void FeatureLayerRenderingModeScene::startAnimation()
{
  if (!m_timer)
    return;

  animate();
  m_timer->start(7000);
}

void FeatureLayerRenderingModeScene::stopAnimation()
{
  if (!m_timer)
    return;

  m_timer->stop();
}
