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
    class Graphic;
    class GraphicsOverlay;
    class MapQuickView;
    class ModelSceneSymbol;
    class SceneQuickView;
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

public:
  Animate3DSymbols(QQuickItem* parent = nullptr);
  ~Animate3DSymbols();

  void componentComplete() Q_DECL_OVERRIDE;

  Q_INVOKABLE void nextFrame();
  Q_INVOKABLE void changeMission(const QString& missionName);
  Q_INVOKABLE QAbstractItemModel* missionsModel() {return m_missionModel;}
  Q_INVOKABLE void setZoom(double zoomDist){m_zoomDist = zoomDist;}
  Q_INVOKABLE void setAngle(double angle){m_angle = angle;}
  Q_INVOKABLE void setFollowing(bool following);

  bool missionReady() const;

signals:
  void missionReadyChanged();

private:
  void createModel3d();
  void createModel2d(Esri::ArcGISRuntime::GraphicsOverlay* mapOverlay);
  void createRoute2d(Esri::ArcGISRuntime::GraphicsOverlay* mapOverlay);
  void createGraphic3D();
  void clearGraphic3D();

  static const QString HEADING;
  static const QString ROLL;
  static const QString PITCH;

  Esri::ArcGISRuntime::SceneQuickView* m_sceneView;
  Esri::ArcGISRuntime::MapQuickView* m_mapView;
  Esri::ArcGISRuntime::ModelSceneSymbol* m_model3d;
  Esri::ArcGISRuntime::Graphic* m_graphic3d;
  Esri::ArcGISRuntime::Graphic* m_graphic2d;
  Esri::ArcGISRuntime::Graphic* m_routeGraphic;
  QString m_dataPath;
  QAbstractItemModel* m_missionModel;
  std::unique_ptr< MissionData >  m_missionData;
  int m_frame;
  bool m_missionReady;
  double m_zoomDist;
  double m_angle;
  bool m_following;
};

#endif // ANIMATE3DSYMBOLS_H
