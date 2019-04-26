// [WriteFile Name=ChooseCameraController, Category=Scenes]
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

#ifndef CHOOSECAMERACONTROLLER_H
#define CHOOSECAMERACONTROLLER_H

// Qt headers
#include <QObject>

namespace Esri
{
namespace ArcGISRuntime
{
class GlobeCameraController;
class GraphicsOverlay;
class OrbitGeoElementCameraController;
class OrbitLocationCameraController;
class Scene;
class SceneQuickView;
}
}

class ChooseCameraController : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)

public:
  explicit ChooseCameraController(QObject* parent = nullptr);
  ~ChooseCameraController();

  Q_INVOKABLE void setGlobeController();
  Q_INVOKABLE void setOrbitLocationController();
  Q_INVOKABLE void setOrbitGeoElementController();

  static void init();

signals:
  void sceneViewChanged();

private:
  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);

  Esri::ArcGISRuntime::GlobeCameraController* m_orbitGlobe = nullptr;
  Esri::ArcGISRuntime::OrbitGeoElementCameraController* m_orbitGeoElement = nullptr;
  Esri::ArcGISRuntime::OrbitLocationCameraController* m_orbitLocation = nullptr;

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;

  Esri::ArcGISRuntime::GraphicsOverlay* m_overlay;
};

#endif // CHOOSECAMERACONTROLLER_H
