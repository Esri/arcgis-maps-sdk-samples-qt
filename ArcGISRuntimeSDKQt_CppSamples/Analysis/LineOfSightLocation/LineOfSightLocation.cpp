// [WriteFile Name=LineOfSightLocation, Category=Analysis]
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

#include "LineOfSightLocation.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "LocationLineOfSight.h"
#include "AnalysisOverlay.h"
#include "Camera.h"

using namespace Esri::ArcGISRuntime;

LineOfSightLocation::LineOfSightLocation(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void LineOfSightLocation::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<LineOfSightLocation>("Esri.Samples", 1, 0, "LineOfSightLocationSample");
}

void LineOfSightLocation::componentComplete()
{
  QQuickItem::componentComplete();

  // Create a scene and give it to the SceneView
  m_sceneView = findChild<SceneQuickView*>("sceneView");

  Scene* scene = new Scene(Basemap::topographic(this), this);
  Surface* surface = new Surface(this);
  surface->elevationSources()->append(
        new ArcGISTiledElevationSource(
          QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"),
          this));
  scene->setBaseSurface(surface);
  m_sceneView->setArcGISScene(scene);

  // Add an Analysis Overlay
  m_analysisOverlay = new AnalysisOverlay(this);
  m_sceneView->analysisOverlays()->append(m_analysisOverlay);

  // set initial viewpoint
  setInitialViewpoint();

  // create LocationLineOfSight
  createLineOfSight();

  // connect signals
  connectSignals();
}

void LineOfSightLocation::createLineOfSight()
{
  // create the observer/target points
  const Point observerPt(-73.06958032962375, -49.253112971555446, 2000, SpatialReference::wgs84());
  const Point targetPt(-73.079266999709162, -49.300457676730559, 1312, SpatialReference::wgs84());

  // create the line of sight
  m_lineOfSight = new LocationLineOfSight(observerPt, targetPt, this);
  m_analysisOverlay->analyses()->append(m_lineOfSight);

  // configure the LoS color and width
  LineOfSight::setVisibleColor(QColor("cyan"));
  LineOfSight::setObstructedColor(QColor("magenta"));
  LineOfSight::setLineWidth(2.0f);
}

void LineOfSightLocation::setInitialViewpoint()
{
  // Set a viewpoint
  Point point(-73.0815, -49.3272, 4059, SpatialReference::wgs84());
  const double heading = 11;
  const double pitch = 62;
  const double roll = 0;
  Camera camera(point, heading, pitch, roll);
  m_sceneView->setViewpointCamera(camera);
}

void LineOfSightLocation::connectSignals()
{
  // on mouse click perform the location viewshed
  connect(m_sceneView, &SceneQuickView::mouseClicked, this, [this](QMouseEvent& event)
  {
    const Point pt = m_sceneView->screenToBaseSurface(event.x(), event.y());
    m_lineOfSight->setTargetLocation(pt);
  });

  connect(m_sceneView, &SceneQuickView::mousePressedAndHeld, this, [this]
  {
    m_calculating = true;
  });

  connect(m_sceneView, &SceneQuickView::mouseMoved, this, [this](QMouseEvent& event)
  {
    if (m_calculating)
    {
      const Point pt = m_sceneView->screenToBaseSurface(event.x(), event.y());
      m_lineOfSight->setTargetLocation(pt);
    }
  });

  connect(m_sceneView, &SceneQuickView::mouseReleased, this, [this]
  {
    m_calculating = false;
  });
}
