// [WriteFile Name=OrbitCameraAroundObject, Category=Scenes]
// [Legal]
// Copyright 2019 Esri.

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

#include "OrbitCameraAroundObject.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "ModelSceneSymbol.h"
#include "Graphic.h"
#include "SceneQuickView.h"
#include "OrbitGeoElementCameraController.h"

#include <QDir>

#include <utility>

using namespace Esri::ArcGISRuntime;

namespace
{
  // helper method to get cross platform data path
  QString defaultDataPath()
  {
    QString dataPath;

    #ifdef Q_OS_ANDROID
      dataPath = "/sdcard";
    #elif defined Q_OS_IOS
      dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    #else
      dataPath = QDir::homePath();
    #endif

    return dataPath;
  }

  //TODO: Is this 2 responsibilities? Factor it out?
  //TODO: Load error handling
  //Load a 3d model from the specified URL. Create a graphic, and add it to the sceneview so it's rendered.
  //Precondition : sceneView must have graphics overlay
  std::pair<ModelSceneSymbol*, Graphic*> load3dModelAndAddToScene(QUrl modelPath,
                                                                  SceneQuickView& sceneView,
                                                                  float modelScale,
                                                                  Point initialPosition,
                                                                  QObject* parent)
  {
    //SceneView must have at least one graphics overlay to add the graphic to.
    assert(sceneView.graphicsOverlays()->size() > 0);

    ModelSceneSymbol* model = new ModelSceneSymbol(modelPath, modelScale, parent);
    model->load();
    Graphic* graphic = new Graphic(initialPosition, model, parent);
    sceneView.graphicsOverlays()->at(0)->graphics()->append(graphic);
    return {model, graphic};
  }
}

OrbitCameraAroundObject::OrbitCameraAroundObject(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(Basemap::imagery(this), this))
{
  // create a new elevation source from Terrain3D REST service
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
        QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);
}

OrbitCameraAroundObject::~OrbitCameraAroundObject() = default;

void OrbitCameraAroundObject::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<OrbitCameraAroundObject>("Esri.Samples", 1, 0, "OrbitCameraAroundObjectSample");
}

SceneQuickView* OrbitCameraAroundObject::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void OrbitCameraAroundObject::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
  {
    return;
  }

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);


  // create a new graphics overlay and add it to the sceneview
  GraphicsOverlay* sceneOverlay = new GraphicsOverlay(this);
  sceneOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::Absolute));
  m_sceneView->graphicsOverlays()->append(sceneOverlay);


  QUrl planeFileURl = QUrl(defaultDataPath() + "/ArcGIS/Runtime/Data/3D/Bristol/Collada/Bristol.dae");
  Point runwayPos = Point{0.0, 51.4934, 1000, SpatialReference::wgs84()}; //Just over London
  std::pair<ModelSceneSymbol*, Graphic*> plane = load3dModelAndAddToScene(planeFileURl, *m_sceneView, 2.0, runwayPos, this);

  // create the camera controller to follow the plane
  OrbitGeoElementCameraController* orbitCam = new OrbitGeoElementCameraController(plane.second, 500, this);

  // restrict the camera's heading to stay behind the plane
  orbitCam->setMinCameraHeadingOffset(-45);
  orbitCam->setMaxCameraHeadingOffset(45);

  // restrict the camera's pitch so it doesn't go completely vertical or collide with the ground
  orbitCam->setMinCameraPitchOffset(10);
  orbitCam->setMaxCameraPitchOffset(100);

  // restrict the camera to stay between 10 and 1000 meters from the plane
  orbitCam->setMinCameraDistance(10);
  orbitCam->setMaxCameraDistance(100);

  // position the plane a third from the bottom of the screen
  orbitCam->setTargetVerticalScreenFactor(0.33f);

  // don't pitch the camera when the plane pitches
  orbitCam->setAutoPitchEnabled(false);

  m_sceneView->setCameraController(orbitCam);





  emit sceneViewChanged();
}

