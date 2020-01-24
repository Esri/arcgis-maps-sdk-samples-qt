// [WriteFile Name=ExploreScenesInFlyoverAR, Category=AR]
// [Legal]
// Copyright 2020 Esri.

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

#ifndef EXPLORESCENESINFLYOVERAR_H
#define EXPLORESCENESINFLYOVERAR_H

namespace Esri
{
namespace ArcGISRuntime
{
class Scene;
class SceneQuickView;
class IntegratedMeshLayer;
}
}

#include <QObject>
#include "ArcGISArView.h"

class ExploreScenesInFlyoverAR : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::Toolkit::ArcGISArView* arcGISArView READ arcGISArView WRITE setArcGISArView
             NOTIFY arcGISArViewChanged)

public:
  explicit ExploreScenesInFlyoverAR(QObject* parent = nullptr);
  ~ExploreScenesInFlyoverAR();

  static void init();

signals:
  void sceneViewChanged();
  void arcGISArViewChanged();

private:
  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);
  Esri::ArcGISRuntime::Toolkit::ArcGISArView* arcGISArView() const;
  void setArcGISArView(Esri::ArcGISRuntime::Toolkit::ArcGISArView* arcGISArView);

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::Toolkit::ArcGISArView* m_arcGISArView = nullptr;
  Esri::ArcGISRuntime::IntegratedMeshLayer* m_integratedMeshLayer = nullptr;
  Esri::ArcGISRuntime::Camera m_originCamera;
};

#endif // EXPLORESCENESINFLYOVERAR_H
