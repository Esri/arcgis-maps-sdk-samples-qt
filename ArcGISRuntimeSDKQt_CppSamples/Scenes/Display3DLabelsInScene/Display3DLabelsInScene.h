// [WriteFile Name=Display3DLabelsInScene, Category=Scenes]
// [Legal]
// Copyright 2021 Esri.

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

#ifndef DISPLAY3DLABELSINSCENE_H
#define DISPLAY3DLABELSINSCENE_H

namespace Esri::ArcGISRuntime
{
class Scene;
class SceneQuickView;
class FeatureLayer;
}

#include <QObject>

Q_MOC_INCLUDE("SceneQuickView.h")

class Display3DLabelsInScene : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)

public:
  explicit Display3DLabelsInScene(QObject* parent = nullptr);
  ~Display3DLabelsInScene();

  static void init();

signals:
  void sceneViewChanged();

private:
  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);
  void display3DLabelsOnFeatureLayer(Esri::ArcGISRuntime::FeatureLayer* featureLayer);

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
};

#endif // DISPLAY3DLABELSINSCENE_H
