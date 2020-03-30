// [WriteFile Name=SpatialRelationships, Category=Geometry]
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

#include "SpatialRelationships.h"

#include "Map.h"
#include "MapQuickView.h"
#include "GraphicsOverlay.h"
#include "Graphic.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "Point.h"
#include "PolylineBuilder.h"
#include "Polyline.h"
#include "PolygonBuilder.h"
#include "Polygon.h"
#include "GeometryEngine.h"
#include <QStringList>
#include <memory>

using namespace Esri::ArcGISRuntime;

SpatialRelationships::SpatialRelationships(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void SpatialRelationships::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<SpatialRelationships>("Esri.Samples", 1, 0, "SpatialRelationshipsSample");
}

void SpatialRelationships::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setSelectionProperties(SelectionProperties(QColor(Qt::yellow)));

  // Create a map using the topographic basemap
  m_map = new Map(Basemap::topographic(this), this);

  // Set map to map view
  m_mapView->setMap(m_map);

  // Create GraphicsOverlay
  m_graphicsOverlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  // Add Graphics
  addGraphics();

  // Set viewpoint
  m_mapView->setViewpointCenter(m_pointGraphic->geometry(), 200000000);

  // connect signals
  connectSignals();
}

void SpatialRelationships::addGraphics()
{
  addPolygonGraphic();
  addPolylineGraphic();
  addPointGraphic();
}

void SpatialRelationships::addPolygonGraphic()
{
  // create polygon geometry
  PolygonBuilder polyBuilder(SpatialReference::webMercator());
  polyBuilder.addPoint(-5991501.677830, 5599295.131468);
  polyBuilder.addPoint(-6928550.398185, 2087936.739807);
  polyBuilder.addPoint(-3149463.800709, 1840803.011362);
  polyBuilder.addPoint(-1563689.043184, 3714900.452072);
  polyBuilder.addPoint(-3180355.516764, 5619889.608838);
  Polygon geom = polyBuilder.toPolygon();

  // create symbol
  SimpleLineSymbol* outline = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("green"), 2.0f /*width*/, this);
  SimpleFillSymbol* sfs = new SimpleFillSymbol(SimpleFillSymbolStyle::ForwardDiagonal, QColor("green"), outline, this);

  // create graphic
  m_polygonGraphic = new Graphic(geom, sfs);
  m_graphicsOverlay->graphics()->append(m_polygonGraphic);
}

void SpatialRelationships::addPolylineGraphic()
{
  // create poyline geometry
  PolylineBuilder polyBuilder(SpatialReference::webMercator());
  polyBuilder.addPoint(-4354240.726880, -609939.795721);
  polyBuilder.addPoint(-3427489.245210, 2139422.933233);
  polyBuilder.addPoint(-2109442.693501, 4301843.057130);
  polyBuilder.addPoint(-1810822.771630, 7205664.366363);
  Polyline geom = polyBuilder.toPolyline();

  // create symbol
  SimpleLineSymbol* sls = new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, QColor("red"), 4.0f /*width*/, this);

  // create graphic
  m_polylineGraphic = new Graphic(geom, sls);
  m_graphicsOverlay->graphics()->append(m_polylineGraphic);
}

void SpatialRelationships::addPointGraphic()
{
  // Create point geometry
  Point geom(-4487263.495911, 3699176.480377, SpatialReference::webMercator());

  // create symbol
  SimpleMarkerSymbol* sms = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("blue"), 10.0f /*size*/, this);

  // create graphic
  m_pointGraphic = new Graphic(geom, sms, this);
  m_graphicsOverlay->graphics()->append(m_pointGraphic);
}


void SpatialRelationships::connectSignals()
{
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    // identify graphics
    m_mapView->identifyGraphicsOverlay(m_graphicsOverlay, mouseEvent.x(), mouseEvent.y(), 1.0 /*tolerance*/, false /*returnPopupsOnly*/);
  });

  connect(m_mapView, &MapQuickView::identifyGraphicsOverlayCompleted, this, [this](QUuid, IdentifyGraphicsOverlayResult* rawResult)
  {
    // Delete rawReslt when we leave scope.
    auto result = std::unique_ptr<IdentifyGraphicsOverlayResult>(rawResult);
    
    const QList<Graphic*> identifiedGraphics = result->graphics();
    if (identifiedGraphics.isEmpty())
      return;

    // get the first identified graphic
    Graphic* graphic = identifiedGraphics.at(0);

    // select the graphic
    m_graphicsOverlay->clearSelection();
    graphic->setSelected(true);

    // get the geometry
    const Geometry selectedGeometry = graphic->geometry();
    const GeometryType selectedGeometryType = selectedGeometry.geometryType();

    // reset the output text
    m_pointRelationships = "";
    m_polylineRelationships = "";
    m_polygonRelationships = "";

    // populate the view with the spatial relationships the selected graphic has to the other graphics
    // ignore testing relationships between the geometry and itself
    if (selectedGeometryType != GeometryType::Point)
    {
      const QString pointRelationships = getSpatialRelationships(selectedGeometry, m_pointGraphic->geometry()).join(",");
      m_pointRelationships = QString("Point: %1").arg(pointRelationships);
    }
    if (selectedGeometryType != GeometryType::Polyline)
    {
      const QString polylineRelationships = getSpatialRelationships(selectedGeometry, m_polylineGraphic->geometry()).join(",");
      m_polylineRelationships = QString("Polyline: %1").arg(polylineRelationships);
    }
    if (selectedGeometryType != GeometryType::Polygon)
    {
      const QString polygonRelationships = getSpatialRelationships(selectedGeometry, m_polygonGraphic->geometry()).join(",");
      m_polygonRelationships = QString("Polygon: %1").arg(polygonRelationships);
    }

    emit relationshipsChanged();
  });
}

// function to return list of relaionships
QStringList SpatialRelationships::getSpatialRelationships(const Geometry& geom1, const Geometry& geom2)
{
  QStringList relationships;
  if (GeometryEngine::crosses(geom1, geom2))
    relationships.append("CROSSES");
  if (GeometryEngine::contains(geom1, geom2))
    relationships.append("CONTAINS");
  if (GeometryEngine::disjoint(geom1, geom2))
    relationships.append("DISJOINT");
  if (GeometryEngine::intersects(geom1, geom2))
    relationships.append("INTERSECTS");
  if (GeometryEngine::overlaps(geom1, geom2))
    relationships.append("OVERLAPS");
  if (GeometryEngine::touches(geom1, geom2))
    relationships.append("TOUCHES");
  if (GeometryEngine::within(geom1, geom2))
    relationships.append("WITHIN");
  return relationships;
}
