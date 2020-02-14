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

#include "GeometryEngine.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "Map.h"
#include "MapQuickView.h"
#include "PolygonBuilder.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"

#include <QLabel>

using namespace Esri::ArcGISRuntime;

NearestVertex::NearestVertex(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::topographic(this), this))
{

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

// Set the view (created in QML)
void NearestVertex::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

//  QLabel* label = new QLabel();
//  label->setText("hello");

  // create a graphics overlay to show the polygon, clicked location, and nearest vertex
  GraphicsOverlay* graphicsOverlay = new GraphicsOverlay(this);

  QList<Point> points = {
    Point(-5991501.677830, 5599295.131468),
    Point(-6928550.398185, 2087936.739807),
    Point(-3149463.800709, 1840803.011362),
    Point(-1563689.043184, 3714900.452072),
    Point(-3180355.516764, 5619889.608838)
  };

  PolygonBuilder* polygonBuilder = new PolygonBuilder(SpatialReference::webMercator(), this);
  polygonBuilder->addPoints(points);
  SimpleLineSymbol* polygonOutlineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(0xFF00FF00), 2, this);
  SimpleFillSymbol* polygonFillSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::ForwardDiagonal, QColor(0xFF00FF00), polygonOutlineSymbol, this);
  Graphic* polygonGraphic = new Graphic(polygonBuilder->toGeometry(), polygonFillSymbol, this);

  graphicsOverlay->graphics()->append(polygonGraphic);

  // create graphics for the clicked location, nearest coordinate, and nearest vertex markers
  SimpleMarkerSymbol* clickedLocationSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::X, QColor(0xFFFFA500), 15, this);
  SimpleMarkerSymbol* nearestCoordinateSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, QColor(0xFFFF0000), 10, this);
  SimpleMarkerSymbol* nearestVertexSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor(0xFF0000FF), 15, this);

  Graphic* clickedLocationGraphic = new Graphic(this);
  clickedLocationGraphic->setSymbol(clickedLocationSymbol);
  Graphic* nearestCoordinateGraphic = new Graphic(this);
  nearestCoordinateGraphic->setSymbol(nearestCoordinateSymbol);
  Graphic* nearestVertexGraphic = new Graphic(this);
  nearestVertexGraphic->setSymbol(nearestVertexSymbol);


  // add graphic to clicked location
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this, polygonBuilder, graphicsOverlay, clickedLocationGraphic, nearestCoordinateGraphic, nearestVertexGraphic](QMouseEvent& e)
  {
    Point clickedLocation = m_mapView->screenToLocation(e.x(), e.y());
    clickedLocationGraphic->setGeometry(clickedLocation);
    graphicsOverlay->graphics()->append(clickedLocationGraphic);

    ProximityResult nearestCoordinateResult = GeometryEngine::nearestCoordinate(polygonBuilder->toGeometry(), clickedLocation);
    nearestCoordinateGraphic->setGeometry(nearestCoordinateResult.coordinate());
    graphicsOverlay->graphics()->append(nearestCoordinateGraphic);

    ProximityResult nearestVertexResult = GeometryEngine::nearestVertex(polygonBuilder->toGeometry(), clickedLocation);
    nearestVertexGraphic->setGeometry(nearestVertexResult.coordinate());
    graphicsOverlay->graphics()->append(nearestVertexGraphic);
  });

  m_mapView->graphicsOverlays()->append(graphicsOverlay);

  emit mapViewChanged();
}
