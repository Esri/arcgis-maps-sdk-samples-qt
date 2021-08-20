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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "AddIntegratedMeshLayer.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "IntegratedMeshLayer.h"

using namespace Esri::ArcGISRuntime;

AddIntegratedMeshLayer::AddIntegratedMeshLayer(QObject* parent /* = nullptr */):
  QObject(parent)
{
  // create the integrated mesh layer
  const QUrl meshLyrUrl("https://tiles.arcgis.com/tiles/z2tnIkrLQ2BRzr6P/arcgis/rest/services/Girona_Spain/SceneServer");
  m_integratedMeshLyr = new IntegratedMeshLayer(meshLyrUrl, this);

  // Connect the done loading signal to the handleLoadingCompleted() method.
  connect(m_integratedMeshLyr, &IntegratedMeshLayer::doneLoading, this, &AddIntegratedMeshLayer::handleLoadingCompleted);

  // Load mesh layer
  m_integratedMeshLyr->load();
}

void AddIntegratedMeshLayer::handleLoadingCompleted(Error error)
{
  if (error.isEmpty())
  {
    createSceneForIntegratedMesh();

    m_scene->operationalLayers()->append(m_integratedMeshLyr);

    setIntegratedMeshViewpoint();
  }
  else
  {
    createDefaultScene();

    seterrorWhileLoading(true);
  }
}

void AddIntegratedMeshLayer::createSceneForIntegratedMesh()
{
  m_scene = new Scene(this);
  m_sceneView->setArcGISScene(m_scene);
  emit sceneViewChanged();
}

void AddIntegratedMeshLayer::setIntegratedMeshViewpoint()
{
  const Point initialPt(2.8259, 41.9906, 200.0, SpatialReference::wgs84());
  const Camera initialCamera(initialPt, 200.0, 190.0, 65.0, 0.0);
  const Viewpoint initialViewpoint(initialPt, initialPt.z(), initialCamera);
  m_scene->setInitialViewpoint(initialViewpoint);
}

void AddIntegratedMeshLayer::createDefaultScene()
{
  m_scene = new Scene(BasemapStyle::ArcGISImagery, this);
  m_sceneView->setArcGISScene(m_scene);
  emit sceneViewChanged();
}

AddIntegratedMeshLayer::~AddIntegratedMeshLayer() = default;

void AddIntegratedMeshLayer::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<AddIntegratedMeshLayer>("Esri.Samples", 1, 0, "AddIntegratedMeshLayerSample");
}

SceneQuickView* AddIntegratedMeshLayer::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void AddIntegratedMeshLayer::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
  {
    return;
  }

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  emit sceneViewChanged();
}

bool AddIntegratedMeshLayer::errorWhileLoading() const
{
  return m_errorWhileLoading;
}

void AddIntegratedMeshLayer::seterrorWhileLoading(bool errorWhileLoadingStatus)
{
  m_errorWhileLoading = errorWhileLoadingStatus;
  emit errorWhileLoadingStatusChanged();
}
