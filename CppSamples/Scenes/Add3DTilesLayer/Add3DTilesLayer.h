// [WriteFile Name=Add3DTilesLayer, Category=Scenes]
// [Legal]
// Copyright 2023 Esri.
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

#ifndef ADD3DTILESLAYER_H
#define ADD3DTILESLAYER_H

// Qt headers
#include <QObject>

namespace Esri::ArcGISRuntime
{
class Scene;
class SceneQuickView;
class Ogc3dTilesLayer;
}

Q_MOC_INCLUDE("SceneQuickView.h");

class Add3DTilesLayer : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)

public:
  explicit Add3DTilesLayer(QObject* parent = nullptr);
  ~Add3DTilesLayer() override;
  static void init();

signals:
  void sceneViewChanged();

private:
  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;

  // add 3D tiles layer
  Esri::ArcGISRuntime::Ogc3dTilesLayer* m_ogc3dTilesLayer = nullptr;
  void add3DTilesLayer();

  void setInitialViewpoint();
};

#endif // ADD3DTILESLAYER_H
