// [WriteFile Name=GORenderers, Category=DisplayInformation]
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

#include "GORenderers.h"

#include "CubicBezierSegment.h"
#include "EllipticArcSegment.h"
#include "ImmutablePart.h"
#include "Map.h"
#include "MapQuickView.h"
#include "SpatialReference.h"
#include "Point.h"
#include "PolylineBuilder.h"
#include "Polyline.h"
#include "PolygonBuilder.h"
#include "Polygon.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleFillSymbol.h"
#include "SimpleRenderer.h"

using namespace Esri::ArcGISRuntime;

GORenderers::GORenderers(QQuickItem* parent) :
  QQuickItem(parent)
{
}

GORenderers::~GORenderers() = default;

void GORenderers::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<GORenderers>("Esri.Samples", 1, 0, "GORenderersSample");
}

void GORenderers::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the topo basemap
  m_map = new Map(BasemapStyle::ArcGISTopographic, this);
  // set map on the map view
  m_mapView->setMap(m_map);

  addGraphicsOverlay();
}

void GORenderers::addGraphicsOverlay()
{
  // create the point geometry
  Point pointGeometry(40e5, 40e5, SpatialReference::webMercator());
  // create the point symbol
  SimpleMarkerSymbol* pointSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, QColor("red"), 10, this);
  // Point graphic
  Graphic* pointGraphic = new Graphic(pointGeometry, this);

  // create a graphic overlay to display the point graphic
  GraphicsOverlay* pointGraphicOverlay = new GraphicsOverlay(this);
  // set the renderer of the overlay to be the marker symbol
  pointGraphicOverlay->setRenderer(new SimpleRenderer(pointSymbol, this));
  // add the graphic to the overlay
  pointGraphicOverlay->graphics()->append(pointGraphic);
  // add the overlay to the mapview
  m_mapView->graphicsOverlays()->append(pointGraphicOverlay);

  // create line geometry
  PolygonBuilder polylineBuilder(SpatialReference::webMercator());
  // build the polyline
  polylineBuilder.addPoint(-10e5, 40e5);
  polylineBuilder.addPoint(20e5, 50e5);
  // create a line symbol
  SimpleLineSymbol* sls = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("blue"), 5, this);
  // create a line graphic
  Graphic* lineGraphic = new Graphic(polylineBuilder.toGeometry(), this);

  // create a graphic overlay to display the line graphic
  GraphicsOverlay* lineGraphicOverlay = new GraphicsOverlay(this);
  // set the renderer of the graphic overlay to be the line symbol
  lineGraphicOverlay->setRenderer(new SimpleRenderer(sls, this));
  // add the graphic to the overlay
  lineGraphicOverlay->graphics()->append(lineGraphic);
  // add the overlay to the mapview
  m_mapView->graphicsOverlays()->append(lineGraphicOverlay);

  // create the polygon geometry
  PolygonBuilder polygonBuilder(SpatialReference::webMercator());
  polygonBuilder.addPoint(-20e5, 20e5);
  polygonBuilder.addPoint(20e5, 20e5);
  polygonBuilder.addPoint(20e5, -20e5);
  polygonBuilder.addPoint(-20e5, -20e5);
  SimpleFillSymbol* sfs = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor(255, 255, 0, 180), this);
  Graphic* polygonGraphic = new Graphic(polygonBuilder.toGeometry(), this);

  GraphicsOverlay* polygonGraphicsOverlay = new GraphicsOverlay(this);
  polygonGraphicsOverlay->setRenderer(new SimpleRenderer(sfs, this));
  polygonGraphicsOverlay->graphics()->append(polygonGraphic);
  m_mapView->graphicsOverlays()->append(polygonGraphicsOverlay);

  // create the curve geometry
  Point origin(40e5, 5e5, 0, SpatialReference::webMercator());
  int sideLength = 10e5;
  Geometry heartShape = createHeart(origin, sideLength);
  Graphic* curveGraphic = new Graphic(heartShape);
  SimpleLineSymbol* curveSls = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("black"), 1, this);
  SimpleFillSymbol* curveSfs = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("red"), curveSls, this);


  // create a graphic overlay to display the curve graphic
  GraphicsOverlay* curveGraphicsOverlay = new GraphicsOverlay(this);
  curveGraphicsOverlay->setRenderer(new SimpleRenderer(curveSfs, this));
  curveGraphicsOverlay->graphics()->append(curveGraphic);
  m_mapView->graphicsOverlays()->append(curveGraphicsOverlay);
}

Geometry GORenderers::createHeart(Point origin, int sideLength)
{
  SpatialReference spatialReference = origin.spatialReference();

  // Create variables to store the min and max X and Y coordinates
  int minX = origin.x() - 0.5 * sideLength;
  int minY = origin.y() - 0.5 * sideLength;

  // Create variable to store radius of the arcs used to create the heart
  int arcRadius = sideLength * 0.25;

  // Create bottom left curve
  Point leftCurveStart(origin.x(), minY, spatialReference);
  Point leftCurveEnd(minX, minY + 0.75 * sideLength, spatialReference);
  Point leftControlPoint1(origin.x(), minY + 0.25 * sideLength, spatialReference);
  Point leftControlPoint2(minX, origin.y(), spatialReference);
  CubicBezierSegment leftCurve(leftCurveStart, leftControlPoint1, leftControlPoint2, leftCurveEnd, spatialReference);

  // Create bottom right curve
  Point rightCurveStart(minX + sideLength, minY + 0.75 * sideLength, spatialReference);
  Point rightCurveEnd = leftCurveStart;
  Point rightControlPoint1(minX + sideLength, origin.y(), spatialReference);
  Point rightControlPoint2 = leftControlPoint1;
  CubicBezierSegment rightCurve(rightCurveStart, rightControlPoint1, rightControlPoint2, rightCurveEnd, spatialReference);

  // Create top left arc
  Point arcIntersection(origin.x(), minY + 0.75 * sideLength);
  Point leftArcCentre(minX + 0.25 * sideLength, minY + 0.75 * sideLength, spatialReference);
  EllipticArcSegment leftArc(leftCurveEnd, arcIntersection, 0, false, false, arcRadius, 1, spatialReference);

  // Create top right arc
  Point rightArcCentre(minX + sideLength, minY + 0.75 * sideLength, spatialReference);
  EllipticArcSegment rightArc(arcIntersection, rightCurveStart, 0, false, false, arcRadius, 1, spatialReference);

  Part* heart = new Part(spatialReference, this);
  heart->addSegment(leftCurve);
  heart->addSegment(leftArc);
  heart->addSegment(rightArc);
  heart->addSegment(rightCurve);

  PartCollection* partCollection = new PartCollection(spatialReference, this);
  partCollection->addPart(heart);

  PolygonBuilder* heartBuilder = new PolygonBuilder(origin.spatialReference());
  heartBuilder->setParts(partCollection);

  return heartBuilder->toGeometry();
}


