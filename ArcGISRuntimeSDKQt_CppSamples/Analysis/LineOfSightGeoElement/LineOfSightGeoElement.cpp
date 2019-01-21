// [WriteFile Name=LineOfSightGeoElement, Category=Analysis]
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

#include "LineOfSightGeoElement.h"

#include "ArcGISSceneLayer.h"
#include "ArcGISTiledElevationSource.h"
#include "GeoElementLineOfSight.h"
#include "GeometryEngine.h"
#include "GraphicsOverlay.h"
#include "ModelSceneSymbol.h"
#include "PointBuilder.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleRenderer.h"

#include <array>

#include <QStandardPaths>
#include <QDir>

using namespace Esri::ArcGISRuntime;

namespace
{

// Initial fixed point to observe taxi.
const Point observationPoint(-73.9853, 40.7484, 200, SpatialReference::wgs84());

// Waypoints around the block for taxi to drive.
const std::array<Point, 4> waypoints = {{
                                          { -73.984513, 40.748469, 2, SpatialReference::wgs84() },
                                          { -73.985068, 40.747786, 2, SpatialReference::wgs84() },
                                          { -73.983452, 40.747091, 2, SpatialReference::wgs84() },
                                          { -73.982961, 40.747762, 2, SpatialReference::wgs84() }
                                        }};
}

LineOfSightGeoElement::LineOfSightGeoElement(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(Basemap::imagery(this), this))
{
  // create a new elevation source from Terrain3D rest service
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
        QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);

  // Load up the buildings that will block the line of sight.
  ArcGISSceneLayer* buildings = new ArcGISSceneLayer(
        QUrl("https://tiles.arcgis.com/tiles/z2tnIkrLQ2BRzr6P/arcgis/rest/services/New_York_LoD2_3D_Buildings/SceneServer/layers/0"));
  m_scene->operationalLayers()->append(buildings);

  // Trigger animation of taxi every 100ms.
  m_animation.setInterval(100);
  m_animation.callOnTimeout(this, &LineOfSightGeoElement::animate);
}

LineOfSightGeoElement::~LineOfSightGeoElement() = default;

void LineOfSightGeoElement::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<LineOfSightGeoElement>("Esri.Samples", 1, 0, "LineOfSightGeoElementSample");
}

double LineOfSightGeoElement::heightZ() const
{
  if (m_observer)
  {
    const Point p = geometry_cast<Point>(m_observer->geometry());
    return p.isValid() ? p.z() : 0.0;
  }
  else
  {
    return 0.0;
  }
}
void LineOfSightGeoElement::setHeightZ(double z)
{
  if (!m_observer)
    return;

  const Point p = geometry_cast<Point>(m_observer->geometry());
  if (p.isValid())
  {
    PointBuilder builder(p);
    builder.setZ(z);
    m_observer->setGeometry(builder.toGeometry());
    emit heightZChanged();
  }
}

SceneQuickView* LineOfSightGeoElement::sceneView() const
{
  return m_sceneView;
}

void LineOfSightGeoElement::setDataPath(QString dataPath)
{
  if (m_dataPath == dataPath)
    return;

  m_dataPath = std::move(dataPath);
  emit dataPathChanged();

  if (!m_dataPath.isEmpty() && m_sceneView)
    initialize();
}

// Set the view (created in QML)
void LineOfSightGeoElement::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
  {
    return;
  }

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);
  emit sceneViewChanged();

  if (!m_dataPath.isEmpty() && m_sceneView)
    initialize();
}

