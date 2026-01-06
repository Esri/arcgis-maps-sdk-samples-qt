// [WriteFile Name=ShowCallout, Category=DisplayInformation]
// [Legal]
// Copyright 2016 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifndef SHOWCALLOUT_H
#define SHOWCALLOUT_H

// Qt headers
#include <QObject>

namespace Esri::ArcGISRuntime
{
  class Map;
  class MapQuickView;
  class Scene;
  class SceneQuickView;
  class LocalSceneQuickView;
} // namespace Esri::ArcGISRuntime

Q_MOC_INCLUDE("MapQuickView.h");
Q_MOC_INCLUDE("SceneQuickView.h");
Q_MOC_INCLUDE("LocalSceneQuickView.h");

class ShowCallout : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::LocalSceneQuickView* localSceneView READ localSceneView WRITE setLocalSceneView NOTIFY localSceneViewChanged)

public:
  explicit ShowCallout(QObject* parent = nullptr);
  ~ShowCallout();

  static void init();

signals:
  void mapViewChanged();
  void sceneViewChanged();
  void localSceneViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);

  Esri::ArcGISRuntime::LocalSceneQuickView* localSceneView() const;
  void setLocalSceneView(Esri::ArcGISRuntime::LocalSceneQuickView* localSceneView);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::Scene* m_localScene = nullptr;

  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::LocalSceneQuickView* m_localSceneView = nullptr;

  QMetaObject::Connection m_mapClickConnection;
  QMetaObject::Connection m_sceneClickConnection;
  QMetaObject::Connection m_localSceneClickConnection;

  QMetaObject::Connection m_mapSceneLocationVisibilityConnection;
  QMetaObject::Connection m_sceneSceneLocationVisibilityConnection;
  QMetaObject::Connection m_localSceneSceneLocationVisibilityConnection;
};

#endif // SHOWCALLOUT_H
