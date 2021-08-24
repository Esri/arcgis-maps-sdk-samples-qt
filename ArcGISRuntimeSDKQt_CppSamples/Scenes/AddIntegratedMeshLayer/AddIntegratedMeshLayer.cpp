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
namespace
{
  const QUrl meshLyrUrl("https://tiles.arcgis.com/tiles/z2tnIkrLQ2BRzr6P/arcgis/rest/services/Girona_Spain/SceneServer");
}

AddIntegratedMeshLayer::AddIntegratedMeshLayer(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(this)),
  m_integratedMeshLyr(new IntegratedMeshLayer(meshLyrUrl, this))
{
  // Connect the doneLoading signal to the handleError() method.
  connect(m_integratedMeshLyr, &IntegratedMeshLayer::doneLoading, this, &AddIntegratedMeshLayer::handleError);

  m_scene->operationalLayers()->append(m_integratedMeshLyr);

  setIntegratedMeshViewpoint();
}

void AddIntegratedMeshLayer::handleError(const Error& error)
{
  // If the doneLoading error message is not empty, an error has been encountered.
  if (!error.isEmpty())
  {
    if (error.additionalMessage().isEmpty())
      setErrorMessage(error.message());
    else
      setErrorMessage(error.message() + "\n" + error.additionalMessage());
  }
}

void AddIntegratedMeshLayer::setIntegratedMeshViewpoint()
{
  const Point initialPt(2.8259, 41.9906, 200.0, SpatialReference::wgs84());
  const Camera initialCamera(initialPt, 200.0, 190.0, 65.0, 0.0);
  const Viewpoint initialViewpoint(initialPt, initialPt.z(), initialCamera);
  m_scene->setInitialViewpoint(initialViewpoint);
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

QString AddIntegratedMeshLayer::errorMessage() const
{
  return m_errorMessage;
}

void AddIntegratedMeshLayer::setErrorMessage(const QString& message)
{
  m_errorMessage = message;
  emit errorMessageChanged();
}
