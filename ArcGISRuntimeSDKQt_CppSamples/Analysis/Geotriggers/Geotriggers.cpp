// [WriteFile Name=Geotriggers, Category=Analysis]
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

#include "Geotriggers.h"

#include "ArcadeExpression.h"
#include "FenceGeotrigger.h"
#include "GeotriggerMonitor.h"
#include "GeotriggerNotification.h"
#include "GeotriggersTypes.h"
#include "Graphic.h"
#include "GraphicFenceParameters.h"
#include "LocationGeotriggerFeed.h"
#include "Map.h"
#include "MapQuickView.h"
#include "PolylineBuilder.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleRenderer.h"
#include "SimulatedLocationDataSource.h"
#include "SimulationParameters.h"

using namespace Esri::ArcGISRuntime;

namespace
{
constexpr int initialZoomScale = 8000;
const Point initialCenter(-13185535.98, 4037766.28);
const QString polylineJson("{\"paths\":[[[-13185646.046666779,4037971.5966668758],[-13185586.780000051,4037827.6633333955],"
                           "[-13185514.813333312,4037709.1299999417],[-13185569.846666701,4037522.8633330846]]],"
                           "\"spatialReference\":{\"wkid\":102100,\"latestWkid\":3857}}");
}

Geotriggers::Geotriggers(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISDarkGray, this))
{
}

Geotriggers::~Geotriggers() = default;

void Geotriggers::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<Geotriggers>("Esri.Samples", 1, 0, "GeotriggersSample");
}

MapQuickView* Geotriggers::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void Geotriggers::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  m_mapView->setViewpointCenter(initialCenter, initialZoomScale);

  handleLocationChanges();
  setup();

  emit mapViewChanged();
}

void Geotriggers::handleLocationChanges()
{
  m_simulatedLocationDataSource = new SimulatedLocationDataSource(this);
  SimulationParameters* simulationParameters = new SimulationParameters(QDateTime::currentDateTime(), 30.0, 0.0, 0.0, this); // set speed

  m_simulatedLocationDataSource->setLocationsWithPolyline(Polyline::fromJson(polylineJson), simulationParameters);

  m_mapView->locationDisplay()->setDataSource(m_simulatedLocationDataSource);
  m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Recenter);
  m_mapView->locationDisplay()->setInitialZoomScale(initialZoomScale);
  m_mapView->locationDisplay()->start();

  m_simulatedLocationDataSource->start();
}

void Geotriggers::setup()
{
  GraphicsOverlay* graphicsOverlay = new GraphicsOverlay(this);

  QVariantMap test;
  test["name"] = "blue";

  m_mapView->graphicsOverlays()->append(graphicsOverlay);
  Graphic* graphic = new Graphic(
        Point(-13185514, 4037709, SpatialReference::webMercator()),
        test,
        new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, Qt::blue, 10, this),
        this);

  test["name"] = "red";

  Graphic* graphic2 = new Graphic(
        Point(-13185514, 4037719, SpatialReference::webMercator()),
        test,
        new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, Qt::red, 10, this),
        this);

  graphicsOverlay->graphics()->append(graphic);
  graphicsOverlay->graphics()->append(graphic2);

  GeotriggerFeed* feed = new LocationGeotriggerFeed(m_simulatedLocationDataSource, this);
  GraphicFenceParameters* graphicFenceParameters = new GraphicFenceParameters(100, graphicsOverlay, this);

  ArcadeExpression* expression = new ArcadeExpression("$fencefeature.name + ' heading ' + Round($feedfeature.course) + '°'", this);

  FenceGeotrigger* fenceGeotrigger = new FenceGeotrigger(feed, FenceRuleType::EnterOrExit, graphicFenceParameters, expression, "blue", this);

  m_geotriggerMonitor = new GeotriggerMonitor(fenceGeotrigger, this);

  connect(m_geotriggerMonitor, &GeotriggerMonitor::geotriggerNotification, this, [this, graphicsOverlay](GeotriggerNotification* notification)
  {
    qDebug() << "triggered " << notification->message();
    graphicsOverlay->graphics()->append(new Graphic(notification->feedGeometry(), new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, Qt::blue, 10, this), this));
  });

  m_geotriggerMonitor->start();
}
