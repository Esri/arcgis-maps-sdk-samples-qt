// [WriteFile Name=RouteAroundBarriers, Category=Routing]
// [Legal]
// Copyright 2020 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "RouteAroundBarriers.h"

// ArcGIS Maps SDK headers
#include "CompositeSymbol.h"
#include "DirectionManeuverListModel.h"
#include "Error.h"
#include "GeometryEngine.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "PictureMarkerSymbol.h"
#include "Point.h"
#include "Polygon.h"
#include "PolygonBarrier.h"
#include "Polyline.h"
#include "Route.h"
#include "RouteResult.h"
#include "RouteTask.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleRenderer.h"
#include "Stop.h"
#include "SymbolTypes.h"
#include "TextSymbol.h"
#include "Viewpoint.h"

// Qt headers
#include <QDir>
#include <QFuture>
#include <QUuid>

using namespace Esri::ArcGISRuntime;

namespace
{
const QUrl pinUrl("qrc:/Samples/Routing/RouteAroundBarriers/orange_symbol.png");
const QUrl routeTaskUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/NetworkAnalysis/SanDiego/NAServer/Route");
}

RouteAroundBarriers::RouteAroundBarriers(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISStreets, this)),
  m_routeOverlay(new GraphicsOverlay(this)),
  m_stopsOverlay(new GraphicsOverlay(this)),
  m_barriersOverlay(new GraphicsOverlay(this))
{
  // create symbols for displaying the barriers and the route line
  m_barrierSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::DiagonalCross, Qt::red, this);
  SimpleLineSymbol* routeLineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::blue, 3, this);

  SimpleRenderer* routeRenderer = new SimpleRenderer(routeLineSymbol, this);
  m_routeOverlay->setRenderer(routeRenderer);

  m_pinSymbol = new PictureMarkerSymbol(pinUrl, this);
  m_pinSymbol->setHeight(50);
  m_pinSymbol->setWidth(50);
  m_pinSymbol->setOffsetY(m_pinSymbol->height() / 2);

  // create route task
  m_routeTask = new RouteTask(routeTaskUrl, this);
}

RouteAroundBarriers::~RouteAroundBarriers() = default;

void RouteAroundBarriers::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<RouteAroundBarriers>("Esri.Samples", 1, 0, "RouteAroundBarriersSample");
}

MapQuickView* RouteAroundBarriers::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void RouteAroundBarriers::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  m_mapView->setViewpointAsync(Viewpoint(32.727, -117.1750, 40000));

  // add the graphics overlays to the MapView
  m_mapView->graphicsOverlays()->append(m_routeOverlay);
  m_mapView->graphicsOverlays()->append(m_stopsOverlay);
  m_mapView->graphicsOverlays()->append(m_barriersOverlay);

  connectRouteSignals();
  m_routeTask->load();
  emit mapViewChanged();
}

void RouteAroundBarriers::connectRouteSignals()
{
  connect(m_routeTask, &RouteTask::doneLoading, this, [this](const Error& loadError)
  {
    if (!loadError.isEmpty())
    {
      qDebug() << loadError.message() << loadError.additionalMessage();
    }
    m_routeTask->createDefaultParametersAsync().then(this, [this](const RouteParameters& defaultParameters)
    {
      m_routeParameters = defaultParameters;

      // set flags to return stops and directions
      m_routeParameters.setReturnStops(true);
      m_routeParameters.setReturnDirections(true);
    });
  });

  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& e)
  {
    const Point clickedPoint = m_mapView->screenToLocation(e.position().x(), e.position().y());
    if (m_addStops)
    {
      // add stop to list of stops
      const Stop stopPoint(clickedPoint);
      m_stopsList << stopPoint;

      // create a marker symbol and graphics, and add the graphics to the graphics overlay
      TextSymbol* textSymbol = new TextSymbol(QString::number(m_stopsList.size()), Qt::white, 16,
                                              HorizontalAlignment::Center, VerticalAlignment::Bottom, this);
      textSymbol->setOffsetY(m_pinSymbol->height() / 2);
      CompositeSymbol* newStopSymbol = new CompositeSymbol(QList<Symbol*>{m_pinSymbol, textSymbol}, this);

      Graphic* stopGraphic = new Graphic(clickedPoint, newStopSymbol, this);
      m_stopsOverlay->graphics()->append(stopGraphic);

      createAndDisplayRoute();
    }
    else if (m_addBarriers)
    {
      // add barrier to list
      const Polygon barrierPolygon = GeometryEngine::buffer(clickedPoint, 500);
      const PolygonBarrier barrier(barrierPolygon);
      m_barriersList << barrier;

      Graphic* barrierGraphic = new Graphic(barrierPolygon, m_barrierSymbol, this);
      m_barriersOverlay->graphics()->append(barrierGraphic);

      createAndDisplayRoute();
    }
  });
}

void RouteAroundBarriers::createAndDisplayRoute()
{
  if (m_stopsList.size() > 1)
  {
    // clear the previous route, if it exists
    if (m_routeOverlay)
      m_routeOverlay->graphics()->clear();

    // clear the directions list
    if (m_directions)
    {
      delete m_directions;
      m_directions = nullptr;
    }

    m_routeParameters.setStops(m_stopsList);
    m_routeParameters.setPolygonBarriers(m_barriersList);
    m_routeParameters.setFindBestSequence(m_findBestSequence);
    m_routeParameters.setPreserveFirstStop(m_preserveFirstStop);
    m_routeParameters.setPreserveLastStop(m_preserveLastStop);

    m_routeTask->solveRouteAsync(m_routeParameters).then(this, [this](const RouteResult& routeResult)
    {
      if (routeResult.isEmpty())
        return;

      const Route route = std::as_const(routeResult).routes()[0];
      const Geometry routeGeometry = route.routeGeometry();
      Graphic* routeGraphic = new Graphic(routeGeometry, this);
      m_routeOverlay->graphics()->append(routeGraphic);

      m_directions = route.directionManeuvers(this);
      emit directionsChanged();
    });
  }
}

void RouteAroundBarriers::clearRouteAndGraphics()
{
  // clear stops from route parameters and stops list
  m_routeParameters.clearStops();
  m_stopsList.clear();

  // clear barriers
  m_routeParameters.clearPolygonBarriers();
  m_barriersList.clear();

  // clear directions list
  if (m_directions)
  {
    delete m_directions;
    m_directions = nullptr;
    emit directionsChanged();
  }

  // delete graphics from overlay, then clear graphics overlays
  for (GraphicsOverlay* overlay : *m_mapView->graphicsOverlays())
  {
    if (overlay)
    {
      for (Graphic* graphic : *overlay->graphics())
      {
        delete graphic;
      }
      overlay->graphics()->clear();
    }
  }
}

void RouteAroundBarriers::clearDirections()
{
  if (m_directions)
  {
    delete m_directions;
    m_directions = nullptr;
    emit directionsChanged();
  }
}
