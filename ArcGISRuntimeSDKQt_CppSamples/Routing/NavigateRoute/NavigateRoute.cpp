// [WriteFile Name=NavigateRoute, Category=Routing]
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

#include "DirectionManeuverListModel.h"
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
#include "SimulationParameters.h"
#include "Stop.h"
#include "TrackingDistance.h"
#include "TrackingProgress.h"
#include "TrackingStatus.h"
#include "VoiceGuidance.h"

#include <memory>
#include <QList>
#include <QTextToSpeech>
#include <QTime>
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
  m_routeOverlay = new GraphicsOverlay(this);
  m_routeTask = new RouteTask(routeTaskUrl, this);
  m_speaker = new QTextToSpeech(this);
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

  m_mapView->graphicsOverlays()->append(m_routeOverlay);
  connectRouteTaskSignals();

  m_routeTask->load();

  // add graphics for the predefined stops
  SimpleMarkerSymbol* stopSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, Qt::red, 20, this);
  m_routeOverlay->graphics()->append(new Graphic(conventionCenterPoint, stopSymbol, this));
  m_routeOverlay->graphics()->append(new Graphic(memorialPoint, stopSymbol, this));
  m_routeOverlay->graphics()->append(new Graphic(aerospaceMuseumPoint, stopSymbol, this));

  emit mapViewChanged();
}

void NavigateRoute::connectRouteTaskSignals()
{
  connect(m_routeTask, &RouteTask::solveRouteCompleted, this, [this](QUuid, RouteResult routeResult)
  {
    if (routeResult.isEmpty())
      return;

    if (routeResult.routes().empty())
      return;

    m_routeResult = routeResult;
    m_route = m_routeResult.routes()[0];

    // adjust viewpoint to enclose the route with a 100 DPI padding
    m_mapView->setViewpointGeometry(m_route.routeGeometry(), 100);

    // create a graphic to show the route
    m_routeAheadGraphic = new Graphic(m_route.routeGeometry(), new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, Qt::blue, 5, this), this);

    // create a graphic to represent the route that's been traveled (initially empty).
    m_routeTraveledGraphic = new Graphic(Geometry(), new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::cyan, 3, this), this);
    m_routeOverlay->graphics()->append(m_routeAheadGraphic);
    m_routeOverlay->graphics()->append(m_routeTraveledGraphic);

    m_navigationEnabled = true;
    emit navigationEnabledChanged();
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

  connect(m_routeTask, &RouteTask::doneLoading, this, [this](const Error& error)
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
}

bool NavigateRoute::navigationEnabled() const
{
  return m_navigationEnabled;
}

bool NavigateRoute::recenterEnabled() const
{
  return m_recenterEnabled;
}

void NavigateRoute::startNavigation()
{
  // disable the navigation button
  m_navigationEnabled = false;
  emit navigationEnabledChanged();

  // get the directions for the route
  m_directions = m_route.directionManeuvers(this);

  // create a route tracker
  m_routeTracker = new RouteTracker(m_routeResult, 0, this);
  connectRouteTrackerSignals();

  // enable "recenter" button when location display is moved from nagivation mode
  connect(m_mapView->locationDisplay(), &LocationDisplay::autoPanModeChanged, this, [this](LocationDisplayAutoPanMode autoPanMode)
  {
    m_recenterEnabled = autoPanMode != LocationDisplayAutoPanMode::Navigation;
    emit recenterEnabledChanged();
  });

  connect(m_mapView->locationDisplay(), &LocationDisplay::locationChanged, this, [this](const Location& location)
  {
    m_routeTracker->trackLocation(location);
  });

  // turn on map view's navigation mode
  m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Navigation);

  // add a data source for the location display
  SimulationParameters* simulationParameters = new SimulationParameters(QDateTime::currentDateTime(), 40.0, 0.0, 0.0, this); // set speed
  m_simulatedLocationDataSource = new SimulatedLocationDataSource(this);
  m_simulatedLocationDataSource->setLocationsWithPolyline(m_route.routeGeometry(), simulationParameters);
  m_mapView->locationDisplay()->setDataSource(m_simulatedLocationDataSource);
  m_simulatedLocationDataSource->start();
}

void NavigateRoute::connectRouteTrackerSignals()
{
  connect(m_routeTracker, &RouteTracker::newVoiceGuidance, this, [this](VoiceGuidance* rawVoiceGuidance)
  {
    auto voiceGuidance = std::unique_ptr<VoiceGuidance>(rawVoiceGuidance);
    m_speaker->say(voiceGuidance->text());
  });

  connect(m_routeTracker, &RouteTracker::trackingStatusChanged, this, [this](TrackingStatus* rawTrackingStatus)
  {
    auto trackingStatus = std::unique_ptr<TrackingStatus>(rawTrackingStatus);
    QString textString("Route status: \n");
    if (trackingStatus->destinationStatus() == DestinationStatus::NotReached || trackingStatus->destinationStatus() == DestinationStatus::Approaching)
    {
      textString += "Distance remaining: " + trackingStatus->routeProgress()->remainingDistance()->displayText() + " " +
          trackingStatus->routeProgress()->remainingDistance()->displayTextUnits().pluralDisplayName() + "\n";
      QTime time = QTime::fromMSecsSinceStartOfDay(trackingStatus->routeProgress()->remainingTime() * 60 * 1000); // convert time to milliseconds
      textString += "Time remaining: " + time.toString("hh:mm:ss") + "\n";

      // display next direction
      if (DirectionManeuverListModel* directionList = dynamic_cast<DirectionManeuverListModel*>(m_directions))
      {
        if (trackingStatus->currentManeuverIndex() + 1 < directionList->directionManeuvers().length())
        {
          textString += "Next direction: "  + directionList->directionManeuvers()[trackingStatus->currentManeuverIndex() + 1].directionText() + "\n";
        }
      }
      m_routeTraveledGraphic->setGeometry(trackingStatus->routeProgress()->traversedGeometry());
      m_routeAheadGraphic->setGeometry(trackingStatus->routeProgress()->remainingGeometry());
    }
    else if (trackingStatus->destinationStatus() == DestinationStatus::Reached)
    {
      textString += "Destination reached.\n";

      // set the route geometries to reflect the completed route
      m_routeAheadGraphic->setGeometry(Geometry());
      m_routeTraveledGraphic->setGeometry(trackingStatus->routeResult().routes()[0].routeGeometry());

      // navigate to next stop, if available
      if (trackingStatus->remainingDestinationCount() > 1)
      {
        m_routeTracker->switchToNextDestination();
      }
      else
      {
        m_simulatedLocationDataSource->stop();
      }
    }
    m_textString = textString;
    emit textStringChanged();
  });

  connect(m_routeTracker, &RouteTracker::trackLocationCompleted, this, [this](QUuid)
  {
    m_routeTracker->generateVoiceGuidance();
  });
}

QString NavigateRoute::textString() const
{
  return m_textString;
}

void NavigateRoute::recenterMap()
{
  m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Navigation);
}
