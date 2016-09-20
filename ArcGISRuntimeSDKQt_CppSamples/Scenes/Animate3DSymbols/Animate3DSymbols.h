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

//#define ANIMATE_MAP

class MissionData;

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Graphic;
    class GraphicsOverlay;
    class MapQuickView;
    class ModelSceneSymbol;
    class SceneQuickView;
    class TaskWatcher;
  }
}

class QAbstractItemModel;

#include <QQuickItem>
#include <QString>

#include <memory>

class Animate3DSymbols : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(bool missionReady READ missionReady NOTIFY missionReadyChanged)
  Q_PROPERTY(int missionSize READ missionSize NOTIFY missionSizeChanged)
  Q_PROPERTY(bool following READ following WRITE setFollowing NOTIFY followingChanged)
  Q_PROPERTY(int missionFrame READ missionFrame WRITE setMissionFrame)
  Q_PROPERTY(double zoom READ zoom WRITE setZoom)
  Q_PROPERTY(double angle READ angle WRITE setAngle)
  Q_PROPERTY(int speed READ speed WRITE setSpeed)

public:
  Animate3DSymbols(QQuickItem* parent = nullptr);
  ~Animate3DSymbols();

  void componentComplete() Q_DECL_OVERRIDE;


  Q_INVOKABLE void animate();
  Q_INVOKABLE void changeMission(const QString& missionName);
  Q_INVOKABLE QAbstractItemModel* missionsModel() {return m_missionsModel;}
  Q_INVOKABLE void zoomMapIn();
  Q_INVOKABLE void zoomMapOut();
  Q_INVOKABLE void viewWidthChanged(bool sceneViewIsWider);

  bool missionReady() const;
  int missionSize() const;
  bool following() const;
  int missionFrame() const;
  double zoom() const;
  double angle() const;
  int speed() const;

  void setFollowing(bool following);
  void setMissionFrame(int newFrame);
  void setZoom(double zoomDist);
  void setAngle(double angle);
  void setSpeed(int intervalMs);

signals:
  void missionReadyChanged();
  void missionSizeChanged();
  void followingChanged();
  void nextFrameRequested();

private:
  void createModel3d();
  void createModel2d(Esri::ArcGISRuntime::GraphicsOverlay* mapOverlay);
  void createRoute2d(Esri::ArcGISRuntime::GraphicsOverlay* mapOverlay);
  void createGraphic3D();
  void clearGraphic3D();

  static const QString HEADING;
  static const QString ROLL;
  static const QString PITCH;
  static const QString ANGLE;

  struct CameraHandler;

  Esri::ArcGISRuntime::SceneQuickView* m_sceneView;
  Esri::ArcGISRuntime::MapQuickView* m_mapView;
  Esri::ArcGISRuntime::ModelSceneSymbol* m_model3d;
  Esri::ArcGISRuntime::Graphic* m_graphic3d;
  Esri::ArcGISRuntime::Graphic* m_graphic2d;
  Esri::ArcGISRuntime::Graphic* m_routeGraphic;
  QString m_dataPath;
  QAbstractItemModel* m_missionsModel;
  std::unique_ptr<MissionData> m_missionData;
  std::unique_ptr<CameraHandler> m_camHandler;
  int m_frame;
  bool m_following;
  double m_mapZoomFactor;
};

#endif // ANIMATE3DSYMBOLS_H
