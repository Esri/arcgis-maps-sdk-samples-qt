// [WriteFile Name=GetElevationAtPoint, Category=Scenes]
// [Legal]
// Copyright 2019 Esri.

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

#include "GetElevationAtPoint.h"

#include "ArcGISTiledElevationSource.h"
#include "GraphicsOverlay.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "SimpleMarkerSymbol.h"
#include "Surface.h"

using namespace Esri::ArcGISRuntime;

GetElevationAtPoint::GetElevationAtPoint(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(Basemap::imagery(this), this)),
  m_graphicsOverlay(new GraphicsOverlay(this)),
  m_elevationMarker(new Graphic(Geometry(), new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("red"), 12, this), this))
{
  // create a new elevation source from Terrain3D REST service
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
        QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);

  // Set the marker to be invisible initially, will be flaggd visible when user interacts with scene for the first time, to visualise clicked position
  m_elevationMarker->setVisible(false);

  // Add the marker to the graphics overlay so it will be displayed. Graphics overlay is attached to the sceneView in ::setSceneView()
  m_graphicsOverlay->graphics()->append(m_elevationMarker);
}

GetElevationAtPoint::~GetElevationAtPoint() = default;

void GetElevationAtPoint::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<GetElevationAtPoint>("Esri.Samples", 1, 0, "GetElevationAtPointSample");
}

SceneQuickView* GetElevationAtPoint::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void GetElevationAtPoint::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
  {
    return;
  }

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  // Create a camera, looking at the Himalayan mountain range.
  constexpr double latitude = 28.4;
  constexpr double longitude = 83.9;
  constexpr double altitude = 10000.0;
  constexpr double heading = 10.0;
  constexpr double pitch = 80.0;
  constexpr double roll = 0.0;
  Camera camera(latitude, longitude, altitude, heading, pitch, roll);

  // Set the sceneview to use above camera, waits for load so scene is immediately displayed in appropriate place.
  m_sceneView->setViewpointCameraAndWait(camera);

  // Append the graphics overlays to the sceneview, so we can visualise elevation on click
  m_sceneView->graphicsOverlays()->append(m_graphicsOverlay);

  // Hook up clicks into the 3d scene to below behaviour that displays marker & elevation value.
  connect(sceneView, &SceneQuickView::mouseClicked, this, &GetElevationAtPoint::displayElevationOnClick);

  emit sceneViewChanged();
}

void GetElevationAtPoint::displayElevationOnClick(QMouseEvent& mouseEvent)
{
  // Convert clicked screen position to position on the map surface.
  const Point baseSurfacePos = m_sceneView->screenToBaseSurface(mouseEvent.x(), mouseEvent.y());

  // Connect to callback for elevation query, which places marker and sets elevation
  connect(m_scene->baseSurface(), &Surface::locationToElevationCompleted,
          this, [baseSurfacePos, this](QUuid /*taskId*/, double elevation)
  {
    // Place the elevation marker circle at the clicked position
    m_elevationMarker->setGeometry(baseSurfacePos);
    m_elevationMarker->setVisible(true);

    // Assign the elevation value. UI is bound to this value, so it updates to display new elevation.
    m_elevation = elevation;

    // Notify of property changes
    emit elevationChanged(elevation);
    emit elevationQueryRunningChanged();
  });

  //Invoke get elevation query
  m_elevationQueryTaskWatcher = m_scene->baseSurface()->locationToElevation(baseSurfacePos);
  emit elevationQueryRunningChanged();
}

double GetElevationAtPoint::elevation() const
{
  return m_elevation;
}

bool GetElevationAtPoint::elevationQueryRunning() const
{
  return !(m_elevationQueryTaskWatcher.isDone() || m_elevationQueryTaskWatcher.isCanceled());
}
