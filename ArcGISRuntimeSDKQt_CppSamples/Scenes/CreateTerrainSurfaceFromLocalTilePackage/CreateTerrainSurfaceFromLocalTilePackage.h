// COPYRIGHT 2023 ESRI
// TRADE SECRETS: ESRI PROPRIETARY AND CONFIDENTIAL
// Unpublished material - all rights reserved under the
// Copyright Laws of the United States and applicable international
// laws, treaties, and conventions.
//
// For additional information, contact:
// Environmental Systems Research Institute, Inc.
// Attn: Contracts and Legal Services Department
// 380 New York Street
// Redlands, California, 92373
// USA
//
// email: contracts@esri.com
/// \file CreateTerrainSurfaceFromLocalTilePackage.h

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

#ifndef CreateTerrainSurfaceFromLocalTilePackage_H
#define CreateTerrainSurfaceFromLocalTilePackage_H

// C++ API headers
#include "Error.h"

namespace Esri::ArcGISRuntime
{
  class Scene;
  class SceneQuickView;
}

#include <QObject>

Q_MOC_INCLUDE("SceneQuickView.h")

class CreateTerrainSurfaceFromLocalTilePackage : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)

public:
  explicit CreateTerrainSurfaceFromLocalTilePackage(QObject* parent = nullptr);
  ~CreateTerrainSurfaceFromLocalTilePackage();

  static void init();

signals:
  void sceneViewChanged();

private slots:
  void elevationSrcFinishedLoading(const Esri::ArcGISRuntime::Error& loadError);

private:
  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
};

#endif // CreateTerrainSurfaceFromLocalTilePackage_H
