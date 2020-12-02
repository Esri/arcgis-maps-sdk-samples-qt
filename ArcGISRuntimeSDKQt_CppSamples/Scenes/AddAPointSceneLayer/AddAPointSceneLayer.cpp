// [WriteFile Name=AddAPointSceneLayer, Category=Scenes]
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

#include "AddAPointSceneLayer.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "ArcGISSceneLayer.h"

using namespace Esri::ArcGISRuntime;

AddAPointSceneLayer::AddAPointSceneLayer(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(Basemap::imagery(this), this))
{
  // create a new elevation source from Terrain3D REST service
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
        QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);

  // create a new point scene layer from world airport REST service
  ArcGISSceneLayer* pointSceneLayer = new ArcGISSceneLayer(
        QUrl("https://tiles.arcgis.com/tiles/V6ZHFr6zdgNZuVG0/arcgis/rest/services/Airports_PointSceneLayer/SceneServer/layers/0"), this);

  // add scene layer source to the scene to display points at world airport locations
  m_scene->operationalLayers()->append(pointSceneLayer);

}

AddAPointSceneLayer::~AddAPointSceneLayer() = default;

void AddAPointSceneLayer::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<AddAPointSceneLayer>("Esri.Samples", 1, 0, "AddAPointSceneLayerSample");
}

SceneQuickView* AddAPointSceneLayer::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void AddAPointSceneLayer::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
    return;

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  emit sceneViewChanged();
}

