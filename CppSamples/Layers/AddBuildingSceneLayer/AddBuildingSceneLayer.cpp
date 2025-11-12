// [WriteFile Name=AddBuildingSceneLayer, Category=Layers]
// [Legal]
// Copyright 2025 Esri.
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "AddBuildingSceneLayer.h"

// ArcGIS Maps SDK headers
#include "ArcGISTiledElevationSource.h"
#include "BuildingSceneLayer.h"
#include "BuildingSublayer.h"
#include "BuildingSublayerListModel.h"
#include "Camera.h"
#include "ElevationSourceListModel.h"
#include "Error.h"
#include "LayerListModel.h"
#include "LocalSceneQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "Scene.h"
#include "SceneViewTypes.h"
#include "SpatialReference.h"
#include "Surface.h"
#include "Viewpoint.h"

using namespace Esri::ArcGISRuntime;

AddBuildingSceneLayer::AddBuildingSceneLayer(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(BasemapStyle::ArcGISTopographic, SceneViewingMode::Local, this))
{
  // create a new elevation source from Terrain3D REST service
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
    QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);
}

AddBuildingSceneLayer::~AddBuildingSceneLayer() = default;

void AddBuildingSceneLayer::init()
{
  // Register classes for QML
  qmlRegisterType<LocalSceneQuickView>("Esri.Samples", 1, 0, "LocalSceneView");
  qmlRegisterType<AddBuildingSceneLayer>("Esri.Samples", 1, 0, "AddBuildingSceneLayerSample");
}

LocalSceneQuickView* AddBuildingSceneLayer::localSceneView() const
{
  return m_localSceneView;
}

// Set the view (created in QML)
void AddBuildingSceneLayer::setLocalSceneView(LocalSceneQuickView* localSceneView)
{
  if (!localSceneView || localSceneView == m_localSceneView)
  {
    return;
  }

  m_localSceneView = localSceneView;
  m_localSceneView->setArcGISScene(m_scene);

  // Sets the initial viewpoint.
  const Point point(
    -13045109.0, // x
    4036614.0, // y
    511.0, // z
    SpatialReference::webMercator()); // spatialReference

  const Camera camera(point,
    343.0, // heading
    68.0, // pitch
    0.0); // roll

  m_scene->setInitialViewpoint(Viewpoint(point, camera));

  // Adds building scene layer to scene.
  m_buildingSceneLayer = new BuildingSceneLayer(
    QUrl("https://www.arcgis.com/home/item.html?id=669f6279c579486eb4a0acc7eb59d7ca"), this);

  m_buildingSceneLayer->setAltitudeOffset(1.0);
  m_scene->operationalLayers()->append(m_buildingSceneLayer);

  connect(m_buildingSceneLayer, &BuildingSceneLayer::doneLoading, this, [this](const Error& e)
  {
    if (!e.isEmpty())
    {
      return;
    }

    // Get the overview and full model sublayers.
    for (BuildingSublayer* sublayer : (*m_buildingSceneLayer->sublayers()))
    {
      if (sublayer->name() == "Overview")
      {
        m_overviewSublayer = sublayer;
      }
      else if (sublayer->name() == "Full Model")
      {
        m_fullModelSublayer = sublayer;
      }
    }
    emit layerLoadedChanged();
  });
  emit localSceneViewChanged();
}

void AddBuildingSceneLayer::setFullModelAndOverviewVisibility(bool showFullModel)
{
  m_fullModelSublayer->setVisible(showFullModel);
  m_overviewSublayer->setVisible(!showFullModel);
}

bool AddBuildingSceneLayer::layerLoaded() const
{
  return m_buildingSceneLayer && m_buildingSceneLayer->loadStatus() == LoadStatus::Loaded;
}
