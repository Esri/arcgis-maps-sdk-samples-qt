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

#ifndef ORBITCAMERAAROUNDOBJECT_H
#define ORBITCAMERAAROUNDOBJECT_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Graphic;
    class ModelSceneSymbol;
    class OrbitGeoElementCameraController;
    class Scene;
    class SceneQuickView;
  }
}

#include <QObject>
#include <QPointF>

class OrbitCameraAroundObject : public QObject
{
  Q_OBJECT

  //Property declarations for interacting with the QML UI.
  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)
  Q_PROPERTY(bool allowCamDistanceInteraction READ camDistanceInteractionAllowed WRITE setCamDistanceInteractionAllowed(bool) NOTIFY camDistanceInteractionAllowedChanged)
  Q_PROPERTY(float planePitch READ planePitch() WRITE setPlanePitch(float) NOTIFY planePitchChanged)
  Q_PROPERTY(double cameraHeading READ cameraHeading() WRITE setCameraHeading(double) NOTIFY cameraHeadingChanged)
  Q_PROPERTY(QPointF cameraHeadingBounds READ cameraHeadingBounds() NOTIFY cameraHeadingBoundsChanged)

public:
  explicit OrbitCameraAroundObject(QObject* parent = nullptr);
  ~OrbitCameraAroundObject();

  static void init();

  //Methods that can be called from the QML UI, connected to the change-view buttons in the top left of the window
  Q_INVOKABLE void cockpitView();
  Q_INVOKABLE void centerView();

public slots:
  //Property setter methods allowing the QML UI to set model state, for changing plane orientation with the sliders, or toggling limits on camera zooming.
  void setCamDistanceInteractionAllowed(bool allowed);
  void setPlanePitch(float pitch);
  void setCameraHeading(double heading);

signals:
  void sceneViewChanged();
  void camDistanceInteractionAllowedChanged();
  void planePitchChanged();
  void cameraHeadingChanged();
  void cameraHeadingBoundsChanged();

private:
  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;

  //The plane that is displayed on screen, pitch property controlled via UI Sliders.
  Esri::ArcGISRuntime::Graphic* m_planeGraphic = nullptr;

  //Camera that orbits the plane in the scene, controlled via mouse/touch interaction or UI sliders
  Esri::ArcGISRuntime::OrbitGeoElementCameraController* m_orbitCam = nullptr;

  //Getter property exposing whether the the orbit-camera allows distance interaction to the QML UI, as it can be toggled there.
  bool camDistanceInteractionAllowed() const;
  float planePitch() const;
  double cameraHeading() const;
  QPointF cameraHeadingBounds() const;
};

#endif // ORBITCAMERAAROUNDOBJECT_H
