
// Copyright 2024 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#include "AllRoadsLeadToRome.h"

#include "DirectionManeuver.h"
#include "GeocodeParameters.h"
#include "GeocodeResult.h"
#include "GeometryEngine.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "LocatorTask.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "Polyline.h"
#include "Route.h"
#include "RouteParameters.h"
#include "RouteResult.h"
#include "RouteTask.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "SpatialReference.h"
#include "Stop.h"
#include "SymbolTypes.h"

#include <QFuture>

using namespace Esri::ArcGISRuntime;

AllRoadsLeadToRome::AllRoadsLeadToRome(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISNavigationNight, this))
{
  // Instantiate parameters
  m_locatorTask = new LocatorTask(QUrl("https://geocode-api.arcgis.com/arcgis/rest/services/World/GeocodeServer"), this);
  m_routeTask = new RouteTask(QUrl("https://route-api.arcgis.com/arcgis/rest/services/World/Route/NAServer/Route_World"), this);

  // Create graphics
  m_romeGraphic = new Graphic(this);
  m_startGraphic = new Graphic(this);
  m_routeLine = new Graphic(this);

  // Set symbols for graphics and add them to the graphics overlay
  m_romeGraphic->setSymbol(new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("red"), 10.0, this));
  m_startGraphic->setSymbol(new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("green"), 10.0, this));
  m_routeLine->setSymbol(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("blue"), 3.0, this));

  // Create the graphics overlay and add the graphics to it
  m_graphicsOverlay = new GraphicsOverlay(this);
  m_graphicsOverlay->graphics()->append({m_romeGraphic, m_startGraphic, m_routeLine});
}

AllRoadsLeadToRome::~AllRoadsLeadToRome()
{
}

MapQuickView* AllRoadsLeadToRome::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void AllRoadsLeadToRome::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  // Add the graphics overlay to the map view once it is set
  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  // Connect to the mouseClicked signal -- this will run every time the mouse is clicked
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& click)
  {
    // Ignore the mouse click if the app is busy
    if (m_busy)
    {
      return;
    }

    // Save the click point
    const Point clickPoint = Point(GeometryEngine::project(m_mapView->screenToLocation(click.pos().x(), click.pos().y()), SpatialReference::wgs84()));

    // Set the app to busy after the click point is determined because the map view may shift when the app becomes busy and the directions list hides
    m_busy = true;
    emit busyChanged();

    // Unset the geometry off all graphics so they do not appear on the map
    for (auto graphic : *m_graphicsOverlay->graphics())
    {
      graphic->setGeometry({});
    }

    // Set the geometry of the green start point
    m_startGraphic->setGeometry(clickPoint);

    // Create and set geocode parameters
    GeocodeParameters parameters;
    parameters.setPreferredSearchLocation(clickPoint);
    parameters.setMinScore(90);
    parameters.setMaxResults(10);

    // Geocode the location of the nearest Rome
    m_locatorTask->geocodeWithParametersAsync("Rome", parameters)
        .then(this, [clickPoint, this](QList<GeocodeResult> resultList)
    {
      // If no results are found, set the app to not busy and return
      if (resultList.isEmpty())
      {
        m_busy = false;
        emit busyChanged();
        return;
      }

      // Find the closest result to the click point
      // We could potentially use GeometryEngine::nearestCoordinate, but that requires us to read all geometries to a multipoint geometry, so we'll just loop through the results
      double distance = INFINITY;
      GeocodeResult closestResult;

      for (const auto& result : resultList)
      {
        const auto resultDistance = GeometryEngine::distance(clickPoint, result.displayLocation());
        if (resultDistance < distance)
        {
          distance = resultDistance;
          closestResult = result;
        }
      }

      // Set the name and the geometry of the red Rome point
      m_destName = closestResult.label();
      m_romeGraphic->setGeometry(closestResult.displayLocation());

      // Get the default parameters from the route task service
      m_routeTask->createDefaultParametersAsync()
          .then(this, [clickPoint, closestResult, this](RouteParameters routeParameters)
      {
        // Set the routing parameters
        routeParameters.setReturnDirections(true);
        routeParameters.setStops({Stop(clickPoint), Stop(closestResult.displayLocation())});

        // Solve the route
        m_routeTask->solveRouteAsync(routeParameters)
            .then(this, [this](RouteResult routeResult)
        {
          if (routeResult.routes().isEmpty())
          {
            qDebug() << "No routes found!";
            m_busy = false;
            emit busyChanged();
            return;
          }

          // Get the best route and set the geometry of the route line
          const Route bestRoute = routeResult.routes().constFirst();
          m_routeLine->setGeometry(bestRoute.routeGeometry());

          // Save the text for the directions box
          m_routeTime = bestRoute.totalTime();
          m_routeLength = bestRoute.totalLength();
          m_directions = bestRoute.directionManeuvers();
          emit directionsChanged();

          // Task completed successfully! Set the app to not busy
          m_busy = false;
          emit busyChanged();
        })
        // If the route task fails, set the app to not busy
        .onCanceled(this, [this]{m_busy = false; emit busyChanged();})
            .onFailed(this, [this]{m_busy = false; emit busyChanged();});
      });
    })
        // If the locator task fails, set the app to not busy
        .onCanceled(this, [this]{m_busy = false; emit busyChanged();})
        .onFailed(this, [this]{m_busy = false; emit busyChanged();});
  });

  emit mapViewChanged();
}
