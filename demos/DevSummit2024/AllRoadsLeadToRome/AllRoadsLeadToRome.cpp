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

#include "GeocodeParameters.h"
#include "GeocodeResult.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "LocatorTask.h"
#include "Map.h"
#include "MapTypes.h"
#include "MapQuickView.h"
#include "Point.h"
#include "SimpleMarkerSymbol.h"
#include "SymbolTypes.h"
#include "GeometryEngine.h"
#include "SpatialReference.h"
#include "RouteTask.h"
#include "RouteParameters.h"
#include "Stop.h"
#include "RouteResult.h"
#include "Route.h"
#include "Polyline.h"
#include "SimpleLineSymbol.h"
#include "DirectionManeuverListModel.h"
#include "DirectionManeuver.h"

#include <QFuture>

using namespace Esri::ArcGISRuntime;

AllRoadsLeadToRome::AllRoadsLeadToRome(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISNavigationNight, this))
{
  m_locatorTask = new LocatorTask(QUrl("https://geocode-api.arcgis.com/arcgis/rest/services/World/GeocodeServer"), this);
  m_routeTask = new RouteTask(QUrl("https://route-api.arcgis.com/arcgis/rest/services/World/Route/NAServer/Route_World"), this);
  m_graphicsOverlay = new GraphicsOverlay(this);
  m_romeGraphic = new Graphic(this);
  m_startGraphic = new Graphic(this);
  m_routeLine = new Graphic(this);
  m_romeGraphic->setSymbol(new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("red"), 10.0, this));
  m_startGraphic->setSymbol(new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("green"), 10.0, this));
  m_routeLine->setSymbol(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("blue"), 3.0, this));
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

  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& click)
  {
    if (m_busy)
    {
      return;
    }

    const Point clickPoint = Point(GeometryEngine::project(m_mapView->screenToLocation(click.pos().x(), click.pos().y()), SpatialReference::wgs84()));

    m_busy = true;
    emit busyChanged();

    for (auto graphic : *m_graphicsOverlay->graphics())
    {
      graphic->setGeometry({});
    }
    m_startGraphic->setGeometry(clickPoint);
    GeocodeParameters parameters;
    parameters.setPreferredSearchLocation(clickPoint);
    parameters.setMinScore(90);
    parameters.setMaxResults(10);

    m_locatorTask->geocodeWithParametersAsync("Rome", parameters)
        .then(this, [clickPoint, this](QList<GeocodeResult> resultList)
    {
      if (resultList.isEmpty())
      {
        return;
      }
      QObject parent;
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
      qDebug() << closestResult.label();
      m_destName = closestResult.label();
      m_romeGraphic->setGeometry(closestResult.displayLocation());

      m_routeTask->createDefaultParametersAsync()
          .then(this, [clickPoint, closestResult, this](RouteParameters routeParameters)
      {
        routeParameters.setReturnDirections(true);
        routeParameters.setStops({Stop(clickPoint), Stop(closestResult.displayLocation())});
        m_routeTask->solveRouteAsync(routeParameters)
            .then(this, [this](RouteResult routeResult)
        {
          if (routeResult.routes().isEmpty())
          {
            qDebug() << "No routes found!";
            return;
          }

          const Route bestRoute = routeResult.routes().first();

          m_routeLine->setGeometry(bestRoute.routeGeometry());

          m_routeTime = bestRoute.totalTime();
          m_routeLength = bestRoute.totalLength();
          m_directions = bestRoute.directionManeuvers();
          emit directionsChanged();
          m_busy = false;
          emit busyChanged();
        })
        .onCanceled(this, [this]{m_busy = false; emit busyChanged();})
            .onFailed(this, [this]{m_busy = false; emit busyChanged();});
      });
    })
        .onCanceled(this, [this]{m_busy = false; emit busyChanged();})
        .onFailed(this, [this]{m_busy = false; emit busyChanged();});
  });

  emit mapViewChanged();
}
