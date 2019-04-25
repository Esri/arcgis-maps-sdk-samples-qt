// [WriteFile Name=ViewContentBeneathTerrainSurface, Category=Scenes]
// [Legal]
// Copyright 2019 Esri.

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

#include "ViewContentBeneathTerrainSurface.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "Portal.h"
#include "PortalItem.h"

using namespace Esri::ArcGISRuntime;

ViewContentBeneathTerrainSurface::ViewContentBeneathTerrainSurface(QObject* parent /* = nullptr */):
  QObject(parent),
  m_portal(new Portal(this)),
  m_portalItem(new PortalItem(m_portal, "91a4fafd747a47c7bab7797066cb9272", this))
{
  m_scene = new Scene(m_portalItem, this);

  connect(m_scene, &Scene::doneLoading, this, [this](Error loadError)
  {
    if (!loadError.isEmpty())
      return;

    m_scene->baseSurface()->setOpacity(0.4f);
    m_scene->baseSurface()->setNavigationConstraint(NavigationConstraint::None);
  });
}

ViewContentBeneathTerrainSurface::~ViewContentBeneathTerrainSurface() = default;

void ViewContentBeneathTerrainSurface::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<ViewContentBeneathTerrainSurface>("Esri.Samples", 1, 0, "ViewContentBeneathTerrainSurfaceSample");
}

SceneQuickView* ViewContentBeneathTerrainSurface::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void ViewContentBeneathTerrainSurface::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
    return;

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  emit sceneViewChanged();
}
