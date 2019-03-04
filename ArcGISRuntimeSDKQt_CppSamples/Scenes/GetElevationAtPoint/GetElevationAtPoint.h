// [WriteFile Name=GetElevationAtPoint, Category=Scenes]
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

#ifndef GETELEVATIONATPOINT_H
#define GETELEVATIONATPOINT_H

#include <QObject>

#include "TaskWatcher.h"

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Graphic;
    class GraphicsOverlay;
    class Scene;
    class SceneQuickView;
  }
}

class QMouseEvent;

class GetElevationAtPoint : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)
  Q_PROPERTY(double elevation READ elevation NOTIFY elevationChanged)
  Q_PROPERTY(bool elevationQueryRunning READ elevationQueryRunning NOTIFY elevationQueryRunningChanged)

public:
  explicit GetElevationAtPoint(QObject* parent = nullptr);
  ~GetElevationAtPoint();

  static void init();

private slots:
  void displayElevationOnClick(QMouseEvent& mouseEvent);

signals:
  void sceneViewChanged();
  void elevationChanged(double newElevation);
  void elevationQueryRunningChanged();

private:
  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;

  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::Graphic* m_elevationMarker = nullptr;

  //Property exposing elevation to the QML UI.
  double elevation() const;
  //Property exposing whether the elevation query is running to the QML UI, so the busy indicator can be displayed
  bool elevationQueryRunning() const;

  double m_elevation = 0.0;
  Esri::ArcGISRuntime::TaskWatcher m_elevationQueryTaskWatcher;
};

#endif // GETELEVATIONATPOINT_H
