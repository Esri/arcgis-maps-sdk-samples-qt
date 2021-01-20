// [WriteFile Name=BasicSceneView, Category=Scenes]
// [Legal]
// Copyright 2015 Esri.

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

#include "BasicSceneView.h"
#include "Scene.h"
#include "SceneGraphicsView.h"
#include "ArcGISTiledElevationSource.h"
#include "Camera.h"
#include <QVBoxLayout>

using namespace Esri::ArcGISRuntime;

BasicSceneView::BasicSceneView(QWidget* parent) :
  QWidget(parent)
{    
    // Create a scene using the Imagery basemap
    m_scene = new Scene(BasemapStyle::ArcGISImageryStandard, this);

    // Create a scene view, and pass in the scene
    m_sceneView = new SceneGraphicsView(m_scene, this);

    // create an elevation source
    ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);
    m_scene->baseSurface()->elevationSources()->append(elevationSource);

    // create a camera
    Camera camera(28.4, 83.9, 10010.0, 10.0, 80.0, 0.0);
    // set the viewpoint to the camera
    m_sceneView->setViewpointCameraAndWait(camera);

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_sceneView);
    setLayout(vBoxLayout);
}

BasicSceneView::~BasicSceneView() = default;
