// [WriteFile Name=NavigateRoute, Category=Analysis]
// [Legal]
// Copyright 2020 Esri.

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

#include "NavigateRoute.h"

#include "DefaultLocationDataSource.h"
//#include "DestinationStatus
#include "GeometryEngine.h"
#include "GraphicsOverlay.h"
#include "Location.h"
#include "LocationDisplay.h"
#include "Map.h"
#include "MapQuickView.h"
#include "NavigationTypes.h"
#include "Point.h"
#include "Route.h"
#include "RouteResult.h"
#include "RouteTask.h"
#include "RouteTracker.h"
#include "SimpleMarkerSymbol.h"
#include "SimulatedLocationDataSource.h"
#include "Stop.h"
#include "TrackingDistance.h"
#include "TrackingProgress.h"
#include "TrackingStatus.h"
#include "VoiceGuidance.h"

#include <QList>
#include <QGeoPositionInfo>
#include <QUrl>

using namespace Esri::ArcGISRuntime;

namespace  {
const QUrl routeTaskUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/NetworkAnalysis/SanDiego/NAServer/Route");
const Point conventionCenterPoint(-117.160386727, 32.706608, SpatialReference::wgs84());
const Point memorialPoint(-117.173034, 32.712327, SpatialReference::wgs84());
const Point aerospaceMuseumPoint(-117.147230, 32.730467, SpatialReference::wgs84());
}

NavigateRoute::NavigateRoute(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::navigationVector(this), this))
{
}

NavigateRoute::~NavigateRoute() = default;

void NavigateRoute::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<NavigateRoute>("Esri.Samples", 1, 0, "NavigateRouteSample");
}

MapQuickView* NavigateRoute::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void NavigateRoute::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  m_routeOverlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(m_routeOverlay);

  m_routeTask = new RouteTask(routeTaskUrl, this);

  connect(m_routeTask, &RouteTask::solveRouteCompleted, this, [this](QUuid, RouteResult routeResult)
  {
    if (routeResult.isEmpty())
      return;
    m_routeResult = routeResult;
    m_route = m_routeResult.routes()[0];

    m_mapView->setViewpointGeometry(m_route.routeGeometry(), 100);

    // create a graphic to show the route
    Graphic* fullRouteGraphic = new Graphic(m_route.routeGeometry(), new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, Qt::blue, 5, this), this);

    // Create a graphic to represent the route that's been traveled (initially empty).
    Graphic* routeTraveledGraphic = new Graphic(this);
    routeTraveledGraphic->setSymbol(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::cyan, 3, this));
    m_routeOverlay->graphics()->append(fullRouteGraphic);
    m_routeOverlay->graphics()->append(routeTraveledGraphic);

    m_navigationButtonEnabled = true;
    emit navigationButtonChanged();
  });

  connect(m_routeTask, &RouteTask::createDefaultParametersCompleted, this, [this](QUuid, RouteParameters defaultParameters)
  {
    // set values for parameters
    defaultParameters.setReturnStops(true);
    defaultParameters.setReturnDirections(true);
    defaultParameters.setReturnRoutes(true);
    defaultParameters.setOutputSpatialReference(SpatialReference::wgs84());

    Stop stop1(conventionCenterPoint);
    Stop stop2(memorialPoint);
    Stop stop3(aerospaceMuseumPoint);

    QList<Stop> stopsList = {stop1, stop2, stop3};
    defaultParameters.setStops(stopsList);

    m_routeTask->solveRoute(defaultParameters);
  });

  connect(m_routeTask, &RouteTask::doneLoading, this, [this](Error error)
  {
    if (error.isEmpty())
    {
      m_routeTask->createDefaultParameters();
    }
    else
    {
     qDebug() << error.message() << error.additionalMessage();
    }
  });

  m_routeTask->load();

  // add graphics for the predefined stops
  SimpleMarkerSymbol* stopSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, Qt::red, 20, this);
  m_routeOverlay->graphics()->append(new Graphic(conventionCenterPoint, stopSymbol, this));
  m_routeOverlay->graphics()->append(new Graphic(memorialPoint, stopSymbol, this));
  m_routeOverlay->graphics()->append(new Graphic(aerospaceMuseumPoint, stopSymbol, this));

  emit mapViewChanged();
}

bool NavigateRoute::navigationButtonEnabled() const
{
  return m_navigationButtonEnabled;
}

void NavigateRoute::startNavigation()
{
  // disable the navigation button
  m_navigationButtonEnabled = false;
  emit navigationButtonChanged();

  // get the directions for the route
  m_directions = m_route.directionManeuvers(this);

  // create a route tracker
  m_routeTracker = new RouteTracker(m_routeResult, 0, this);
  //m_routeTracker->enableRerouting(m_routeTask, )

  connect(m_routeTracker, &RouteTracker::newVoiceGuidance, this, [this](VoiceGuidance* voiceGuidance)
  {
    qDebug("new voice guidance");
    qDebug() << voiceGuidance->text();
  });

  connect(m_routeTracker, &RouteTracker::trackingStatusChanged, this, [this](TrackingStatus* trackingStatus)
  {
    QString textString("Route status: \n");
    if (trackingStatus->destinationStatus() == DestinationStatus::NotReached || trackingStatus->destinationStatus() == DestinationStatus::Approaching)
    {
      textString += "Distance remaining: " + trackingStatus->routeProgress()->remainingDistance()->displayText() + "km. \n";
    }
    else if (trackingStatus->destinationStatus() == DestinationStatus::Reached)
    {
      textString += "Destination reached.\n";
    }
    m_textString = textString;
    emit textStringChanged();
  });

  connect(m_mapView->locationDisplay(), &LocationDisplay::autoPanModeChanged, this, [this](LocationDisplayAutoPanMode autoPanMode)
  {
    // enable recenter button
  });

  connect(m_mapView->locationDisplay(), &LocationDisplay::locationChanged, this, [this](const Location& location)
  {
//    m_routeTracker->trackLocation(location);
    Point projectedLocation = GeometryEngine::project(location.position(), SpatialReference::wgs84());
    QGeoPositionInfo tempLocation(QGeoCoordinate(projectedLocation.y(), projectedLocation.x()), QDateTime::currentDateTime());
    m_routeTracker->trackLocation(tempLocation);
  });

  connect(m_routeTracker, &RouteTracker::trackLocationCompleted, this, [this](QUuid)
  {
    qDebug("completed");
    m_routeTracker->generateVoiceGuidance();
  });

  // turn on map view's navigation mode
  m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Navigation);

  // Add a data source for the location display.
  m_simulatedLocationDataSource = new SimulatedLocationDataSource(this);
  m_simulatedLocationDataSource->setLocationsWithPolyline(m_route.routeGeometry());
  m_mapView->locationDisplay()->setDataSource(m_simulatedLocationDataSource);
  m_simulatedLocationDataSource->start();

//  m_mapView->locationDisplay()->setDataSource(new SimulatedLocationDataSource(this));

//  m_mapView->locationDisplay()->setDataSource(new SimulatedLocationDataSource(this));

}

QString NavigateRoute::textString() const
{
  return m_textString;
}
