// [WriteFile Name=FindRoute, Category=Routing]
// [Legal]
// Copyright 2016 Esri.

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

#include "FindRoute.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "ArcGISVectorTiledLayer.h"
#include "GraphicsOverlay.h"
#include "Viewpoint.h"
#include "Point.h"
#include "SpatialReference.h"
#include "SimpleRenderer.h"
#include "SimpleLineSymbol.h"
#include "PictureMarkerSymbol.h"
#include "RouteTask.h"
#include "RouteParameters.h"
#include "Stop.h"

using namespace Esri::ArcGISRuntime;

FindRoute::FindRoute(QQuickItem* parent) :
  QQuickItem(parent)
{
}

FindRoute::~FindRoute() = default;

void FindRoute::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterUncreatableType<QAbstractListModel>("Esri.Samples", 1, 0, "AbstractListModel", "AbstractListModel is uncreateable");
  qmlRegisterType<FindRoute>("Esri.Samples", 1, 0, "FindRouteSample");
}

void FindRoute::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // create a new basemap instance
  Basemap* basemap = new Basemap(BasemapStyle::ArcGISNavigation, this);

  // create a new map instance
  m_map = new Map(basemap, this);
  m_map->setInitialViewpoint(Viewpoint(Point(-13041154, 3858170, SpatialReference(3857)), 1e5));

  // set map on the map view
  m_mapView->setMap(m_map);

  // create initial graphics overlays
  m_routeGraphicsOverlay = new GraphicsOverlay(this);
  SimpleLineSymbol* simpleLineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("cyan"), 4, this);
  SimpleRenderer* simpleRenderer = new SimpleRenderer(simpleLineSymbol, this);
  m_routeGraphicsOverlay->setRenderer(simpleRenderer);
  m_stopsGraphicsOverlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(m_routeGraphicsOverlay);
  m_mapView->graphicsOverlays()->append(m_stopsGraphicsOverlay);

  // connect to loadStatusChanged signal
  connect(m_map, &Map::loadStatusChanged, this, [this](LoadStatus loadStatus)
  {
    if (loadStatus == LoadStatus::Loaded)
    {
      addStopGraphics();
      setupRouteTask();
    }
  });
}

void FindRoute::addStopGraphics()
{
  //! [FindRoute cpp addStopGraphics]
  // create the stop graphics' geometry
  Point stop1Geometry(-13041171, 3860988, SpatialReference(3857));
  Point stop2Geometry(-13041693, 3856006, SpatialReference(3857));

  // create the stop graphics' symbols
  PictureMarkerSymbol* stop1Symbol = getPictureMarkerSymbol(QUrl("qrc:/Samples/Routing/FindRoute/pinA.png"));
  PictureMarkerSymbol* stop2Symbol = getPictureMarkerSymbol(QUrl("qrc:/Samples/Routing/FindRoute/pinB.png"));

  // create the stop graphics
  Graphic* stop1Graphic = new Graphic(stop1Geometry, stop1Symbol, this);
  Graphic* stop2Graphic = new Graphic(stop2Geometry, stop2Symbol, this);

  // add to the overlay
  m_stopsGraphicsOverlay->graphics()->append(stop1Graphic);
  m_stopsGraphicsOverlay->graphics()->append(stop2Graphic);
  //! [FindRoute cpp addStopGraphics]
}

// Helper function for creating picture marker symbols
PictureMarkerSymbol* FindRoute::getPictureMarkerSymbol(QUrl imageUrl)
{
  PictureMarkerSymbol* pictureMarkerSymbol = new PictureMarkerSymbol(imageUrl, this);
  pictureMarkerSymbol->setWidth(32);
  pictureMarkerSymbol->setHeight(32);
  pictureMarkerSymbol->setOffsetY(16);
  return pictureMarkerSymbol;
}

void FindRoute::setupRouteTask()
{
  //! [FindRoute new RouteTask]
  // create the route task pointing to an online service
  m_routeTask = new RouteTask(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/NetworkAnalysis/SanDiego/NAServer/Route"), this);

  // connect to loadStatusChanged signal
  connect(m_routeTask, &RouteTask::loadStatusChanged, this, [this](LoadStatus loadStatus)
  {
    if (loadStatus == LoadStatus::Loaded)
    {
      // Request default parameters once the task is loaded
      m_routeTask->createDefaultParameters();
    }
  });
  //! [FindRoute new RouteTask]

  //! [FindRoute connect RouteTask signals]
  // connect to createDefaultParametersCompleted signal
  connect(m_routeTask, &RouteTask::createDefaultParametersCompleted, this, [this](QUuid, RouteParameters routeParameters)
  {
    // Store the resulting route parameters
    m_routeParameters = routeParameters;
  });

  // connect to solveRouteCompleted signal
  connect(m_routeTask, &RouteTask::solveRouteCompleted, this, [this](QUuid, RouteResult routeResult)
  {
    // Add the route graphic once the solve completes
    Route generatedRoute = routeResult.routes().at(0);
    Graphic* routeGraphic = new Graphic(generatedRoute.routeGeometry(), this);
    m_routeGraphicsOverlay->graphics()->append(routeGraphic);

    // set the direction maneuver list model
    m_directions = generatedRoute.directionManeuvers(this);
    emit directionsChanged();

    // emit that the route has solved successfully
    emit solveRouteComplete();
  });
  //! [FindRoute connect RouteTask signals]

  // load the route task
  m_routeTask->load();
}

QAbstractListModel* FindRoute::directions()
{
  return m_directions;
}

//! [FindRoute solveRoute]
void FindRoute::solveRoute()
{
  if (m_routeTask->loadStatus() == LoadStatus::Loaded)
  {
    if (!m_routeParameters.isEmpty())
    {
      // set parameters to return directions
      m_routeParameters.setReturnDirections(true);

      // clear previous stops from the parameters
      m_routeParameters.clearStops();

      // set the stops to the parameters
      Stop stop1(geometry_cast<Point>(m_stopsGraphicsOverlay->graphics()->at(0)->geometry()));
      stop1.setName("Origin");
      Stop stop2(geometry_cast<Point>(m_stopsGraphicsOverlay->graphics()->at(1)->geometry()));
      stop2.setName("Destination");
      m_routeParameters.setStops(QList<Stop> { stop1, stop2 });

      // solve the route with the parameters
      m_routeTask->solveRoute(m_routeParameters);
    }
  }
}
//! [FindRoute solveRoute]