void LineOfSightGeoElement::initialize()
{
  // Setup the graphics overlay - ensure that all z-position are relative to the height of the surface.
  GraphicsOverlay* graphicsOverlay = new GraphicsOverlay(this);
  {
    LayerSceneProperties properties = graphicsOverlay->sceneProperties();
    properties.setSurfacePlacement(SurfacePlacement::Relative);
    graphicsOverlay->setSceneProperties(properties);
  }
  m_sceneView->graphicsOverlays()->append(graphicsOverlay);

  // Set up the renderer so that we can orient the taxi using the `HEADING` attribute.
  SimpleRenderer* renderer3D = new SimpleRenderer(this);
  {
    RendererSceneProperties properties = renderer3D->sceneProperties();
    properties.setHeadingExpression("[HEADING]");
    renderer3D->setSceneProperties(properties);
  }
  graphicsOverlay->setRenderer(renderer3D);

  // Create our observation point as a red sphere.
  m_observer = new Graphic(observationPoint, new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, Qt::red, 5.f), this);
  emit heightZChanged(); // We now have a point to extract the Z-height from.
  graphicsOverlay->graphics()->append(m_observer);

  // Get our Taxi model. We will attempt to load it and continue our setup after it has loaded.
  QUrl modelPath = QUrl(QDir(m_dataPath).filePath("ArcGIS/Runtime/Data/3D/dolmus_3ds/dolmus.3ds")).toLocalFile();
  ModelSceneSymbol* taxiSymbol = new ModelSceneSymbol(modelPath, 1.0, this);
  taxiSymbol->setAnchorPosition(SceneSymbolAnchorPosition::Bottom);

  connect(taxiSymbol, &ModelSceneSymbol::doneLoading, this,
          [this, graphicsOverlay, taxiSymbol](Error error)
  {
    if (!error.isEmpty())
    {
      return;
    }

    // Create taxi from loaded taxi symbol, with an initial "HEADING" attribute for orientation.
    m_taxi = new Graphic(waypoints.front(), taxiSymbol, this);
    m_taxi->attributes()->insertAttribute("HEADING", 0.0);
    graphicsOverlay->graphics()->append(m_taxi);

    // Set up our line of sight analysis.
    AnalysisOverlay*  analysisOverlay = new AnalysisOverlay(this);
    m_sceneView->analysisOverlays()->append(analysisOverlay);

    GeoElementLineOfSight* lineOfSight = new GeoElementLineOfSight(m_observer, m_taxi, this);
    analysisOverlay->analyses()->append(lineOfSight);

    connect(lineOfSight, &GeoElementLineOfSight::targetVisibilityChanged, this,
            [this](LineOfSightTargetVisibility targetVisibility)
    {
      // Select taxi whenever there is a clear line of sight from observer position.
      m_taxi->setSelected(targetVisibility == LineOfSightTargetVisibility::Visible);
    });

    Camera camera(observationPoint, 700, -30, 45, 0);
    m_sceneView->setViewpointCamera(camera, 0);

    m_animation.start();
  });

  taxiSymbol->load();
}

void LineOfSightGeoElement::animate()
{
  // Goal point to travel to
  Point waypoint = waypoints.at(m_waypointIndex);

  // Calculate azimuth between current location and goal location for taxi.
  Point location = geometry_cast<Point>(m_taxi->geometry());

  if (!location.isValid())
    return;

  GeodeticDistanceResult distance = GeometryEngine::distanceGeodetic(
        location, waypoint, LinearUnit::meters(), AngularUnit::degrees(), GeodeticCurveType::Geodesic);

  // Move taxi one metre along the line between its current position and the goal location.
  QList<Point> newPoints = GeometryEngine::moveGeodetic(
  { location }, 1.0, LinearUnit::meters(), distance.azimuth1(), AngularUnit::degrees(), GeodeticCurveType::Geodesic);
  if (newPoints.size() > 0)
  {
    location = newPoints.last();
  }

  // Update taxi position and orientation.
  m_taxi->setGeometry(geometry_cast<Geometry>(location));
  m_taxi->attributes()->replaceAttribute("HEADING", distance.azimuth1());

  // When taxi is close enough to the waypoint then increment the index for a new goal next animation step.
  // Index is cyclic and an element of [0, 3].
  if (distance.distance() <= 2) {
    m_waypointIndex = (m_waypointIndex + 1) % waypoints.size();
  }
}
