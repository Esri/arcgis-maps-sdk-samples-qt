// [WriteFile Name=DisplayLocalSceneView, Category=Scenes]
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
#include "DisplayLocalSceneView.h"

// ArcGIS Maps SDK headers
#include "ArcGISTiledElevationSource.h"
#include "ArcGISSceneLayer.h"
#include "Camera.h"
#include "ElevationSourceListModel.h"
#include "Envelope.h"
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

DisplayLocalSceneView::DisplayLocalSceneView(QObject* parent /* = nullptr */) :
  QObject(parent),
  m_scene(new Scene(BasemapStyle::ArcGISTopographic, SceneViewingMode::Local, this))
{
  // create a new elevation source from Terrain3D REST service
  ArcGISTiledElevationSource* elevationSource =
    new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);

  ArcGISSceneLayer* sceneLayer = new ArcGISSceneLayer(QUrl("https://www.arcgis.com/home/item.html?id=61da8dc1a7bc4eea901c20ffb3f8b7af"), this);

  m_scene->operationalLayers()->append(sceneLayer);
}

DisplayLocalSceneView::~DisplayLocalSceneView() = default;

void DisplayLocalSceneView::init()
{
  // Register classes for QML
  qmlRegisterType<LocalSceneQuickView>("Esri.Samples", 1, 0, "LocalSceneView");
  qmlRegisterType<DisplayLocalSceneView>("Esri.Samples", 1, 0, "DisplayLocalSceneViewSample");
}

LocalSceneQuickView* DisplayLocalSceneView::localSceneView() const
{
  return m_localSceneView;
}

// Set the view (created in QML)
void DisplayLocalSceneView::setLocalSceneView(LocalSceneQuickView* localSceneView)
{
  if (!localSceneView || localSceneView == m_localSceneView)
  {
    return;
  }

  m_localSceneView = localSceneView;
  m_localSceneView->setArcGISScene(m_scene);

  // Sets the clipping area.
  const Envelope envelope(19454578.8235, // xMin
                          -5055381.4798, // yMin
                          19455518.8814, // xMax
                          -5054888.4150, // yMax
                          SpatialReference::webMercator()); // spatialReference
  m_scene->setClippingArea(envelope);

  m_scene->setClippingEnabled(true);

  // Sets the initial viewpoint.
  const Point lookAtPoint(19455578.6821, // x
                          -5056336.2227, // y
                          1699.3366, // z
                          SpatialReference::webMercator()); // spatialReference

  const Camera camera(lookAtPoint,
                      338.7410, // heading
                      40.3763, // pitch
                      0.0); // roll

  const Point point(19455026.8116, // x
                    -5054995.7415, // y
                    SpatialReference::webMercator()); // spatialReference

  m_scene->setInitialViewpoint(Viewpoint(point,
                                         8314.6991, // scale
                                         camera));

  emit localSceneViewChanged();
}
