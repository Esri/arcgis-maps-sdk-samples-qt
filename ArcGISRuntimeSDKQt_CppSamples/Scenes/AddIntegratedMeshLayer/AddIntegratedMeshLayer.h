// [WriteFile Name=AddIntegratedMeshLayer, Category=Scenes]
// [Legal]
// Copyright 2018 Esri.

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

#ifndef ADDINTEGRATEDMESHLAYER_H
#define ADDINTEGRATEDMESHLAYER_H

namespace Esri
{
namespace ArcGISRuntime
{
class Scene;
class SceneQuickView;
class IntegratedMeshLayer;
class Error;
}
}

#include <QObject>
/*FOR TESTING*/#include <MapTypes.h>

class AddIntegratedMeshLayer : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)
  Q_PROPERTY(bool errorWhileLoading READ errorWhileLoading WRITE seterrorWhileLoading NOTIFY errorWhileLoadingStatusChanged)

public:
  explicit AddIntegratedMeshLayer(QObject* parent = nullptr);
  ~AddIntegratedMeshLayer() override;

  static void init();

signals:
  void sceneViewChanged();
  void errorWhileLoadingStatusChanged();

private:
  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);
  bool errorWhileLoading() const;
  void seterrorWhileLoading(bool errorWhileLoadingStatus);

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::IntegratedMeshLayer* m_integratedMeshLyr = nullptr;
  bool m_errorWhileLoading = false;

  void handleLoadingCompleted(Esri::ArcGISRuntime::Error error);
  void createSceneForIntegratedMesh();
  void setIntegratedMeshViewpoint();
  void createDefaultScene();
};

#endif // ADDINTEGRATEDMESHLAYER_H
