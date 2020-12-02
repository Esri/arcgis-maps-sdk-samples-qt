// [WriteFile Name=DisplaySceneLayer, Category=Scenes]
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

#include "DisplaySceneLayer.h"

#include "Scene.h"
#include "SceneQuickView.h"
#include "Basemap.h"
#include "ArcGISTiledElevationSource.h"
#include "ArcGISSceneLayer.h"
#include "Point.h"
#include "Viewpoint.h"
#include "Camera.h"

using namespace Esri::ArcGISRuntime;

DisplaySceneLayer::DisplaySceneLayer(QQuickItem* parent) :
  QQuickItem(parent)
{
}

DisplaySceneLayer::~DisplaySceneLayer() = default;

void DisplaySceneLayer::init()
{
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<DisplaySceneLayer>("Esri.Samples", 1, 0, "DisplaySceneLayerSample");
}

void DisplaySceneLayer::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML SceneView component
  m_sceneView = findChild<SceneQuickView*>("sceneView");

  // create a new scene instance
  Basemap* basemap = Basemap::topographic(this);
  m_scene = new Scene(basemap, this);

  //! [add a scene service with ArcGISSceneLayer]
  m_sceneLayer = new ArcGISSceneLayer(QUrl("https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/Buildings_Brest/SceneServer/layers/0"), this);
  m_scene->operationalLayers()->append(m_sceneLayer);
  //! [add a scene service with ArcGISSceneLayer]

  // create a new elevation source and add to scene
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);
  m_scene->baseSurface()->elevationSources()->append(elevationSource);

  // create a camera and set the initial viewpoint
  Point pt(-4.49779155626782, 48.38282454039932, 62.013264927081764, SpatialReference(4326));
  Camera camera(pt, 41.64729875588979, 71.2017391571523, 2.194677223e-314);
  Viewpoint initViewpoint(pt, camera);
  m_scene->setInitialViewpoint(initViewpoint);

  // set scene on the scene view
  m_sceneView->setArcGISScene(m_scene);
}

