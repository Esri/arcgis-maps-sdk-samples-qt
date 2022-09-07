// [WriteFile Name=EditKmlGroundOverlay, Category=EditData]
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

#ifndef EDITKMLGROUNDOVERLAY_H
#define EDITKMLGROUNDOVERLAY_H

namespace Esri
{
namespace ArcGISRuntime
{
class Scene;
class SceneQuickView;
class KmlGroundOverlay;
}
}

#include <QObject>

Q_MOC_INCLUDE("SceneQuickView.h")

class EditKmlGroundOverlay : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)

public:
  explicit EditKmlGroundOverlay(QObject* parent = nullptr);
  ~EditKmlGroundOverlay();

  static void init();
  Q_INVOKABLE void setOpacity(int opacity);

signals:
  void sceneViewChanged();

private:
  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::KmlGroundOverlay* m_groundOverlay = nullptr;
};

#endif // EDITKMLGROUNDOVERLAY_H
