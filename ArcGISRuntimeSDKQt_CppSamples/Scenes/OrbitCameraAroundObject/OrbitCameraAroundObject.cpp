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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "OrbitCameraAroundObject.h"

#include "ArcGISTiledElevationSource.h"
#include "Graphic.h"
#include "ModelSceneSymbol.h"
#include "OrbitGeoElementCameraController.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "SimpleRenderer.h"

#ifdef Q_OS_IOS
#include <QStandardPaths>
#endif

#include <QDir>

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

  /* Scene Rendering setup */
  // create a new graphics overlay and add it to the sceneview
  GraphicsOverlay* sceneOverlay = new GraphicsOverlay(this);
  sceneOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::Relative));
  m_sceneView->graphicsOverlays()->append(sceneOverlay);

  // create a renderer to control the plane's orientation by its attributes
  SimpleRenderer* renderer3D = new SimpleRenderer(this);
  {
    //Set the renderer pitch/heading expressions, so the plane can be oriented via properties.
    RendererSceneProperties properties = renderer3D->sceneProperties();
    properties.setPitchExpression("[PITCH]");
    properties.setHeadingExpression("[HEADING]");
    renderer3D->setSceneProperties(properties);
  }
  sceneOverlay->setRenderer(renderer3D);


  /* Plane model Setup */
  //Create a plane model.
  QUrl planeFileURl = QUrl(defaultDataPath() + "/ArcGIS/Runtime/Data/3D/Bristol/Collada/Bristol.dae");
  ModelSceneSymbol* planeModel = new ModelSceneSymbol(planeFileURl, 1.0, this);

  //Create a graphic from the plane model, positioned atop a runway.
  Point runwayPos = Point{6.637, 45.399, 100, SpatialReference::wgs84()};
  m_planeGraphic = new Graphic(runwayPos, planeModel, this);

  //Create the plane orientation attributes that are going to be used. Orient the heading 45 degs so the plane faces down the runway.
  m_planeGraphic->attributes()->insertAttribute("PITCH", 0.0);
  m_planeGraphic->attributes()->insertAttribute("HEADING", 45.0);

  //Add the plane graphic to the scene.
  m_sceneView->graphicsOverlays()->at(0)->graphics()->append(m_planeGraphic);


  /* Orbiting Camera setup */
  // create the camera controller to follow the plane
  m_orbitCam = new OrbitGeoElementCameraController(m_planeGraphic, 50, this);

  // restrict the camera's heading to stay behind the plane
  // Note : Since this is exposed and hooked up as a property, the heading slider in the UI will automatically adjust its bounds, try changing these values.
  m_orbitCam->setMinCameraHeadingOffset(-45);
  m_orbitCam->setMaxCameraHeadingOffset(45);
  emit cameraHeadingBoundsChanged();

  // restrict the camera's pitch so it doesn't go completely vertical or collide with the ground
  m_orbitCam->setMinCameraPitchOffset(10);
  m_orbitCam->setMaxCameraPitchOffset(100);

  // restrict the camera to stay between 10 and 1000 meters from the plane
  m_orbitCam->setMinCameraDistance(10);
  m_orbitCam->setMaxCameraDistance(100);

  // position the plane a third from the bottom of the screen
  m_orbitCam->setTargetVerticalScreenFactor(0.33f);
  // don't pitch the camera when the plane pitches
  m_orbitCam->setAutoPitchEnabled(false);

  // Forward the camera heading changed signal on to the QML property signal.
  // This allows the "camera heading" slider to react to changing the camera heading via mouse motion
  // See "onMoved" signal handler in OrbitCameraAroundObject.qml
  connect(m_orbitCam, &OrbitGeoElementCameraController::cameraHeadingOffsetChanged, this, &OrbitCameraAroundObject::cameraHeadingChanged);

  //Apply our new orbiting camera to the scene view.
  m_sceneView->setCameraController(m_orbitCam);

  emit sceneViewChanged();
}

