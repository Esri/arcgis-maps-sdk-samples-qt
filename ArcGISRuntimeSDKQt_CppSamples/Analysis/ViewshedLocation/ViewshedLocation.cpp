// [WriteFile Name=ViewshedLocation, Category=Analysis]
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

#include "ViewshedLocation.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "LocationViewshed.h"
#include "Camera.h"
#include "Point.h"
#include "AnalysisOverlay.h"


using namespace Esri::ArcGISRuntime;

ViewshedLocation::ViewshedLocation(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void ViewshedLocation::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<ViewshedLocation>("Esri.Samples", 1, 0, "ViewshedLocationSample");
}

void ViewshedLocation::componentComplete()
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

  // connect signals
  connectSignals();
}

void ViewshedLocation::setInitialViewpoint()
{
  // Set a viewpoint
  double x = 6.86088;
  double y = 45.3604;
  double z = 3582.55;
  Point point(x, y, z, SpatialReference(4326));
  double heading = 345;
  double pitch = 70;
  double roll = 0;
  Camera camera(point, heading, pitch, roll);
  m_sceneView->setViewpointCamera(camera);
}

void ViewshedLocation::connectSignals()
{
  // on mouse click perform the location viewshed
  connect(m_sceneView, &SceneQuickView::mouseClicked, this, [this](QMouseEvent& event)
  {
    if (!m_locationViewshed)
      createViewshed(event.x(), event.y());
    else
    {
      const Point pt = m_sceneView->screenToBaseSurface(event.x(), event.y());
      m_locationViewshed->setLocation(pt);
    }
  });

  connect(m_sceneView, &SceneQuickView::mousePressedAndHeld, this, [this](QMouseEvent& event)
  {
    if (!m_locationViewshed)
      createViewshed(event.x(), event.y());

    m_calculating = true;
  });

  connect(m_sceneView, &SceneQuickView::mouseMoved, this, [this](QMouseEvent& event)
  {
    if (m_calculating)
    {
      const Point pt = m_sceneView->screenToBaseSurface(event.x(), event.y());
      m_locationViewshed->setLocation(pt);
    }
  });

  connect(m_sceneView, &SceneQuickView::mouseReleased, this, [this]
  {
    m_calculating = false;
  });
}

void ViewshedLocation::createViewshed(double x, double y)
{
  const Point pt = m_sceneView->screenToBaseSurface(x, y);

  // Create the Location Viewshed
  m_locationViewshed = new LocationViewshed(pt, m_heading, m_pitch,
                                            m_horizontalAngle, m_veriticalAngle,
                                            m_minDistance, m_maxDistance, this);
  m_locationViewshed->setVisible(m_viewshedVisible);

  // Add the Viewshed to the Analysis Overlay
  m_analysisOverlay->analyses()->append(m_locationViewshed);

  return;
}

// Getters/Setters for each Q_PROPERTY
bool ViewshedLocation::isViewshedVisible() const
{
  return m_viewshedVisible;
}

void ViewshedLocation::setViewshedVisible(bool viewshedVisible)
{
  if (m_locationViewshed)
  {
    if (m_locationViewshed->isVisible() == viewshedVisible)
      return;

    m_viewshedVisible = viewshedVisible;
    m_locationViewshed->setVisible(viewshedVisible);
  }
  else
  {
    if (m_viewshedVisible == viewshedVisible)
      return;

    m_viewshedVisible = viewshedVisible;
  }

  emit viewshedVisibleChanged();
}

bool ViewshedLocation::isFrustumOutlineVisible() const
{
  return m_locationViewshed ? m_locationViewshed->isFrustumOutlineVisible() : m_frustumVisible;
}

void ViewshedLocation::setFrustumOutlineVisible(bool frustumVisible)
{
  if (m_locationViewshed)
  {
    if (m_locationViewshed->isFrustumOutlineVisible() == frustumVisible)
      return;

    m_frustumVisible = frustumVisible;
    m_locationViewshed->setFrustumOutlineVisible(frustumVisible);
  }
  else
  {
    if (m_frustumVisible == frustumVisible)
      return;

    m_frustumVisible = frustumVisible;
  }

  emit frustumVisibleChanged();
}

