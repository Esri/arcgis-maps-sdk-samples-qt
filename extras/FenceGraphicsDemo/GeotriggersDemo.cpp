// Copyright 2022 ESRI
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

#include "GeotriggersDemo.h"

#include "Basemap.h"
#include "Map.h"
#include "MapQuickView.h"

#include "ArcadeExpression.h"
#include "Geotrigger.h"
#include "GeotriggerMonitor.h"
#include "GeotriggerNotificationInfo.h"
#include "GraphicFenceParameters.h"
#include "GraphicsOverlayFenceParameters.h"
#include "FeatureFenceParameters.h"
#include "FenceGeotrigger.h"
#include "GeotriggersTypes.h"
#include "FenceParameters.h"
#include "FenceGeotriggerNotificationInfo.h"
#include "LocationGeotriggerFeed.h"
#include "GeometryEngine.h"

#include "Point.h"
#include "Polygon.h"
#include "PolygonBuilder.h"
#include "Polyline.h"
#include "PolylineBuilder.h"

#include "SimpleMarkerSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleFillSymbol.h"
#include "SimulationParameters.h"
#include "SimulatedLocationDataSource.h"

#include "GeometryBuilder.h"

#include <QUrl>

using namespace Esri::ArcGISRuntime;

GeotriggersDemo::GeotriggersDemo(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISStreets, this)),
  m_graphicsOverlay(new GraphicsOverlay(this))
{
  /* Create the point and polygon fence graphics and a path for the simulated location display */
  initGeometries();
  m_bufferSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Null, Qt::black, new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, Qt::red, 2, this), this);
}

GeotriggersDemo::~GeotriggersDemo()
{
}

void GeotriggersDemo::initGeometries()
{
  m_point = Point(0, 1, SpatialReference::webMercator());

  PolygonBuilder polygonBuilder(SpatialReference::webMercator());
  polygonBuilder.addPoint(-2,-5);
  polygonBuilder.addPoint(-2,5);
  polygonBuilder.addPoint(2,5);
  polygonBuilder.addPoint(2,-5);
  m_polygon = polygonBuilder.toPolygon();

  PolylineBuilder polylineBuilder(SpatialReference::webMercator());
  polylineBuilder.addPoint(-4, 0);
  polylineBuilder.addPoint(4, 0);
  polylineBuilder.addPoint(-4,0);
  m_path = polylineBuilder.toPolyline();
}

void GeotriggersDemo::usePointBufferFence()
{
  SimpleMarkerSymbol* simpleMarkerSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("white"), 14.0f /*size*/, this);

  m_fenceGraphic = new Graphic(m_point, simpleMarkerSymbol, this);

  m_graphicsOverlay->graphics()->append(m_fenceGraphic);
  m_graphicsOverlay->graphics()->append(new Graphic(GeometryEngine::buffer(m_point, m_bufferSize), new SimpleFillSymbol(SimpleFillSymbolStyle::Null, Qt::black, new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, Qt::red, 2, this), this), this));
}

void GeotriggersDemo::usePolygonFence()
{
  SimpleFillSymbol* polygonSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("#88ffffff"), this);
  m_fenceGraphic = new Graphic(m_polygon, polygonSymbol, this);
  m_graphicsOverlay->graphics()->append(m_fenceGraphic);
}

MapQuickView* GeotriggersDemo::mapView() const
{
  return m_mapView;
}

void GeotriggersDemo::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  m_mapView->setViewpoint(Viewpoint(m_polygon.extent()));

  runGeotriggers();

  emit mapViewChanged();
}

