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

#include "GraphicsOverlay.h"
#include "Map.h"
#include "MapQuickView.h"
#include "Point.h"
#include "Route.h"
#include "RouteResult.h"
#include "RouteTask.h"
#include "SimpleMarkerSymbol.h"
#include "Stop.h"

#include <QList>
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

  GraphicsOverlay* routeOverlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(routeOverlay);

  m_routeTask = new RouteTask(routeTaskUrl, this);

  connect(m_routeTask, &RouteTask::solveRouteCompleted, this, [this](QUuid, RouteResult routeResult)
  {
    if (routeResult.isEmpty())
      return;
    Route route = routeResult.routes()[0];
  });

  connect(m_routeTask, &RouteTask::createDefaultParametersCompleted, this, [this](QUuid, RouteParameters defaultParameters)
  {
    m_routeParameters = defaultParameters;

    // set values for parameters
    m_routeParameters.setReturnStops(true);
    m_routeParameters.setReturnDirections(true);
    m_routeParameters.setReturnRoutes(true);
    m_routeParameters.setOutputSpatialReference(SpatialReference::wgs84());

    Stop stop1(conventionCenterPoint);
    Stop stop2(memorialPoint);
    Stop stop3(aerospaceMuseumPoint);

    QList<Stop> stopsList = {stop1, stop2, stop3};
    m_routeParameters.setStops(stopsList);

    m_routeTask->solveRoute(m_routeParameters);
  });

  connect(m_routeTask, &RouteTask::doneLoading, this, [this](Error error)
  {
    if (error.isEmpty())
    {
      m_routeTask->createDefaultParameters();
      qDebug("loaded");
    }
    else
    {
     qDebug() << error.message() << error.additionalMessage();
    }
  });

  m_routeTask->load();

  // add graphics for the predefined stops
  SimpleMarkerSymbol* stopSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, Qt::red, 20, this);
  routeOverlay->graphics()->append(new Graphic(conventionCenterPoint, stopSymbol, this));
  routeOverlay->graphics()->append(new Graphic(memorialPoint, stopSymbol, this));
  routeOverlay->graphics()->append(new Graphic(aerospaceMuseumPoint, stopSymbol, this));

  emit mapViewChanged();
}
