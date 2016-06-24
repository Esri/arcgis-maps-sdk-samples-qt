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
    QQuickItem(parent),
    m_scene(nullptr),
    m_sceneView(nullptr),
    m_sceneLayer(nullptr)
{
}

DisplaySceneLayer::~DisplaySceneLayer()
{
}

void DisplaySceneLayer::componentComplete()
{
    QQuickItem::componentComplete();

    // find QML SceneView component
    m_sceneView = findChild<SceneQuickView*>("sceneView");

    // create a new scene instance
    Basemap* basemap = Basemap::topographic(this);
    m_scene = new Scene(basemap, this);

    // add a scene service with ArcGISSceneLayer
    m_sceneLayer = new ArcGISSceneLayer(QUrl("http://scene.arcgis.com/arcgis/rest/services/Hosted/Buildings_Brest/SceneServer/layers/0"));
    m_scene->operationalLayers()->append(m_sceneLayer);;

    // create a new elevation source and add to scene
    ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(QUrl("http://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);
    m_scene->baseSurface()->elevationSources()->append(elevationSource);

    // create a camera and set the initial viewpoint
    Camera camera(28.4, 83.9, 10010.0, 10.0, 80.0, 300.0);
    Viewpoint initViewpoint(Point(-4.49779155626782, 48.38282454039932, m_sceneView->spatialReference()), 62.0132, camera);
    m_scene->setInitialViewpoint(initViewpoint);

    // set scene on the scene view
    m_sceneView->setScene(m_scene);
}

