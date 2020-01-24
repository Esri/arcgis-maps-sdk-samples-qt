// [WriteFile Name=DisplayScenesInTabletopAR, Category=AR]
// [Legal]
// Copyright 2020 Esri.

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

#ifndef DISPLAYSCENESINTABLETOPAR_H
#define DISPLAYSCENESINTABLETOPAR_H

namespace Esri
{
namespace ArcGISRuntime
{
class ArcGISSceneLayer;
class Error;
class MobileScenePackage;
class Scene;
class SceneQuickView;
}
}

#include <QObject>
#include "ArcGISArView.h"
#include "PermissionsHelper.h"

class DisplayScenesInTabletopAR : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::Toolkit::ArcGISArView* arcGISArView READ arcGISArView WRITE setArcGISArView NOTIFY arcGISArViewChanged)
  Q_PROPERTY(bool dialogVisible MEMBER m_dialogVisible NOTIFY dialogVisibleChanged)

public:
  explicit DisplayScenesInTabletopAR(QObject* parent = nullptr);
  ~DisplayScenesInTabletopAR();

  static void init();

private slots:
  void onMouseClicked(QMouseEvent& event);
  void packageLoaded(const Esri::ArcGISRuntime::Error& e);

signals:
  void sceneViewChanged();
  void arcGISArViewChanged();
  void dialogVisibleChanged();

private:
  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);
  void createScenePackage(const QString& path);

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::MobileScenePackage* m_scenePackage = nullptr;
  Esri::ArcGISRuntime::Camera m_originCamera;
  PermissionsHelper* m_permissionsHelper = nullptr;

  Esri::ArcGISRuntime::Toolkit::ArcGISArView* arcGISArView() const;
  void setArcGISArView(Esri::ArcGISRuntime::Toolkit::ArcGISArView* arcGISArView);

  Esri::ArcGISRuntime::Toolkit::ArcGISArView* m_arcGISArView = nullptr;
  bool m_dialogVisible = true;

  // The width of the scene content is about 800 meters.
  const double m_sceneWidth = 800;

  // The desired physical width of the scene is 1 meter.
  const double m_tableTopWidth = 1;
};

#endif // DISPLAYSCENESINTABLETOPAR_H