void GeotriggersDemo::runGeotriggers()
{
  m_bufferSize = 2;
  m_horizontalAccuracy = 0;
  initializeSimulatedLocationDisplay();

  //usePolygonFence();
  usePointBufferFence();

  // The users location, input
  m_geotriggerFeed = new LocationGeotriggerFeed(
        m_simulatedLocationDataSource, this);

  /* The graphics and buffer around them to monitor */
  GraphicFenceParameters* graphicFenceParameters =
      new GraphicFenceParameters({m_fenceGraphic}, m_bufferSize, this);

  FenceGeotrigger* fenceGeotrigger = new FenceGeotrigger(
        m_geotriggerFeed, // user's location
        FenceRuleType::EnterOrExit, // When to trigger a geotriggerNotification
        graphicFenceParameters,  // the graphics and buffer to monitor
        new ArcadeExpression("'Current course is: ' + $feedfeature.course", this), // message to include in notification
        "Graphic Fence Geotrigger", // name of the geotrigger for identification purposes
        this); // parent object

  /* Choose to include the user's horizontal accuracy in trigger evaluations (since 100.13)
   * Default is to just use the geometry with no horizontal accuracy */
  //    fenceGeotrigger->setFeedAccuracyMode(
  //          FenceGeotriggerFeedAccuracyMode::UseGeometryWithAccuracy);

  /* Optionally choose when the geotrigger notification is triggered (since 100.13)
   * Default is EnterContainsAndExitDoesNotIntersect */
  //    fenceGeotrigger->setEnterExitSpatialRelationship(
  //          FenceEnterExitSpatialRelationship::EnterContainsAndExitDoesNotContain);

  /* Create a geotrigger monitor for the fence geotrigger */
  GeotriggerMonitor* geotriggerMonitor = new GeotriggerMonitor(fenceGeotrigger, this);

  /* Create a slot to listen for the geotriggerNotification signal */
  connect(geotriggerMonitor, &GeotriggerMonitor::geotriggerNotification,
          this, [this](GeotriggerNotificationInfo* geotriggerNotificationInfo)
  {
    /* We can get the user's position when the notification is triggered */
    Point loc = geotriggerNotificationInfo->feedLocation().position();
    m_graphicsOverlay->graphics()->append(new Graphic(GeometryEngine::buffer(loc, m_horizontalAccuracy), new SimpleFillSymbol(SimpleFillSymbolStyle::Null, Qt::black, new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, Qt::blue, 2, this), this), this));

    /* GeotriggerNotificationInfo exposes the message set in the FenceGeotrigger */
    qDebug() << geotriggerNotificationInfo->message();

    /* This provides information about the fence graphic itself */
    FenceGeotriggerNotificationInfo* fenceGeotriggerNotificationInfo = static_cast<FenceGeotriggerNotificationInfo*>(geotriggerNotificationInfo);

    if (fenceGeotriggerNotificationInfo->fenceNotificationType() ==
        FenceNotificationType::Entered)
    {
      qDebug() << "welcome!";

      /* We can edit the fence graphic directly */
      //dynamic_cast<Graphic*>(fenceGeotriggerNotificationInfo->fenceGeoElement())->setSymbol(new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("#8800ff00"), this));
      m_graphicsOverlay->graphics()->append(new Graphic(loc, new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, Qt::green, 15, this), this));
    }
    else
    {
      qDebug() << "goodbye!";

      //dynamic_cast<Graphic*>(fenceGeotriggerNotificationInfo->fenceGeoElement())->setSymbol(new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("#88ff0000"), this));
      m_graphicsOverlay->graphics()->append(new Graphic(loc, new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, Qt::red, 15, this), this));
    }
  });

  geotriggerMonitor->start();
}

void GeotriggersDemo::initializeSimulatedLocationDisplay()
{
  m_simulatedLocationDataSource = new SimulatedLocationDataSource(this);

  SimulationParameters* simulationParameters = new SimulationParameters(QDateTime::currentDateTime(), 0.5, m_horizontalAccuracy, 0.0, this);

  m_simulatedLocationDataSource->setLocationsWithPolyline(m_path, simulationParameters);

  m_mapView->locationDisplay()->setDataSource(m_simulatedLocationDataSource);
  m_mapView->locationDisplay()->start();

  m_simulatedLocationDataSource->start();
}
