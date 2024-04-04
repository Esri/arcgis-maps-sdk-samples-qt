// [WriteFile Name=Add3DTilesLayer, Category=Scenes]
// [Legal]
// Copyright 2023 Esri.

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

#include "Add3DTilesLayer.h"

#include "ArcGISTiledElevationSource.h"
#include "Basemap.h"
#include "Camera.h"
#include "ElevationSourceListModel.h"
#include "Layer.h"
#include "LayerListModel.h"
#include "MapTypes.h"
#include "Ogc3dTilesLayer.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "Surface.h"

using namespace Esri::ArcGISRuntime;

Add3DTilesLayer::Add3DTilesLayer(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(BasemapStyle::ArcGISDarkGray, this))
{
  // create a new elevation source from Terrain3D REST service
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
        QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);

  add3DTilesLayer();
}

Add3DTilesLayer::~Add3DTilesLayer() = default;

void Add3DTilesLayer::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<Add3DTilesLayer>("Esri.Samples", 1, 0, "Add3DTilesLayerSample");
}

SceneQuickView* Add3DTilesLayer::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void Add3DTilesLayer::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
    return;

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  emit sceneViewChanged();

  setInitialViewpoint();
}

void Add3DTilesLayer::add3DTilesLayer()
{
  const QUrl modelPath = QUrl("https://tiles.arcgis.com/tiles/N82JbI5EYtAkuUKU/arcgis/rest/services/Stuttgart/3DTilesServer/tileset.json");
  m_ogc3dTilesLayer = new Ogc3dTilesLayer(modelPath, this);
  m_scene->operationalLayers()->append(m_ogc3dTilesLayer);
}

void Add3DTilesLayer::setInitialViewpoint()
{
  // add a camera
  constexpr double latitude = 48.8418;
  constexpr double longitude = 9.1536;
  constexpr double altitude = 1325.0;
  constexpr double heading = 48.3497;
  constexpr double pitch = 57.8414;
  constexpr double roll = 0.0;
  const Camera sceneCamera(latitude, longitude, altitude, heading, pitch, roll);
  m_sceneView->setViewpointCameraAndWait(sceneCamera);
}
