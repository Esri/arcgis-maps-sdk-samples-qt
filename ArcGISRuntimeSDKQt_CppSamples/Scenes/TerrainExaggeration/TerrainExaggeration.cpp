// [WriteFile Name=TerrainExaggeration, Category=Scenes]
// [Legal]
// Copyright 2018 Esri.

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

#include "TerrainExaggeration.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "Camera.h"
#include "Point.h"
#include "SceneQuickView.h"

using namespace Esri::ArcGISRuntime;

TerrainExaggeration::TerrainExaggeration(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void TerrainExaggeration::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<TerrainExaggeration>("Esri.Samples", 1, 0, "TerrainExaggerationSample");
}

void TerrainExaggeration::componentComplete()
{
  QQuickItem::componentComplete();

  // Create a scene and give it to the SceneView
  m_sceneView = findChild<SceneQuickView*>("sceneView");
  Scene* scene = new Scene(Basemap::nationalGeographic(this), this);
  m_surface = new Surface(this);
  m_surface->elevationSources()->append(
        new ArcGISTiledElevationSource(
          QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"),
          this));

  // Create the camera object at our initial viewpoint
  const Point initialPoint(-119.9616962169934, 46.7000413426849, 3183, SpatialReference(4326));
  const Camera initialViewpointCamera(initialPoint, 0, 7, 70, 0);

  // Set the initial ViewpointCamera for the scene
  m_sceneView->setViewpointCamera(initialViewpointCamera);

  // Initialize the sceneview by applying the surface
  scene->setBaseSurface(m_surface);
  m_sceneView->setArcGISScene(scene);
}

void TerrainExaggeration::setElevationExaggeration(double factor)
{
  // If the surface exists, trigger a change in elevation exaggeration by [factor] amount
  if (m_surface)
    m_surface->setElevationExaggeration(factor);
}
