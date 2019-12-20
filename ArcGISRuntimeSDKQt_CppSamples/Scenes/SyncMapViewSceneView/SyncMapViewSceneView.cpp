// [WriteFile Name=SyncMapViewSceneView, Category=Scenes]
// [Legal]
// Copyright 2018 Esri.

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

#include "SyncMapViewSceneView.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "Map.h"
#include "MapQuickView.h"

using namespace Esri::ArcGISRuntime;

SyncMapViewSceneView::SyncMapViewSceneView(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(Basemap::imagery(this), this)),
  m_map(new Map(Basemap::imagery(this), this))
{
}

SyncMapViewSceneView::~SyncMapViewSceneView() = default;

void SyncMapViewSceneView::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<SyncMapViewSceneView>("Esri.Samples", 1, 0, "SyncMapViewSceneViewSample");
}

SceneQuickView* SyncMapViewSceneView::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void SyncMapViewSceneView::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
  {
    return;
  }

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  connect(m_sceneView, &SceneQuickView::viewpointChanged, this,
          [this]
  {
    if (m_mapView && m_sceneView->isNavigating())
      m_mapView->setViewpoint(m_sceneView->currentViewpoint(ViewpointType::CenterAndScale), 0);

  },
  Qt::UniqueConnection);

  emit sceneViewChanged();
}

MapQuickView* SyncMapViewSceneView::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void SyncMapViewSceneView::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);
  m_mapView->setRotationByPinchingEnabled(true);

  connect(m_mapView, &MapQuickView::viewpointChanged, this,
          [this]
  {
    if (m_sceneView && m_mapView->isNavigating())
      m_sceneView->setViewpoint(m_mapView->currentViewpoint(ViewpointType::CenterAndScale), 0);

  },
  Qt::UniqueConnection);

  emit mapViewChanged();
}
