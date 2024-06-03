// [WriteFile Name=ViewshedCamera, Category=Analysis]
// [Legal]
// Copyright 2017 Esri.

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

#include "ViewshedCamera.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "AnalysisOverlay.h"
#include "Point.h"
#include "Camera.h"
#include "Viewpoint.h"
#include "LocationViewshed.h"
#include "IntegratedMeshLayer.h"
#include "MapTypes.h"
#include "Surface.h"
#include "ElevationSourceListModel.h"
#include "LayerListModel.h"
#include "AnalysisOverlayListModel.h"
#include "AnalysisListModel.h"
#include "SpatialReference.h"

using namespace Esri::ArcGISRuntime;

ViewshedCamera::ViewshedCamera(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void ViewshedCamera::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<ViewshedCamera>("Esri.Samples", 1, 0, "ViewshedCameraSample");
}

void ViewshedCamera::componentComplete()
{
  QQuickItem::componentComplete();

  // Create a scene
  m_sceneView = findChild<SceneQuickView*>("sceneView");
  m_scene = new Scene(BasemapStyle::ArcGISImageryStandard, this);

  // Set a base surface
  Surface* surface = new Surface(this);
  surface->elevationSources()->append(
        new ArcGISTiledElevationSource(
          QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"),
          this));
  m_scene->setBaseSurface(surface);

  // Create and add the Girona, Spain integrated mesh layer
  IntegratedMeshLayer* gironaMeshLayer = new IntegratedMeshLayer(QUrl("https://tiles.arcgis.com/tiles/z2tnIkrLQ2BRzr6P/arcgis/rest/services/Girona_Spain/SceneServer"), this);
  m_scene->operationalLayers()->append(gironaMeshLayer);

  // Add an AnalysisOverlay to display the viewshed analysis result
  m_analysisOverlay = new AnalysisOverlay(this);
  m_sceneView->analysisOverlays()->append(m_analysisOverlay);

  // Set initial viewpoint
  setInitialViewpoint();

  // Add the Scene to the Sceneview
  m_sceneView->setArcGISScene(m_scene);
}

void ViewshedCamera::calculateViewshed()
{
  if (!m_viewshed)
  {
    // Create the viewshed
    const double minDistance = 1;
    const double maxDistance = 1000;
    m_viewshed = new LocationViewshed(m_sceneView->currentViewpointCamera(), minDistance, maxDistance, this);

    // display the frustum
    m_viewshed->setFrustumOutlineVisible(true);

    // Add the viewshed to the overlay
    m_analysisOverlay->analyses()->append(m_viewshed);
  }
  else
  {
    m_viewshed->updateFromCamera(m_sceneView->currentViewpointCamera());
  }
}

void ViewshedCamera::setInitialViewpoint()
{
  const double x_longitude = 2.82691;
  const double y_latitude = 41.985;
  const double z_altitude = 124.987;
  Point pt(x_longitude, y_latitude, z_altitude, SpatialReference(4326));

  const double heading = 332.131;
  const double pitch = 82.4732;
  const double roll = 0;
  Camera camera(pt, heading, pitch, roll);

  Viewpoint initViewpoint(pt, camera);
  m_scene->setInitialViewpoint(initViewpoint);
}
