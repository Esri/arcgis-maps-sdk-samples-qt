// [WriteFile Name=DistanceMeasurementAnalysis, Category=Analysis]
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

#include "DistanceMeasurementAnalysis.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "AnalysisOverlay.h"
#include "LocationDistanceMeasurement.h"
#include "Viewpoint.h"
#include "Camera.h"
#include "ArcGISSceneLayer.h"
#include "Point.h"

using namespace Esri::ArcGISRuntime;

DistanceMeasurementAnalysis::DistanceMeasurementAnalysis(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void DistanceMeasurementAnalysis::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<DistanceMeasurementAnalysis>("Esri.Samples", 1, 0, "DistanceMeasurementAnalysisSample");
}

void DistanceMeasurementAnalysis::componentComplete()
{
  QQuickItem::componentComplete();

  // Get the Scene View
  m_sceneView = findChild<SceneQuickView*>("sceneView");

  // Create a Scene with the imagery basemap
  Scene* scene = new Scene(Basemap::imagery(this), this);

  // Add a Scene Layer
  ArcGISSceneLayer* sceneLayer = new ArcGISSceneLayer(QUrl("https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/Buildings_Brest/SceneServer/layers/0"), this);
  scene->operationalLayers()->append(sceneLayer);

  // Create a list of elevation sources
  const QList<ElevationSource*> sources
  {
    new ArcGISTiledElevationSource(
          QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"),
          this),
        new ArcGISTiledElevationSource(
          QUrl("https://tiles.arcgis.com/tiles/d3voDfTFbHOCRwVR/arcgis/rest/services/MNT_IDF/ImageServer"),
          this)
  };

  // Create and set the surface on the scene
  Surface* surface = new Surface(sources, this);
  scene->setBaseSurface(surface);

  // Add Analysis Overlay
  AnalysisOverlay* analysisOverlay = new AnalysisOverlay(this);
  m_sceneView->analysisOverlays()->append(analysisOverlay);

  // Create and add the LocationDistanceMeasurement
  const Point startLocation(-4.494677, 48.384472, 24.772694, SpatialReference::wgs84());
  const Point endLocation(-4.495646, 48.384377, 58.501115, SpatialReference::wgs84());
  m_distanceAnalysis = new LocationDistanceMeasurement(startLocation, endLocation, this);
  m_distanceAnalysis->setUnitSystem(UnitSystem::Metric);
  analysisOverlay->analyses()->append(m_distanceAnalysis);

  // Set initial viewpoint
  constexpr double distance = 400.0;
  constexpr double pitch = 45.0;
  constexpr double heading = 0.0;
  constexpr double roll = 0.0;
  const Camera initCamera(startLocation, distance, heading, pitch, roll);
  const Viewpoint initViewpoint(startLocation, distance, initCamera);
  scene->setInitialViewpoint(initViewpoint);

  // Set the scene on the scene view
  m_sceneView->setArcGISScene(scene);

  connectSignals();
}

void DistanceMeasurementAnalysis::connectSignals()
{
  // connect to signal to obtain updated distances
  connect(m_distanceAnalysis, &LocationDistanceMeasurement::measurementChanged, this, [this](const Distance& directDistance,
                                                                                             const Distance& horizontalDistance,
                                                                                             const Distance& verticalDistance)
  {
    const QString unitLabel = m_distanceAnalysis->unitSystem() == UnitSystem::Metric ? "m" : "ft";
    m_directDistance = QString::number(directDistance.value(), 'f', 2) + QString(" %1").arg(unitLabel);
    m_horizontalDistance = QString::number(horizontalDistance.value(), 'f', 2) + QString(" %1").arg(unitLabel);
    m_verticalDistance = QString::number(verticalDistance.value(), 'f', 2) + QString(" %1").arg(unitLabel);
    emit directDistanceChanged();
    emit horizontalDistanceChanged();
    emit verticalDistanceChanged();
  });

  // connect to mouse signals to update the analysis

  // When the mouse is pressed and held, start updating the distance analysis end point
  connect(m_sceneView, &SceneQuickView::mousePressedAndHeld, this, [this](QMouseEvent& mouseEvent)
  {
    m_isPressAndHold = true;
    m_sceneView->screenToLocation(mouseEvent.x(), mouseEvent.y());
  });

  // When the mouse is released...
  connect(m_sceneView, &SceneQuickView::mouseReleased, this, [this](QMouseEvent& mouseEvent)
  {
    // Check if the mouse was released from a pan gesture
    if (m_isNavigating)
    {
      m_isNavigating = false;
      return;
    }

    // Ignore if Right click
    if (mouseEvent.button() == Qt::RightButton)
      return;

    // If pressing and holding, do nothing
    if (m_isPressAndHold)
      m_isPressAndHold = false;
    // Else get the location from the screen coordinates
    else
      m_sceneView->screenToLocation(mouseEvent.x(), mouseEvent.y());
  });

  // Update the distance analysis when the mouse moves if it is a press and hold movement
  connect(m_sceneView, &SceneQuickView::mouseMoved, this, [this](QMouseEvent& mouseEvent)
  {
    if (m_isPressAndHold)
      m_sceneView->screenToLocation(mouseEvent.x(), mouseEvent.y());
  });

  // Set a flag when mousePressed signal emits
  connect(m_sceneView, &SceneQuickView::mousePressed, this, [this]
  {
    m_isNavigating = false;
  });

  // When screenToLocation completes...
  connect(m_sceneView, &SceneQuickView::screenToLocationCompleted, this, [this](QUuid, Point pt)
  {
    // If it was from a press and hold, update the end location
    if (m_isPressAndHold)
      m_distanceAnalysis->setEndLocation(pt);
    // Else if it was a normal mouse click (press and release), update the start location
    else
      m_distanceAnalysis->setStartLocation(pt);
  });

  // Set a flag when viewpointChanged signal emits
  connect(m_sceneView, &SceneQuickView::viewpointChanged, this, [this]
  {
    m_isNavigating = true;
  });
}

void DistanceMeasurementAnalysis::setUnits(const QString& unitName)
{
  if (!m_distanceAnalysis)
    return;

  if (unitName == "Metric")
    m_distanceAnalysis->setUnitSystem(UnitSystem::Metric);
  else
    m_distanceAnalysis->setUnitSystem(UnitSystem::Imperial);
}

QString DistanceMeasurementAnalysis::directDistance() const
{
  return m_directDistance;
}

QString DistanceMeasurementAnalysis::horizontalDistance() const
{
  return m_horizontalDistance;
}

QString DistanceMeasurementAnalysis::verticalDistance() const
{
  return m_verticalDistance;
}
