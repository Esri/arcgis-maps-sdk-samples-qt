// [WriteFile Name=ConvexHull, Category=Geometry]
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

#include "ConvexHull.h"

#include "GeometryEngine.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MultipointBuilder.h"
#include "PointCollection.h"
#include "SimpleFillSymbol.h"
#include "SimpleMarkerSymbol.h"

using namespace Esri::ArcGISRuntime;

ConvexHull::ConvexHull(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::topographic(this), this))
{
  setupGraphics();
}

ConvexHull::~ConvexHull() = default;

void ConvexHull::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ConvexHull>("Esri.Samples", 1, 0, "ConvexHullSample");
}

MapQuickView* ConvexHull::mapView() const
{
  return m_mapView;
}

void ConvexHull::displayConvexHull()
{
  if (m_inputsGraphic->geometry().isEmpty())
    return;

  // normalizing the geometry before performing geometric operations
  const Geometry normalizedPoints = GeometryEngine::normalizeCentralMeridian(m_inputsGraphic->geometry());
  const Geometry convexHull = GeometryEngine::convexHull(normalizedPoints);

  // change the symbol based on the returned geometry type
  if (convexHull.geometryType() == GeometryType::Point)
  {
    m_convexHullGraphic->setSymbol(m_markerSymbol);
  }
  else if (convexHull.geometryType() == GeometryType::Polyline)
  {
    m_convexHullGraphic->setSymbol(m_lineSymbol);
  }
  else if (convexHull.geometryType() == GeometryType::Polygon)
  {
    m_convexHullGraphic->setSymbol(m_fillSymbol);
  }
  else
  {
    qWarning("Not a valid geometry.");
  }

  m_convexHullGraphic->setGeometry(convexHull);
}

void ConvexHull::clearGraphics()
{
  if (m_multipointBuilder)
    m_multipointBuilder->points()->removeAll();
  if (m_inputsGraphic)
    m_inputsGraphic->setGeometry(Geometry());
  if (m_convexHullGraphic)
    m_convexHullGraphic->setGeometry(Geometry());
}

void ConvexHull::setupGraphics()
{
  // graphics overlay to show clicked points and convex hull
  m_graphicsOverlay = new GraphicsOverlay(this);

  // create a graphic to show clicked points
  m_markerSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, Qt::red, 10, this);
  m_inputsGraphic = new Graphic(this);
  m_inputsGraphic->setSymbol(m_markerSymbol);
  m_graphicsOverlay->graphics()->append(m_inputsGraphic);

  // create a graphic to display the convex hull
  m_convexHullGraphic = new Graphic(this);
  m_graphicsOverlay->graphics()->append(m_convexHullGraphic);

  // create a graphic to show the convex hull
  m_lineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::blue, 3, this);
  m_fillSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Null, Qt::transparent, m_lineSymbol, this);
}

void ConvexHull::getInputs()
{
  // show clicked points on MapView
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& e)
  {
    e.accept();

    const Point clickedPoint = m_mapView->screenToLocation(e.x(), e.y());
    m_multipointBuilder->points()->addPoint(clickedPoint);
    m_inputsGraphic->setGeometry(m_multipointBuilder->toGeometry());
  });
}

// Set the view (created in QML)
void ConvexHull::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  // wait for map to load before creating multipoint builder
  connect(m_map, &Map::doneLoading, this, [this](Error e){
    if (!e.isEmpty())
    {
      qDebug() << e.message() << e.additionalMessage();
      return;
    }

    if (m_map->loadStatus() == LoadStatus::FailedToLoad)
    {
      qWarning( "Failed to load map.");
      return;
    }

    m_multipointBuilder = new MultipointBuilder(m_map->spatialReference(), this);
  });

  getInputs();

  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);
  emit mapViewChanged();
}
