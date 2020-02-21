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
#include "Multipoint.h"
#include "MultipointBuilder.h"
#include "PointCollection.h"
#include "SimpleFillSymbol.h"
#include "SimpleMarkerSymbol.h"

#include <QList>

using namespace Esri::ArcGISRuntime;

ConvexHull::ConvexHull(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::imagery(this), this))
{

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
  qDebug() << "clicked!";
  return;

}

// Set the view (created in QML)
void ConvexHull::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  // graphics overlay to show clicked points and convex hull
  GraphicsOverlay* graphicsOverlay = new GraphicsOverlay(this);

  // create a graphic to show clicked points
  SimpleMarkerSymbol* simpleMarkerSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, Qt::red, 10, this);
  Graphic* inputsGraphic = new Graphic(this);
  inputsGraphic->setSymbol(simpleMarkerSymbol);
  graphicsOverlay->graphics()->append(inputsGraphic);

  // create a graphic to show the convex hull
  SimpleLineSymbol* lineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::blue, 3, this);
  SimpleFillSymbol* fillSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Null, Qt::transparent, lineSymbol, this);
  Graphic* convexHullGraphic = new Graphic(this);
  graphicsOverlay->graphics()->append(convexHullGraphic);

  // list of Points clicked by user
//  QList<Point> inputs;

  connect(m_mapView, &MapQuickView::mouseClicked, this, [inputsGraphic, this](QMouseEvent& e){
    Point clickedPoint = m_mapView->screenToLocation(e.x(), e.y());
    m_inputs.push_back(clickedPoint);
    inputsGraphic->setGeometry(clickedPoint);

    PointCollection* pointCollection = new PointCollection(m_mapView->spatialReference(), this);
    pointCollection->addPoints(m_inputs);
    MultipointBuilder* multipointBuilder = new MultipointBuilder(m_mapView->spatialReference(), this);
    multipointBuilder->setPoints(pointCollection);
    Multipoint* inputsGeometry = new Multipoint(multipointBuilder->toGeometry());
    Multipoint blah(multipointBuilder->toGeometry());

    e.accept();
    qDebug() << m_inputs.length();
  });

  m_mapView->graphicsOverlays()->append(graphicsOverlay);
  emit mapViewChanged();
}
