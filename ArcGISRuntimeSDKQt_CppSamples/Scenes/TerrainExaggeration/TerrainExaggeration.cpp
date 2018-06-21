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

#include "TerrainExaggeration.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
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
          QUrl("http://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"),
          this));
  // Create a viewpoint where the camera will be
  Viewpoint initialViewpoint(Point(46.75792111605992, -119.94891542688772, 0, SpatialReference(4326)));
  // Create the camera object at our initial viewpoint
  Camera initialViewpointCamera(46.75792111605992, -119.94891542688772, 15000, 60, 40, 0);

  // Set the initial camera/viewpoint combination for the scene
  m_sceneView->setViewpoint(initialViewpoint);
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
