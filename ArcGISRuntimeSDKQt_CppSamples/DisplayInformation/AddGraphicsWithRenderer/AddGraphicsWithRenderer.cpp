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

#include "AddGraphicsWithRenderer.h"

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

AddGraphicsWithRenderer::AddGraphicsWithRenderer(QQuickItem* parent) :
  QQuickItem(parent)
{
}

AddGraphicsWithRenderer::~AddGraphicsWithRenderer() = default;

void AddGraphicsWithRenderer::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<AddGraphicsWithRenderer>("Esri.Samples", 1, 0, "AddGraphicsWithRendererSample");
}

void AddGraphicsWithRenderer::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the topo basemap
  m_map = new Map(BasemapStyle::ArcGISTopographic, this);
  // set map on the map view
  m_mapView->setMap(m_map);

  addGraphicsOverlays();
}

void AddGraphicsWithRenderer::addGraphicsOverlays()
{
  addPointGraphic();

  addLineGraphic();

  addPolygonGraphic();

  addCurveGraphic();
}

void AddGraphicsWithRenderer::addPointGraphic()
{
  Point pointGeometry(40e5, 40e5, SpatialReference::webMercator());

  SimpleMarkerSymbol* pointSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, QColor("red"), 10, this);

  Graphic* pointGraphic = new Graphic(pointGeometry, this);

  createGraphicOverlayWithGraphicAndSymbol(pointGraphic, pointSymbol);
}

void AddGraphicsWithRenderer::addLineGraphic()
{
  PolygonBuilder polylineBuilder(SpatialReference::webMercator());
  polylineBuilder.addPoint(-10e5, 40e5);
  polylineBuilder.addPoint(20e5, 50e5);

  SimpleLineSymbol* lineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("blue"), 5, this);

  Graphic* lineGraphic = new Graphic(polylineBuilder.toGeometry(), this);

  createGraphicOverlayWithGraphicAndSymbol(lineGraphic, lineSymbol);
}

void AddGraphicsWithRenderer::addPolygonGraphic()
{
  PolygonBuilder polygonBuilder(SpatialReference::webMercator());
  polygonBuilder.addPoint(-20e5, 20e5);
  polygonBuilder.addPoint(20e5, 20e5);
  polygonBuilder.addPoint(20e5, -20e5);
  polygonBuilder.addPoint(-20e5, -20e5);

  SimpleFillSymbol* polygonFillSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor(255, 255, 0, 180), this);

  Graphic* polygonGraphic = new Graphic(polygonBuilder.toGeometry(), this);

  createGraphicOverlayWithGraphicAndSymbol(polygonGraphic, polygonFillSymbol);
}

void AddGraphicsWithRenderer::addCurveGraphic()
{
  Geometry heartShapedCurve = createHeart();

  Graphic* curveGraphic = new Graphic(heartShapedCurve);

  SimpleLineSymbol* curveLineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("black"), 1, this);
  SimpleFillSymbol* curveFillSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("red"), curveLineSymbol, this);

  createGraphicOverlayWithGraphicAndSymbol(curveGraphic, curveFillSymbol);
}

void AddGraphicsWithRenderer::createGraphicOverlayWithGraphicAndSymbol(Graphic* graphic, Symbol* symbol)
{
  GraphicsOverlay* graphicOverlay = new GraphicsOverlay(this);
  // set the renderer of the overlay to be the marker symbol
  graphicOverlay->setRenderer(new SimpleRenderer(symbol, this));
  // add the graphic to the overlay
  graphicOverlay->graphics()->append(graphic);
  // add the overlay to the mapview
  m_mapView->graphicsOverlays()->append(graphicOverlay);
}


Geometry AddGraphicsWithRenderer::createHeart()
{
  // Declare common variables
  Point origin(40e5, 5e5, 0, SpatialReference::webMercator());
  SpatialReference spatialReference = origin.spatialReference();
  int sideLength = 10e5;
  int minX = origin.x() - 0.5 * sideLength;
  int minY = origin.y() - 0.5 * sideLength;
  int arcRadius = sideLength * 0.25;

  // The heart-shape comprises 4-curve segments: 2 bezier curves make the base and 2 arc curves make the top
  // Declare all points used to create heart-shaped curve, starting at bottom tip and moving clockwise
  Point bottomTip(origin.x(), minY, spatialReference);
  Point leftControlPoint1(origin.x(), minY + 0.25 * sideLength, spatialReference);
  Point leftControlPoint2(minX, origin.y(), spatialReference);
  Point leftCurveEnd(minX, minY + 0.75 * sideLength, spatialReference);
  Point leftArcCentre(minX + 0.25 * sideLength, minY + 0.75 * sideLength, spatialReference);
  Point arcIntersection(origin.x(), minY + 0.75 * sideLength);
  Point rightArcCentre(minX + sideLength, minY + 0.75 * sideLength, spatialReference);
  Point rightCurveStart(minX + sideLength, minY + 0.75 * sideLength, spatialReference);
  Point rightControlPoint1(minX + sideLength, origin.y(), spatialReference);
  Point rightControlPoint2 = leftControlPoint1;

  // Create curve segments
  CubicBezierSegment leftCurve(bottomTip, leftControlPoint1, leftControlPoint2, leftCurveEnd, spatialReference);
  CubicBezierSegment rightCurve(rightCurveStart, rightControlPoint1, rightControlPoint2, bottomTip, spatialReference);
  EllipticArcSegment leftArc(leftCurveEnd, arcIntersection, 0, false, false, arcRadius, 1, spatialReference);
  EllipticArcSegment rightArc(arcIntersection, rightCurveStart, 0, false, false, arcRadius, 1, spatialReference);

  // Create part from segments
  Part* heart = new Part(spatialReference, this);
  heart->addSegment(leftCurve);
  heart->addSegment(leftArc);
  heart->addSegment(rightArc);
  heart->addSegment(rightCurve);

  // Create part collection
  PartCollection* partCollection = new PartCollection(spatialReference, this);
  partCollection->addPart(heart);

  // Construct polygon from part collection
  PolygonBuilder* heartBuilder = new PolygonBuilder(origin.spatialReference());
  heartBuilder->setParts(partCollection);

  return heartBuilder->toGeometry();
}
