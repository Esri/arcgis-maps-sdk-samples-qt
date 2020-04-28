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
constexpr int initialZoomScale = 10000;
const QList<Point> polylinePoints = {Point(-13011809.657491295,4039640.7880717744),Point(-13011701.568936104,4039568.214327574),Point(-13011421.31075371,4039507.9935611095),Point(-13011391.20037048,4039491.7802778305),Point(-13011322.48693183,4039583.6555497493),Point(-13011388.112126058,4039654.6851717336),Point(-13011265.354409827,4039800.6047212547),Point(-13011536.347858932,4039929.538926385),Point(-13011405.869531592,4039975.0905317874),Point(-13011511.64190346,4040024.5024427325),Point(-13011580.355342126,4040222.922147628),Point(-13011617.41427536,4040253.8045919817),Point(-13011693.848325104,4040314.7974195546),Point(-13011672.230614064,4040421.34185253),Point(-13011601.973053196,4040524.0259799687)};
}

ShowLocationHistory::ShowLocationHistory(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::imageryWithLabelsVector(this), this)),
  m_locationHistoryOverlay(new GraphicsOverlay(this)),
  m_locationHistoryLineOverlay(new GraphicsOverlay(this))
{
  // graphics overlay for displaying the trail
  SimpleLineSymbol* locationLineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::green, 2, this);
  m_locationHistoryLineOverlay->setRenderer(new SimpleRenderer(locationLineSymbol, this));

  // graphics overlay for showing points
  SimpleMarkerSymbol* locationPointSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, Qt::red, 3, this);
  m_locationHistoryOverlay->setRenderer(new SimpleRenderer(locationPointSymbol, this));

  m_polylineBuilder = new PolylineBuilder(SpatialReference::webMercator(), this);
}

void ShowLocationHistory::handleLocationReady()
{
  m_simulatedLocationDataSource = new SimulatedLocationDataSource(this);

  PolylineBuilder polylineBuilder(SpatialReference::webMercator());
  polylineBuilder.addPoints(polylinePoints);

  m_simulatedLocationDataSource->setLocationsWithPolyline(GeometryEngine::densify(polylineBuilder.toPolyline(), 10));
//  m_simulatedLocationDataSource->setLocationsWithPolyline(polylineBuilder.toPolyline());
  m_simulatedLocationDataSource->start();

  m_mapView->locationDisplay()->setDataSource(m_simulatedLocationDataSource);
  m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Recenter);
  m_mapView->locationDisplay()->setInitialZoomScale(initialZoomScale);

  // if tracking is enabled then show location history
  connect(m_mapView->locationDisplay(), &LocationDisplay::locationChanged, this, [this](const Location& location)
  {
    if (!m_trackingEnabled)
    {
      return;
    }

    // clear old route
    m_locationHistoryLineOverlay->graphics()->clear();

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
