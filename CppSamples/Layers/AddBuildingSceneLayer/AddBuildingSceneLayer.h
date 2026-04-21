// [WriteFile Name=AddBuildingSceneLayer, Category=Layers]
// [Legal]
// Copyright 2025 Esri.
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

#ifndef ADDBUILDINGSCENELAYER_H
#define ADDBUILDINGSCENELAYER_H

// Qt headers
#include <QObject>

namespace Esri::ArcGISRuntime
{
  class BuildingSceneLayer;
  class BuildingSublayer;
  class LocalSceneQuickView;
  class Scene;
} // namespace Esri::ArcGISRuntime

Q_MOC_INCLUDE("LocalSceneQuickView.h");

class AddBuildingSceneLayer : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::LocalSceneQuickView* localSceneView READ localSceneView WRITE setLocalSceneView NOTIFY localSceneViewChanged)
  Q_PROPERTY(bool layerLoaded READ layerLoaded NOTIFY layerLoadedChanged)

public:
  explicit AddBuildingSceneLayer(QObject* parent = nullptr);
  ~AddBuildingSceneLayer() override;

  static void init();
  Q_INVOKABLE void setFullModelAndOverviewVisibility(bool showFullModel);
  bool layerLoaded() const;

signals:
  void layerLoadedChanged();
  void localSceneViewChanged();

private:
  Esri::ArcGISRuntime::LocalSceneQuickView* localSceneView() const;
  void setLocalSceneView(Esri::ArcGISRuntime::LocalSceneQuickView* localSceneView);

  Esri::ArcGISRuntime::BuildingSceneLayer* m_buildingSceneLayer = nullptr;
  Esri::ArcGISRuntime::BuildingSublayer* m_fullModelSublayer = nullptr;
  Esri::ArcGISRuntime::LocalSceneQuickView* m_localSceneView = nullptr;
  Esri::ArcGISRuntime::BuildingSublayer* m_overviewSublayer = nullptr;
  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
};

#endif // ADDBUILDINGSCENELAYER_H
