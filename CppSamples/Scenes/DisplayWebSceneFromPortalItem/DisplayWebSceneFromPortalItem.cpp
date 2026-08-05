// [WriteFile Name=DisplayWebSceneFromPortalItem, Category=Scenes]
// [Legal]
// Copyright 2018 Esri.
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
#include "DisplayWebSceneFromPortalItem.h"

// ArcGIS Maps SDK headers
#include "PortalItem.h"
#include "Scene.h"
#include "SceneQuickView.h"

using namespace Esri::ArcGISRuntime;

DisplayWebSceneFromPortalItem::DisplayWebSceneFromPortalItem(QQuickItem* parent /* = nullptr */) :
  QQuickItem(parent)
{
}

void DisplayWebSceneFromPortalItem::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<DisplayWebSceneFromPortalItem>("Esri.Samples", 1, 0, "DisplayWebSceneFromPortalItemSample");
}

void DisplayWebSceneFromPortalItem::componentComplete()
{
  QQuickItem::componentComplete();

  m_sceneView = findChild<SceneQuickView*>("sceneView");

  // Create a PortalItem with an Item ID
  PortalItem* item = new PortalItem(QStringLiteral("c6f90b19164c4283884361005faea852"), this);

  // Create a scene with the Item
  Scene* scene = new Scene(item, this);

  // Set the Scene on the SceneView
  m_sceneView->setArcGISScene(scene);
}
