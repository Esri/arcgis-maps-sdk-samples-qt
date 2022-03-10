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
  initGeometries();
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

  m_bufferSize = 0;
}

void GeotriggersDemo::usePointBufferFence()
{
  SimpleFillSymbol* bufferSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Null, Qt::blue, new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, Qt::red, 2, this), this);
  SimpleMarkerSymbol* simpleMarkerSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("white"), 14.0f /*size*/, this);

  m_fenceGraphic = new Graphic(m_point, simpleMarkerSymbol, this);

  m_graphicsOverlay->graphics()->append(m_fenceGraphic);
  m_graphicsOverlay->graphics()->append(new Graphic(GeometryEngine::buffer(m_point, m_bufferSize), bufferSymbol, this));
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

// Set the view (created in QML)
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
  initializeSimulatedLocationDisplay();

  runGeotriggers();

  emit mapViewChanged();
}

void GeotriggersDemo::initializeSimulatedLocationDisplay()
{
  m_simulatedLocationDataSource = new SimulatedLocationDataSource(this);

  // Create SimulationParameters starting at the current time, a velocity of 5 m/s, and a horizontal and vertical accuracy of 0.0
  SimulationParameters* simulationParameters = new SimulationParameters(QDateTime::currentDateTime(), 0.5, 1.5, 0.0, this);

  // Use the polyline as defined above or from this AGOL GeoJSON to define the path. retrieved from https://https://arcgisruntime.maps.arcgis.com/home/item.html?id=2a346cf1668d4564b8413382ae98a956
  m_simulatedLocationDataSource->setLocationsWithPolyline(m_path, simulationParameters);

  m_mapView->locationDisplay()->setDataSource(m_simulatedLocationDataSource);
  m_mapView->locationDisplay()->start();

  m_simulatedLocationDataSource->start();
}

void GeotriggersDemo::runGeotriggers()
{
  //usePointBufferFence();
  usePolygonFence();

  m_geotriggerFeed = new LocationGeotriggerFeed(m_simulatedLocationDataSource, this);

  GraphicFenceParameters* graphicFenceParameters = new GraphicFenceParameters({m_fenceGraphic}, m_bufferSize, this);

  FenceGeotrigger* fenceGeotrigger = new FenceGeotrigger(m_geotriggerFeed, FenceRuleType::EnterOrExit, graphicFenceParameters, new ArcadeExpression("'Current course is: ' + $feedfeature.course", this), "a name", this);
  fenceGeotrigger->setFeedAccuracyMode(FenceGeotriggerFeedAccuracyMode::UseGeometryWithAccuracy);
  fenceGeotrigger->setEnterExitSpatialRelationship(FenceEnterExitSpatialRelationship::EnterContainsAndExitDoesNotContain);

  GeotriggerMonitor* geotriggerMonitor = new GeotriggerMonitor(fenceGeotrigger, this);

  connect(geotriggerMonitor, &GeotriggerMonitor::geotriggerNotification, this, [this](GeotriggerNotificationInfo* geotriggerNotificationInfo)
  {
    Point loc = geotriggerNotificationInfo->feedLocation().position();
    qDebug() << geotriggerNotificationInfo->message();

    FenceGeotriggerNotificationInfo* fenceGeotriggerNotificationInfo = static_cast<FenceGeotriggerNotificationInfo*>(geotriggerNotificationInfo);

    if (fenceGeotriggerNotificationInfo->fenceNotificationType() == FenceNotificationType::Entered)
    {
      qDebug() << "welcome!";
      m_graphicsOverlay->graphics()->append(new Graphic(loc, new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, Qt::green, 15, this), this));
    }
    else
    {
      qDebug() << "goodbye!";
      m_graphicsOverlay->graphics()->append(new Graphic(loc, new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, Qt::red, 15, this), this));
    }
  });

  geotriggerMonitor->start();
}
