// [WriteFile Name=BasicSceneView, Category=Scenes]
// [Legal]
// Copyright 2022 Esri.
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "BasicSceneView.h"

// ArcGIS Maps SDK headers
#include "ArcGISTiledElevationSource.h"
#include "Camera.h"
#include "ElevationSourceListModel.h"
#include "MapTypes.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "Surface.h"

using namespace Esri::ArcGISRuntime;

BasicSceneView::BasicSceneView(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(BasemapStyle::ArcGISImagery, this))
{
  //! [create a new elevation source]
  // create a new elevation source from Terrain3D REST service
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
        QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);
  //! [create a new elevation source]

}

BasicSceneView::~BasicSceneView() = default;

void BasicSceneView::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<BasicSceneView>("Esri.Samples", 1, 0, "BasicSceneViewSample");
}

SceneQuickView* BasicSceneView::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void BasicSceneView::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
    return;

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  //! [create a camera]
  // create a camera
  const double latitude = 28.4;
  const double longitude = 83.9;
  const double altitude = 10010.0;
  const double heading = 10.0;
  const double pitch = 80.0;
  const double roll = 0.0;
  Camera camera(latitude, longitude, altitude, heading, pitch, roll);

  // set the viewpoint
  m_sceneView->setViewpointCameraAndWait(camera);
  //! [create a camera]

  emit sceneViewChanged();
}
