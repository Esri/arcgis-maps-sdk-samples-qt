// [WriteFile Name=CreateGeometries, Category=Geometry]
// [Legal]
// Copyright 2018 Esri.

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

#include "CreateGeometries.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "Geometry.h"
#include "Multipoint.h"
#include "Point.h"
#include "Polygon.h"
#include "Polyline.h"
#include "PolygonBuilder.h"
#include "PolylineBuilder.h"
#include "MultipointBuilder.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"

using namespace Esri::ArcGISRuntime;

CreateGeometries::CreateGeometries(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void CreateGeometries::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<CreateGeometries>("Esri.Samples", 1, 0, "CreateGeometriesSample");
}

void CreateGeometries::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the topographic basemap
  m_map = new Map(Basemap::topographic(this), this);

  // Add a Graphics Overlay to the map view
  m_graphicsOverlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  // Add graphics to the overlay once the map loads
  connect(m_map, &Map::doneLoading, [this](Error e)
  {
    if (!e.isEmpty())
      return;

    addGraphics();
  });

  // Set map to map view
  m_mapView->setMap(m_map);    
}

// function to add Graphics to a Graphics Overlay
void CreateGeometries::addGraphics()
{
  // Create a simple fill symbol - used to render a polygon covering Colorado.
  SimpleFillSymbol* simpleFillSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Cross, QColor("blue"), this);

  // Create a simple line symbol - used to render a polyline border between California and Nevada.
  SimpleLineSymbol* simpleLineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("blue"), 3.0f /*width*/, this);

  // Create a simple marker symbol - used to render multi-points for various state capital locations in the Western United States.
  SimpleMarkerSymbol* simpleMarkerSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Triangle, QColor("blue"), 14.0f /*size*/, this);

  // Create a simple marker symbol - used to render a map point where the Esri headquarters is located.
  SimpleMarkerSymbol* simpleMarkerSymbol2 = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, QColor("red"), 18.0f /*size*/, this);

  // Add a graphic to the graphic collection - polygon with a simple fill symbol.
  m_graphicsOverlay->graphics()->append(new Graphic(createPolygon(), simpleFillSymbol, this));

  // Add a graphic to the graphic collection - polyline with a simple line symbol.
  m_graphicsOverlay->graphics()->append(new Graphic(createPolyline(), simpleLineSymbol, this));

  // Add a graphic to the graphic collection - map point with a simple marker symbol.
  m_graphicsOverlay->graphics()->append(new Graphic(createPoint(), simpleMarkerSymbol2, this));

  // Add a graphic to the graphic collection - multi-point with a simple marker symbol.
  m_graphicsOverlay->graphics()->append(new Graphic(createMultipoint(), simpleMarkerSymbol, this));

  // Zoom to the extent of an envelope with some padding (10 device-independent pixels).
  m_mapView->setViewpointGeometry(createEnvelope(), 10);
}

// Return an envelope that covers the extent of the western United States.
Geometry CreateGeometries::createEnvelope() const
{
  constexpr double xMin = -123.0;
  constexpr double yMin = 33.5;
  constexpr double xMax = -101.0;
  constexpr double yMax = 48.0;
  const SpatialReference spatialRef(4326);

  return Envelope(xMin, yMin, xMax, yMax, spatialRef);
}

Geometry CreateGeometries::createPolygon() const
{
  // Create a polygon builder
  const SpatialReference spatialRef(4326);
  PolygonBuilder polygonBuilder(spatialRef);

  // add points to the builder that approximates the boundary of Colorado.
  polygonBuilder.addPoint(-109.048, 40.998);
  polygonBuilder.addPoint(-102.047, 40.998);
  polygonBuilder.addPoint(-102.037, 36.989);
  polygonBuilder.addPoint(-109.048, 36.998);

  // Return the geometry.
  return polygonBuilder.toGeometry();
}

Geometry CreateGeometries::createPolyline() const
{
  // Create a polyline builder
  const SpatialReference spatialRef(4326);
  PolylineBuilder polylineBuilder(spatialRef);

  // add points to the builder that approximates the border between California and Nevada.
  polylineBuilder.addPoint(-119.992, 41.989);
  polylineBuilder.addPoint(-119.994, 38.994);
  polylineBuilder.addPoint(-114.620, 35.0);

  // Return the geometry.
  return polylineBuilder.toGeometry();
}

Geometry CreateGeometries::createPoint() const
{
  constexpr double x = -117.195800;
  constexpr double y = 34.056295;
  const SpatialReference spatialRef(4326);

  // Return a map point where the Esri headquarters is located.
  return Point(x, y, spatialRef);
}

Geometry CreateGeometries::createMultipoint() const
{
  // Create a polygon builder
  const SpatialReference spatialRef(4326);
  MultipointBuilder multipointBuilder(spatialRef);

  // add points to the builder representing various state capital locations in the Western United States.
  multipointBuilder.points()->addPoint(-121.491014, 38.579065); // - Sacramento, CA
  multipointBuilder.points()->addPoint(-122.891366, 47.039231); // - Olympia, WA
  multipointBuilder.points()->addPoint(-123.043814, 44.93326); // - Salem, OR
  multipointBuilder.points()->addPoint(-119.766999, 39.164885); // - Carson City, NV

  // Return the geometry.
  return multipointBuilder.toGeometry();
}
