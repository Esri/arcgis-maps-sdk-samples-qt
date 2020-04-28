// [WriteFile Name=ShowLocationHistory, Category=Maps]
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

#include "ShowLocationHistory.h"

#include "GeometryEngine.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "ImmutablePointCollection.h"
#include "Location.h"
#include "Map.h"
#include "MapQuickView.h"
#include "Polyline.h"
#include "PolylineBuilder.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleRenderer.h"
#include "SimulatedLocationDataSource.h"

using namespace Esri::ArcGISRuntime;

namespace
{
const Point circleRouteCenter(-117.195801, 34.056007, SpatialReference::wgs84());
constexpr int initialZoomScale = 10000;
}

ShowLocationHistory::ShowLocationHistory(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::darkGrayCanvasVector(this), this)),
  m_locationHistoryOverlay(new GraphicsOverlay(this)),
  m_locationHistoryLineOverlay(new GraphicsOverlay(this))
{
  // graphics overlay for displaying the trail
  SimpleLineSymbol* locationLineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::green, 2, this);
  m_locationHistoryLineOverlay->setRenderer(new SimpleRenderer(locationLineSymbol, this));

  // graphics overlay for showing points
  SimpleMarkerSymbol* locationPointSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, Qt::red, 3, this);
  m_locationHistoryOverlay->setRenderer(new SimpleRenderer(locationPointSymbol, this));

  m_polylineBuilder = new PolylineBuilder(SpatialReference::wgs84(), this);
}

void ShowLocationHistory::handleLocationReady()
{
  m_simulatedLocationDataSource = new SimulatedLocationDataSource(this);
  Polygon outerCircle = GeometryEngine::bufferGeodetic(circleRouteCenter, 1000, LinearUnit::feet(), 10, GeodeticCurveType::ShapePreserving);
  Polyline circlePath = outerCircle.toPolyline();
//  ImmutablePointCollection pointCollection = outerCircle.parts().part(0).points();
//  for (int i = 0; i < pointCollection.size(); ++i)
//  {
//    Location location;
//    location
//  }
  m_simulatedLocationDataSource->setLocationsWithPolyline(circlePath);
  m_simulatedLocationDataSource->start();

  m_mapView->locationDisplay()->setDataSource(m_simulatedLocationDataSource);
  m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Recenter);
  m_mapView->locationDisplay()->setInitialZoomScale(initialZoomScale);

  connect(m_mapView->locationDisplay(), &LocationDisplay::locationChanged, this, [this](const Location& location)
  {
    // if tracking is enabled then show location history

    if (!m_trackingEnabled)
    {
      // if graphics overlays contain graphics then clear them
//      if (m_locationHistoryLineOverlay->graphics() != nullptr)
//      {
//        m_locationHistoryLineOverlay->graphics()->clear();
//      }

//      if (m_locationHistoryOverlay->graphics() != nullptr)
//      {
//        m_locationHistoryOverlay->graphics()->clear();
//      }

      return;
    }

    // clear old route
//    qDebug() << m_locationHistoryLineOverlay->graphics()->size();
    m_locationHistoryLineOverlay->graphics()->clear();
//    qDebug() << m_locationHistoryLineOverlay->graphics()->size() << "\n\n";

    if (m_lastPosition.isValid())
    {
      m_polylineBuilder->addPoint(m_lastPosition);
      m_locationHistoryOverlay->graphics()->append(new Graphic(m_lastPosition, this));
    }

    // store the current position
    m_lastPosition = location.position();

    // update the polyline
    m_locationHistoryLineOverlay->graphics()->append(new Graphic(m_polylineBuilder->toGeometry()));
    
    

  });
}

ShowLocationHistory::~ShowLocationHistory() = default;

void ShowLocationHistory::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ShowLocationHistory>("Esri.Samples", 1, 0, "ShowLocationHistorySample");
}

MapQuickView* ShowLocationHistory::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ShowLocationHistory::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  m_mapView->graphicsOverlays()->append(m_locationHistoryLineOverlay);
  m_mapView->graphicsOverlays()->append(m_locationHistoryOverlay);

  handleLocationReady();

  emit mapViewChanged();
}
