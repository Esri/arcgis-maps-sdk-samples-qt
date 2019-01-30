// [WriteFile Name=AddIntegreatedMeshLayer, Category=Scenes]
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

#include "AddIntegreatedMeshLayer.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "IntegratedMeshLayer.h"

using namespace Esri::ArcGISRuntime;

AddIntegreatedMeshLayer::AddIntegreatedMeshLayer(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(Basemap::imagery(this), this))
{
  // create an elevation source from a REST service
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
        QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);

  // create the integrated mesh layer
  const QUrl meshLyrUrl("https://tiles.arcgis.com/tiles/FQD0rKU8X5sAQfh8/arcgis/rest/services/VRICON_Yosemite_Sample_Integrated_Mesh_scene_layer/SceneServer");
  IntegratedMeshLayer* integratedMeshLyr = new IntegratedMeshLayer(meshLyrUrl, this);

  // add the layer to the scene
  m_scene->operationalLayers()->append(integratedMeshLyr);

  // set the intial viewpoint of the scene
  const Point initialPt(-119.622075, 37.720650, 2104.901239, SpatialReference(4326));
  const Camera initialCamera(initialPt, initialPt.z(), 315.50368761552056, 78.09465920130114, 0);
  const Viewpoint initialViewpoint(initialPt, initialPt.z(), initialCamera);
  m_scene->setInitialViewpoint(initialViewpoint);

}

AddIntegreatedMeshLayer::~AddIntegreatedMeshLayer() = default;

void AddIntegreatedMeshLayer::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<AddIntegreatedMeshLayer>("Esri.Samples", 1, 0, "AddIntegreatedMeshLayerSample");
}

SceneQuickView* AddIntegreatedMeshLayer::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void AddIntegreatedMeshLayer::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
  {
    return;
  }

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  emit sceneViewChanged();
}
