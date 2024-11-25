// [WriteFile Name=FilterFeaturesInScene, Category=Scenes]
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

#ifndef FILTERFEATURESINSCENE_H
#define FILTERFEATURESINSCENE_H

// ArcGIS Maps SDK headers
#include "Polygon.h"

// Qt headers
#include <QObject>

namespace Esri::ArcGISRuntime
{
class ArcGISSceneLayer;
class Graphic;
class Scene;
class SceneLayerPolygonFilter;
class SceneQuickView;
}

Q_MOC_INCLUDE("SceneQuickView.h");

class FilterFeaturesInScene : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)

public:
  explicit FilterFeaturesInScene(QObject* parent = nullptr);
  ~FilterFeaturesInScene() override;

  static void init();

  Q_INVOKABLE void loadScene();
  Q_INVOKABLE void filterScene();
  Q_INVOKABLE void reset();

signals:
  void sceneViewChanged();

private:
  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::ArcGISSceneLayer* m_osmBuildings = nullptr;
  Esri::ArcGISRuntime::ArcGISSceneLayer* m_detailedBuildingsSceneLayer = nullptr;
  Esri::ArcGISRuntime::Polygon m_sceneLayerExtentPolygon;
  Esri::ArcGISRuntime::Graphic* m_sanFranciscoExtentGraphic = nullptr;
  Esri::ArcGISRuntime::SceneLayerPolygonFilter* m_sceneLayerPolygonFilter = nullptr;
};

#endif // FILTERFEATURESINSCENE_H
