// [WriteFile Name=NearestVertex, Category=Geometry]
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

#include "NearestVertex.h"

#include "FeatureLayer.h"
#include "GeometryEngine.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "Map.h"
#include "MapQuickView.h"
#include "PolygonBuilder.h"
#include "PortalItem.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "SpatialReference.h"
#include "GraphicsOverlayListModel.h"
#include "GraphicListModel.h"
#include "SymbolTypes.h"
#include "TaskWatcher.h"
#include "LayerListModel.h"
#include "ProximityResult.h"
#include "Basemap.h"
#include "Envelope.h"
#include "Point.h"

using namespace Esri::ArcGISRuntime;

namespace {
  const SpatialReference statePlaneCaliforniaZone5SpatialReference = SpatialReference(2229);
}

NearestVertex::NearestVertex(QObject* parent /* = nullptr */):
  QObject(parent)
{
  m_map = new Map(statePlaneCaliforniaZone5SpatialReference, this);

  PortalItem* portalItem = new PortalItem("99fd67933e754a1181cc755146be21ca", this);
  FeatureLayer* usStatesGeneralizedLayer = new FeatureLayer(portalItem, this);
  m_map->basemap()->baseLayers()->append(usStatesGeneralizedLayer);
}

NearestVertex::~NearestVertex() = default;

void NearestVertex::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<NearestVertex>("Esri.Samples", 1, 0, "NearestVertexSample");
}

MapQuickView* NearestVertex::mapView() const
{
  return m_mapView;
}

int NearestVertex::vertexDistance() const
{
  return m_vertexDistance;
}

int NearestVertex::coordinateDistance() const
{
  return m_coordinateDistance;
}

// Set the view (created in QML)
void NearestVertex::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  // shows the polygon, clicked location, and nearest coordinate and vertex on polygon
  setupGraphics();

  // Set viewpoint to the polygon graphic
  m_mapView->setViewpointCenter(m_mapView->graphicsOverlays()->first()->graphics()->first()->geometry().extent().center(), 8e6);

  emit mapViewChanged();
}

void NearestVertex::setupGraphics()
{
  // create a graphics overlay to show the polygon, clicked location, and nearest vertex
  GraphicsOverlay* graphicsOverlay = new GraphicsOverlay(this);

  // Construct a polygon from a point collection that uses the California zone 5 (ftUS) state plane coordinate system
  QList<Point> points = {
    Point(6627416.41469281, 1804532.53233782),
    Point(6669147.89779046, 2479145.16609522),
    Point(7265673.02678292, 2484254.50442408),
    Point(7676192.55880379, 2001458.66365744),
    Point(7175695.94143837, 1840722.34474458)
  };

  PolygonBuilder* polygonBuilder = new PolygonBuilder(statePlaneCaliforniaZone5SpatialReference, this);
  polygonBuilder->addPoints(points);
  SimpleLineSymbol* polygonOutlineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::green, 2, this);
  SimpleFillSymbol* polygonFillSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::ForwardDiagonal, Qt::green, polygonOutlineSymbol, this);
  Graphic* polygonGraphic = new Graphic(polygonBuilder->toGeometry(), polygonFillSymbol, this);

  graphicsOverlay->graphics()->append(polygonGraphic);

  // create graphics for the clicked location, nearest coordinate, and nearest vertex markers
  SimpleMarkerSymbol* clickedLocationSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::X, Qt::yellow, 15, this);
  SimpleMarkerSymbol* nearestCoordinateSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, Qt::red, 10, this);
  SimpleMarkerSymbol* nearestVertexSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, Qt::blue, 15, this);

  Graphic* clickedLocationGraphic = new Graphic(this);
  clickedLocationGraphic->setSymbol(clickedLocationSymbol);
  Graphic* nearestCoordinateGraphic = new Graphic(this);
  nearestCoordinateGraphic->setSymbol(nearestCoordinateSymbol);
  Graphic* nearestVertexGraphic = new Graphic(this);
  nearestVertexGraphic->setSymbol(nearestVertexSymbol);

  graphicsOverlay->graphics()->append(clickedLocationGraphic);
  graphicsOverlay->graphics()->append(nearestCoordinateGraphic);
  graphicsOverlay->graphics()->append(nearestVertexGraphic);

  // add graphic to clicked location
  connect(m_mapView, &MapQuickView::mouseClicked, this,
          [nearestVertexGraphic, nearestCoordinateGraphic, polygonBuilder, clickedLocationGraphic, this]
          (QMouseEvent& e)
  {
    const Point clickedLocation = m_mapView->screenToLocation(e.position().x(), e.position().y());
    // normalizing the geometry before performing geometric operations
    const Point normalizedPoint = geometry_cast<Point>(GeometryEngine::normalizeCentralMeridian(clickedLocation));

    clickedLocationGraphic->setGeometry(normalizedPoint);

    const ProximityResult nearestCoordinateResult = GeometryEngine::nearestCoordinate(polygonBuilder->toGeometry(), normalizedPoint);
    nearestCoordinateGraphic->setGeometry(nearestCoordinateResult.coordinate());

    const ProximityResult nearestVertexResult = GeometryEngine::nearestVertex(polygonBuilder->toGeometry(), normalizedPoint);
    nearestVertexGraphic->setGeometry(nearestVertexResult.coordinate());

    // get the distances to the nearest vertex and nearest coordinate, converted from feet to miles
    m_vertexDistance = nearestVertexResult.distance()/5280;
    m_coordinateDistance = nearestCoordinateResult.distance()/5280;

    e.accept();
    emit vertexDistanceCalculated();
    emit coordinateDistanceCalculated();
  });

  m_mapView->graphicsOverlays()->append(graphicsOverlay);
}
