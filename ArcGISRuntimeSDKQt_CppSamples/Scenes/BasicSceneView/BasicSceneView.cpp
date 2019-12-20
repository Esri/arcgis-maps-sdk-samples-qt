// [WriteFile Name=BasicSceneView, Category=Scenes]
// [Legal]
// Copyright 2016 Esri.

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

#include "BasicSceneView.h"

#include "Scene.h"
#include "SceneQuickView.h"
#include "Basemap.h"
#include "ArcGISTiledElevationSource.h"

using namespace Esri::ArcGISRuntime;

BasicSceneView::BasicSceneView(QQuickItem* parent) :
  QQuickItem(parent)
{
}

BasicSceneView::~BasicSceneView() = default;

void BasicSceneView::init()
{
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<BasicSceneView>("Esri.Samples", 1, 0, "BasicSceneSample");
}

void BasicSceneView::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML SceneView component
  m_sceneView = findChild<SceneQuickView*>("sceneView");

  // create a new basemap instance
  Basemap* basemap = Basemap::imagery(this);
  // create a new scene instance
  m_scene = new Scene(basemap, this);
  // set scene on the scene view
  m_sceneView->setArcGISScene(m_scene);

  //! [create a new elevation source]
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);
  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);
  //! [create a new elevation source]

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
}

