// [WriteFile Name=ScenePropertiesExpressions, Category=Scenes]
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

#ifndef SCENEPROPERTIESEXPRESSIONS_H
#define SCENEPROPERTIESEXPRESSIONS_H

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

#include <QObject>

class ScenePropertiesExpressions : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)
  Q_PROPERTY(double pitch READ pitch WRITE setPitch NOTIFY pitchChanged)
  Q_PROPERTY(double heading READ heading WRITE setHeading NOTIFY headingChanged)

public:
  explicit ScenePropertiesExpressions(QObject* parent = nullptr);
  ~ScenePropertiesExpressions();

  static void init();

signals:
  void sceneViewChanged();
  void pitchChanged();
  void headingChanged();

private:
  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);
  void setPitch(double pitchInDegress);
  double pitch() const;
  void setHeading(double headingInDegrees);
  double heading() const;

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::Graphic* m_graphic = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay;
};

#endif // SCENEPROPERTIESEXPRESSIONS_H
