// [WriteFile Name=ChangeBasemap, Category=Maps]
// [Legal]
// Copyright 2021 Esri.
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
#include "ChangeBasemap.h"

// ArcGIS Maps SDK headers
#include "Scene.h"
#include "LocalSceneQuickView.h"
#include "MapTypes.h"
#include "SceneViewTypes.h"

using namespace Esri::ArcGISRuntime;

ChangeBasemap::ChangeBasemap(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(SceneViewingMode::Local, BasemapStyle::ArcGISTopographic, this))
{
}

ChangeBasemap::~ChangeBasemap() = default;

void ChangeBasemap::init()
{
  // Register the map view for QML
  qmlRegisterType<LocalSceneQuickView>("Esri.Samples", 1, 0, "LocalSceneView");
  qmlRegisterType<ChangeBasemap>("Esri.Samples", 1, 0, "ChangeBasemapSample");
}

LocalSceneQuickView* ChangeBasemap::localSceneView() const
{
  return m_localSceneView;
}

// Set the view (created in QML)
void ChangeBasemap::setLocalSceneView(LocalSceneQuickView* localSceneView)
{
  if (!localSceneView || localSceneView == m_localSceneView)
    return;

  m_localSceneView = localSceneView;
  m_localSceneView->setArcGISScene(m_scene);

  emit mapViewChanged();
}
