// [WriteFile Name=ShowExploratoryLineOfSightBetweenGeoelements, Category=Analysis]
// [Legal]
// Copyright 2019 Esri.
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

#ifndef ShowExploratoryLineOfSightBetweenGeoelements_H
#define ShowExploratoryLineOfSightBetweenGeoelements_H

// Qt headers
#include <QObject>
#include <QTimer>

namespace Esri::ArcGISRuntime
{
  class Scene;
  class SceneQuickView;
  class Graphic;
} // namespace Esri::ArcGISRuntime

Q_MOC_INCLUDE("SceneQuickView.h")

class ShowExploratoryLineOfSightBetweenGeoelements : public QObject
{
  Q_OBJECT

  Q_PROPERTY(double heightZ READ heightZ WRITE setHeightZ NOTIFY heightZChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)

public:
  explicit ShowExploratoryLineOfSightBetweenGeoelements(QObject* parent = nullptr);
  ~ShowExploratoryLineOfSightBetweenGeoelements() override;

  static void init();

signals:
  void heightZChanged();
  void sceneViewChanged();

private slots:
  void animate();

private:
  double heightZ() const;
  void setHeightZ(double z);

  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);
  void initialize();

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;

  QTimer m_animation;
  std::size_t m_waypointIndex = 0;
  Esri::ArcGISRuntime::Graphic* m_taxi = nullptr;
  Esri::ArcGISRuntime::Graphic* m_observer = nullptr;
};

#endif // ShowExploratoryLineOfSightBetweenGeoelements_H
