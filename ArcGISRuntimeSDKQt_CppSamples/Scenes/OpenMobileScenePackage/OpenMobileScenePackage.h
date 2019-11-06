// [WriteFile Name=OpenMobileScenePackage, Category=Scenes]
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

#ifndef OPENMOBILESCENEPACKAGE_H
#define OPENMOBILESCENEPACKAGE_H

namespace Esri
{
namespace ArcGISRuntime
{
class Error;
class Scene;
class SceneQuickView;
class MobileScenePackage;
}
}

#include <QObject>

class OpenMobileScenePackage : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)

public:
  explicit OpenMobileScenePackage(QObject* parent = nullptr);
  ~OpenMobileScenePackage();

  static void init();

private slots:
  void packageLoaded(const Esri::ArcGISRuntime::Error& e);

signals:
  void sceneViewChanged();

private:
  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);
  void createScenePackage(const QString& path);

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::MobileScenePackage* m_scenePackage = nullptr;   
};

#endif // OPENMOBILESCENEPACKAGE_H
