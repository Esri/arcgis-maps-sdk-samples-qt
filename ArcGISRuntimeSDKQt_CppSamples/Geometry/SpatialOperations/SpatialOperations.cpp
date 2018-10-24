// [WriteFile Name=SpatialOperations, Category=Geometry]
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

#include "SpatialOperations.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "Point.h"
#include "Part.h"
#include "PolygonBuilder.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "GeometryEngine.h"

using namespace Esri::ArcGISRuntime;

SpatialOperations::SpatialOperations(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_polygon1(Geometry()),
  m_polygon2(Geometry()),
  m_inputsOverlay (new GraphicsOverlay(this)),
  m_outputsOverlay (new GraphicsOverlay(this)),
  m_geometryOperations { QStringLiteral("None"),       QStringLiteral("Union")
                       , QStringLiteral("Difference"), QStringLiteral("Symmetric difference")
                       , QStringLiteral("Intersection") }
{
  
}

void SpatialOperations::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<SpatialOperations>("Esri.Samples", 1, 0, "SpatialOperationsSample");
}

void SpatialOperations::componentComplete()
{
  QQuickItem::componentComplete();

  m_map = new Map(Basemap::topographic(this), this);
  m_mapView->setMap(m_map);

  addPolygons();
  m_mapView->graphicsOverlays()->append(m_inputsOverlay);
  m_mapView->graphicsOverlays()->append(m_outputsOverlay);
  m_map->setInitialViewpoint(Viewpoint(Point(-13453, 6710127, SpatialReference::webMercator()), 30000));
}
void SpatialOperations::addPolygons()
{
  // create blue polygon
  PolygonBuilder polygonBuilder1(SpatialReference::webMercator());
  polygonBuilder1.addPoint(-13960, 6709400);
  polygonBuilder1.addPoint(-14660, 6710000);
  polygonBuilder1.addPoint(-13760, 6710730);
  polygonBuilder1.addPoint(-13300, 6710500);
  polygonBuilder1.addPoint(-13160, 6710100);

  SimpleFillSymbol* fillSymbol1 = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("blue"), this);
  m_polygon1 = polygonBuilder1.toGeometry();
  m_inputsOverlay->graphics()->append(new Graphic(m_polygon1, fillSymbol1, this));

  // create green polygon
  // outer ring
  Part* outerRing = new Part(SpatialReference::webMercator(), this);
  outerRing->addPoint(-13060, 6711030);
  outerRing->addPoint(-12160, 6710730);
  outerRing->addPoint(-13160, 6709700);
  outerRing->addPoint(-14560, 6710730);
  outerRing->addPoint(-13060, 6711030);

  // inner ring
  Part* innerRing = new Part(SpatialReference::webMercator(), this);
  innerRing->addPoint(-13060, 6710910);
  innerRing->addPoint(-14160, 6710630);
  innerRing->addPoint(-13160, 6709900);
  innerRing->addPoint(-12450, 6710660);
  innerRing->addPoint(-13060, 6710910);

  PolygonBuilder polygonBuilder2(SpatialReference::webMercator());
  polygonBuilder2.parts()->addPart(outerRing);
  polygonBuilder2.parts()->addPart(innerRing);
  m_polygon2 = polygonBuilder2.toGeometry();
  SimpleFillSymbol* fillSymbol2 = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("green"), this);
  m_inputsOverlay->graphics()->append(new Graphic(m_polygon2, fillSymbol2, this));
}

MapQuickView* SpatialOperations::mapQuickView() const
{
  return m_mapView;
}

void SpatialOperations::setMapQuickView(MapQuickView* mapQuickView)
{
  m_mapView = mapQuickView;
  emit mapQuickViewChanged();
}

void SpatialOperations::applyGeometryOperation(int index)
{
  if (!m_map || m_map->loadStatus() != LoadStatus::Loaded)
    return;

  // Perform geometry calculations
  Geometry resultPolygon;
  switch (index) {
  case 1:
    resultPolygon = GeometryEngine::unionOf(m_polygon1, m_polygon2);
    break;
  case 2:
    resultPolygon = GeometryEngine::difference(m_polygon1, m_polygon2);
    break;
  case 3:
    resultPolygon = GeometryEngine::symmetricDifference(m_polygon1, m_polygon2);
    break;
  case 4:
    resultPolygon = GeometryEngine::intersection(m_polygon1, m_polygon2);
    break;
  case 0:
  default:
    break;
  }

  // Clear previous results
  m_outputsOverlay->graphics()->clear();
  if (resultPolygon.isEmpty())
    return;

  // Add the resulting polygon as a Graphic
  SimpleFillSymbol* fillSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("red"), this);
  Graphic* graphic = new Graphic(resultPolygon, fillSymbol, this);
  m_outputsOverlay->graphics()->append(graphic);
}
