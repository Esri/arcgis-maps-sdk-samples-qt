// [WriteFile Name=Animate3DSymbols, Category=Scenes]
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

#ifndef ANIMATE3DSYMBOLS_H
#define ANIMATE3DSYMBOLS_H

class MissionData;

namespace Esri
{
  namespace ArcGISRuntime
  {
    class GlobeCameraController;
    class Graphic;
    class GraphicsOverlay;
    class MapQuickView;
    class ModelSceneSymbol;
    class OrbitGeoElementCameraController;
    class SceneQuickView;
    class SimpleMarkerSymbol;
  }
}

class QAbstractListModel;
class MissionData;

#include <QQuickItem>
#include <QString>

#include <memory>

class Animate3DSymbols : public QQuickItem
{
  Q_PROPERTY(bool missionReady READ missionReady NOTIFY missionReadyChanged)
  Q_PROPERTY(int missionSize READ missionSize NOTIFY missionSizeChanged)
  Q_PROPERTY(int missionFrame READ missionFrame WRITE setMissionFrame NOTIFY missionFrameChanged)
  Q_PROPERTY(QAbstractListModel* missionsModel READ missionsModel CONSTANT)
  Q_PROPERTY(double minZoom READ minZoom NOTIFY minZoomChanged)
  Q_PROPERTY(double zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
  Q_PROPERTY(double angle READ angle WRITE setAngle NOTIFY angleChanged)
  Q_OBJECT

public:
  Animate3DSymbols(QQuickItem* parent = nullptr);
  ~Animate3DSymbols() override;

  void componentComplete() override;
  static void init();

  Q_INVOKABLE void animate();
  Q_INVOKABLE void changeMission(const QString& missionName);
  QAbstractListModel* missionsModel();
  Q_INVOKABLE void zoomMapIn();
  Q_INVOKABLE void zoomMapOut();
  Q_INVOKABLE void viewWidthChanged(bool sceneViewIsWider);
  Q_INVOKABLE void setFollowing(bool following);

  bool missionReady() const;
  int missionSize() const;
  int missionFrame() const;
  double zoom() const;
  double angle() const;
  double minZoom() const;

  void setMissionFrame(int newFrame);
  void setZoom(double zoomDist);
  void setAngle(double angle);
  void setSpeed(int intervalMs);

signals:
  void missionReadyChanged();
  void missionSizeChanged();
  void nextFrameRequested();
  void minZoomChanged();
  void zoomChanged();
  void angleChanged();
  void missionFrameChanged();

private:
  void createModel2d(Esri::ArcGISRuntime::GraphicsOverlay* mapOverlay);
  void createRoute2d(Esri::ArcGISRuntime::GraphicsOverlay* mapOverlay);
  void createGraphic3D();

  static const QString HEADING;
  static const QString ROLL;
  static const QString PITCH;
  static const QString ANGLE;

  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::ModelSceneSymbol* m_model3d = nullptr;
  Esri::ArcGISRuntime::Graphic* m_graphic3d = nullptr;
  Esri::ArcGISRuntime::Graphic* m_graphic2d = nullptr;
  Esri::ArcGISRuntime::SimpleMarkerSymbol* m_symbol2d = nullptr;
  Esri::ArcGISRuntime::Graphic* m_routeGraphic = nullptr;
  Esri::ArcGISRuntime::GlobeCameraController* m_globeController = nullptr;
  Esri::ArcGISRuntime::OrbitGeoElementCameraController* m_followingController = nullptr;
  QString m_dataPath;
  QAbstractListModel* m_missionsModel = nullptr;
  std::unique_ptr<MissionData> m_missionData;
  int m_frame = 0;
  double m_mapZoomFactor = 5.0;
};

#endif // ANIMATE3DSYMBOLS_H
