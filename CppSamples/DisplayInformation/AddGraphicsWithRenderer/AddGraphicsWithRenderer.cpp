// [WriteFile Name=AddGraphicsWithRenderer, Category=DisplayInformation]
// [Legal]
// Copyright 2022 Esri.
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
#include "AddGraphicsWithRenderer.h"

// ArcGIS Maps SDK headers
#include "AngularUnit.h"
#include "CubicBezierSegment.h"
#include "EllipticArcSegment.h"
#include "GeodesicEllipseParameters.h"
#include "GeometryEngine.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "LinearUnit.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Part.h"
#include "PartCollection.h"
#include "Point.h"
#include "Polygon.h"
#include "PolygonBuilder.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleRenderer.h"
#include "SpatialReference.h"
#include "SymbolTypes.h"

using namespace Esri::ArcGISRuntime;

AddGraphicsWithRenderer::AddGraphicsWithRenderer(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISTopographic, this))
{
}

AddGraphicsWithRenderer::~AddGraphicsWithRenderer() = default;

void AddGraphicsWithRenderer::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<AddGraphicsWithRenderer>("Esri.Samples", 1, 0, "AddGraphicsWithRendererSample");
}

MapQuickView* AddGraphicsWithRenderer::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void AddGraphicsWithRenderer::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();

  addGraphicsOverlays();
}

void AddGraphicsWithRenderer::addGraphicsOverlays()
{
  addPointGraphic();
  addLineGraphic();
  addPolygonGraphic();
  addCurveGraphic();
  addEllipseGraphic();
}

void AddGraphicsWithRenderer::createGraphicsOverlayWithGraphicAndSymbol(Esri::ArcGISRuntime::Graphic* graphic, Esri::ArcGISRuntime::Symbol* symbol)
{
  GraphicsOverlay* graphicOverlay = new GraphicsOverlay(this);
  // set the renderer of the overlay to be the symbol
  graphicOverlay->setRenderer(new SimpleRenderer(symbol, this));
  // add the graphic to the overlay
  graphicOverlay->graphics()->append(graphic);
  // add the overlay to the mapview
  m_mapView->graphicsOverlays()->append(graphicOverlay);
}

void AddGraphicsWithRenderer::addPointGraphic()
{
  const Point pointGeometry(40e5, 40e5, SpatialReference::webMercator());

  SimpleMarkerSymbol* pointSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, QColor("red"), 10, this);

  Graphic* pointGraphic = new Graphic(pointGeometry, this);

  createGraphicsOverlayWithGraphicAndSymbol(pointGraphic, pointSymbol);
}

void AddGraphicsWithRenderer::addLineGraphic()
{
  PolygonBuilder polylineBuilder(SpatialReference::webMercator());
  polylineBuilder.addPoint(-10e5, 40e5);
  polylineBuilder.addPoint(20e5, 50e5);

  SimpleLineSymbol* lineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("blue"), 5, this);

  Graphic* lineGraphic = new Graphic(polylineBuilder.toGeometry(), this);

  createGraphicsOverlayWithGraphicAndSymbol(lineGraphic, lineSymbol);
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

  createGraphicsOverlayWithGraphicAndSymbol(polygonGraphic, polygonFillSymbol);
}

void AddGraphicsWithRenderer::addCurveGraphic()
{
  Geometry heartShapedCurve = createHeart();

  Graphic* curveGraphic = new Graphic(heartShapedCurve, this);

  SimpleLineSymbol* curveLineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("black"), 1, this);
  SimpleFillSymbol* curveFillSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("red"), curveLineSymbol, this);

  createGraphicsOverlayWithGraphicAndSymbol(curveGraphic, curveFillSymbol);
}

void AddGraphicsWithRenderer::addEllipseGraphic()
{
  GeodesicEllipseParameters parameters = GeodesicEllipseParameters();
  parameters.setCenter(Point(40e5, 25e5, SpatialReference::webMercator()));
  parameters.setGeometryType(GeometryType::Polygon);
  parameters.setSemiAxis1Length(200);
  parameters.setSemiAxis2Length(400);
  parameters.setAxisDirection(-45);
  parameters.setMaxPointCount(100);
  parameters.setAngularUnit(AngularUnit(AngularUnitId::Degrees));
  parameters.setLinearUnit(LinearUnit(LinearUnitId::Kilometers));
  parameters.setMaxSegmentLength(20);

  const Polygon ellipsePolygon = geometry_cast<Polygon>(GeometryEngine::ellipseGeodesic(parameters));

  SimpleFillSymbol* ellipseSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor(125, 0, 125), this);

  createGraphicsOverlayWithGraphicAndSymbol(new Graphic(ellipsePolygon, this), ellipseSymbol);
}

Geometry AddGraphicsWithRenderer::createHeart()
{
  // Declare common variables
  const Point origin(40e5, 5e5, 0, SpatialReference::webMercator());
  SpatialReference spatialReference = origin.spatialReference();
  int sideLength = 10e5;
  int minX = origin.x() - 0.5 * sideLength;
  int minY = origin.y() - 0.5 * sideLength;
  int arcRadius = sideLength * 0.25;

  // The heart-shape comprises 4-curve segments: 2 bezier curves make the base and 2 arc curves make the top
  // Declare all points used to create heart-shaped curve, starting at bottom tip and moving clockwise
  const Point bottomTip(origin.x(), minY, spatialReference);
  const Point leftControlPoint1(origin.x(), minY + 0.25 * sideLength, spatialReference);
  const Point leftControlPoint2(minX, origin.y(), spatialReference);
  const Point leftCurveEnd(minX, minY + 0.75 * sideLength, spatialReference);
  const Point leftArcCentre(minX + 0.25 * sideLength, minY + 0.75 * sideLength, spatialReference);
  const Point arcIntersection(origin.x(), minY + 0.75 * sideLength, spatialReference);
  const Point rightArcCentre(minX + sideLength, minY + 0.75 * sideLength, spatialReference);
  const Point rightCurveStart(minX + sideLength, minY + 0.75 * sideLength, spatialReference);
  const Point rightControlPoint1(minX + sideLength, origin.y(), spatialReference);
  const Point rightControlPoint2 = leftControlPoint1;

  // Create curve segments
  CubicBezierSegment leftCurve(bottomTip, leftControlPoint1, leftControlPoint2, leftCurveEnd, spatialReference);
  EllipticArcSegment leftArc(leftCurveEnd, arcIntersection, 0, false, false, arcRadius, 1, spatialReference);
  EllipticArcSegment rightArc(arcIntersection, rightCurveStart, 0, false, false, arcRadius, 1, spatialReference);
  CubicBezierSegment rightCurve(rightCurveStart, rightControlPoint1, rightControlPoint2, bottomTip, spatialReference);

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
  PolygonBuilder* heartBuilder = new PolygonBuilder(origin.spatialReference(), this);
  heartBuilder->setParts(partCollection);

  return heartBuilder->toGeometry();
}