double ViewshedLocation::minDistance() const
{
  return m_locationViewshed ? m_locationViewshed->minDistance() : m_minDistance;
}

void ViewshedLocation::setMinDistance(double minDistance)
{
  if (m_locationViewshed)
  {
    if (m_locationViewshed->minDistance() == minDistance)
      return;

    m_minDistance = minDistance;
    m_locationViewshed->setMinDistance(minDistance);
  }
  else
  {
    if (m_minDistance == minDistance)
      return;

    m_minDistance = minDistance;
  }

  emit minDistanceChanged();
}

double ViewshedLocation::maxDistance() const
{
  return m_locationViewshed ? m_locationViewshed->maxDistance() : m_maxDistance;
}

void ViewshedLocation::setMaxDistance(double maxDistance)
{
  m_maxDistance = maxDistance;

  if (m_locationViewshed)
  {
    if (m_locationViewshed->maxDistance() == maxDistance)
      return;

    m_maxDistance = maxDistance;
    m_locationViewshed->setMaxDistance(maxDistance);
  }
  else
  {
    if (m_maxDistance == maxDistance)
      return;

    m_maxDistance = maxDistance;
  }

  emit maxDistanceChanged();
}

double ViewshedLocation::horizontalAngle() const
{
  return m_locationViewshed ? m_locationViewshed->horizontalAngle() : m_horizontalAngle;
}

void ViewshedLocation::setHorizontalAngle(double horizontalAngle)
{
  if (m_locationViewshed)
  {
    if (m_locationViewshed->horizontalAngle() == horizontalAngle)
      return;

    m_horizontalAngle = horizontalAngle;
    m_locationViewshed->setHorizontalAngle(horizontalAngle);
  }
  else
  {
    if (m_horizontalAngle == horizontalAngle)
      return;

    m_horizontalAngle = horizontalAngle;
  }

  emit horizontalAngleChanged();
}

double ViewshedLocation::verticalAngle() const
{
  return m_locationViewshed ? m_locationViewshed->verticalAngle() : m_veriticalAngle;
}

void ViewshedLocation::setVerticalAngle(double verticalAngle)
{
  if (m_locationViewshed)
  {
    if (m_locationViewshed->verticalAngle() == verticalAngle)
      return;

    m_veriticalAngle = verticalAngle;
    m_locationViewshed->setVerticalAngle(verticalAngle);
  }
  else
  {
    if (m_veriticalAngle == verticalAngle)
      return;

    m_veriticalAngle = verticalAngle;
  }

  emit verticalAngleChanged();
}

double ViewshedLocation::heading() const
{
  return m_locationViewshed ? m_locationViewshed->heading() : m_heading;
}

void ViewshedLocation::setHeading(double heading)
{
  if (m_locationViewshed)
  {
    if (m_locationViewshed->heading() == heading)
      return;

    m_heading = heading;
    m_locationViewshed->setHeading(heading);
  }
  else
  {
    if (m_heading == heading)
      return;

    m_heading = heading;
  }

  emit headingChanged();
}

double ViewshedLocation::pitch() const
{
  return m_locationViewshed ? m_locationViewshed->pitch() : m_pitch;
}

void ViewshedLocation::setPitch(double pitch)
{
  if (m_locationViewshed)
  {
    if (m_locationViewshed->pitch() == pitch)
      return;

    m_pitch = pitch;
    m_locationViewshed->setPitch(pitch);
  }
  else
  {
    if (m_pitch == pitch)
      return;

    m_pitch = pitch;
  }

  emit pitchChanged();
}

QColor ViewshedLocation::visibleColor() const
{
  return LocationViewshed::visibleColor();
}

void ViewshedLocation::setVisibleColor(const QColor& visibleColor)
{
  if (LocationViewshed::visibleColor() == visibleColor)
    return;

  LocationViewshed::setVisibleColor(visibleColor);

  emit visibleColorChanged();
}

QColor ViewshedLocation::obstructedColor() const
{
  return LocationViewshed::obstructedColor();
}

void ViewshedLocation::setObstructedColor(const QColor& obstructedColor)
{
  if (LocationViewshed::obstructedColor() == obstructedColor)
    return;

  LocationViewshed::setObstructedColor(obstructedColor);

  emit obstructedColorChanged();
}