void OrbitCameraAroundObject::cockpitView()
{
  /*
   * Animates the camera to a cockpit view. The camera's target is offset to the cockpit (instead of the plane's
   * center). The camera is moved onto the target position to create a swivelling camera effect. Auto pitch is
   * enabled so the camera pitches when the plane pitches.
   */

  //No zooming in/out by default in cockpit view
  setCamDistanceInteractionAllowed(false);

  //allow the camera to get closer to the target
  m_orbitCam->setMinCameraDistance(0);

  // animate the camera target to the cockpit instead of the center of the plane
  m_orbitCam->setTargetOffsets(0, -2, 1.1, 1);

  //The animation may rotate us over the set camera bounds based on the plane pitch, so unlock them.
  m_orbitCam->setMinCameraPitchOffset(-180.0);
  m_orbitCam->setMaxCameraPitchOffset(180.0);

  //Camera move-to-cockpit callback.
  connect(m_orbitCam, &OrbitGeoElementCameraController::moveCameraCompleted, this,
          [this](QUuid, bool succeeded)
  {
    if (succeeded)
    {
      //once the camera is in the cockpit, only allow the camera's heading to change
      m_orbitCam->setMinCameraPitchOffset(90);
      m_orbitCam->setMaxCameraPitchOffset(90);

      // pitch the camera when the plane pitches
      m_orbitCam->setAutoPitchEnabled(true);
    }
  });

  //Start the camera move into the cockpit
  //If the camera is already tracking object pitch, don't want to animate the pitch any further, we're exactly where we should be.
  m_orbitCam->moveCamera(0 - m_orbitCam->cameraDistance(),
                         0 - m_orbitCam->cameraHeadingOffset(),
                         m_orbitCam->isAutoPitchEnabled() ? 0.0 : (90 - m_orbitCam->cameraPitchOffset()) + planePitch(), 1.0);

}

void OrbitCameraAroundObject::centerView()
{
  /*
   * Configures the camera controller for a "follow" view. The camera targets the center of the plane with a default
   * position directly behind and slightly above the plane. Auto pitch is disabled so the camera does not pitch when
   * the plane pitches.
   */

  setCamDistanceInteractionAllowed(true);
  m_orbitCam->setAutoPitchEnabled(false);
  m_orbitCam->setTargetOffsetX(0);
  m_orbitCam->setTargetOffsetY(0);
  m_orbitCam->setTargetOffsetZ(0);
  m_orbitCam->setCameraHeadingOffset(0);
  m_orbitCam->setMinCameraPitchOffset(10);
  m_orbitCam->setMaxCameraPitchOffset(100);
  m_orbitCam->setCameraPitchOffset(45);
  m_orbitCam->setMinCameraDistance(10.0);
  m_orbitCam->setCameraDistance(50.0);
}

// QML Property methods, (see the Q_PROPERTY declarations in header file,) for allowing the model to interact with the UI
// We need to do nullptr checks here as it's entirely possible, and even expected, that these getter properties will be called by the UI
// before setSceneView has been called. Objects like the camera are not initialised until then, so return sensible defaults if the object is null.
bool OrbitCameraAroundObject::camDistanceInteractionAllowed() const
{
  return m_orbitCam != nullptr ? m_orbitCam->isCameraDistanceInteractive() : true;
}

void OrbitCameraAroundObject::setCamDistanceInteractionAllowed(bool allowed)
{
  //Check the allowed flags arn't the same on set to avoid a binding loop
  if ((m_orbitCam != nullptr) && (allowed != m_orbitCam->isCameraDistanceInteractive()))
  {
    m_orbitCam->setCameraDistanceInteractive(allowed);
    emit camDistanceInteractionAllowedChanged();
  }
}

double OrbitCameraAroundObject::cameraHeading() const
{
  return m_orbitCam != nullptr ? m_orbitCam->cameraHeadingOffset() : 0.0;
}

void OrbitCameraAroundObject::setCameraHeading(double heading)
{
  if (m_orbitCam != nullptr)
  {
    m_orbitCam->setCameraHeadingOffset(heading);
    emit cameraHeadingChanged();
  }
}

float OrbitCameraAroundObject::planePitch() const
{
  if(!m_planeGraphic->attributes()->containsAttribute("PITCH"))
  {
    return 0.0;
  }

  return m_planeGraphic->attributes()->attributeValue("PITCH").toFloat();
}

void OrbitCameraAroundObject::setPlanePitch(float pitch)
{
  if(!m_planeGraphic->attributes()->containsAttribute("PITCH"))
  {
     m_planeGraphic->attributes()->insertAttribute("PITCH", pitch);
  }
  else
  {
     m_planeGraphic->attributes()->replaceAttribute("PITCH", pitch);
  }
  emit planePitchChanged();
}

QPointF OrbitCameraAroundObject::cameraHeadingBounds() const
{
  return m_orbitCam != nullptr ? QPointF{m_orbitCam->minCameraHeadingOffset(), m_orbitCam->maxCameraHeadingOffset()}
                               : QPointF{-45.0, 45.0};
}
