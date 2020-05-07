// [WriteFile Name=Simple_Renderer, Category=DisplayInformation]
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

#include "Simple_Renderer.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Point.h"
#include "Viewpoint.h"
#include "GraphicsOverlay.h"
#include "Graphic.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleRenderer.h"
#include "SpatialReference.h"

using namespace Esri::ArcGISRuntime;

Simple_Renderer::Simple_Renderer(QQuickItem* parent) :
  QQuickItem(parent)
{
}

Simple_Renderer::~Simple_Renderer() = default;

void Simple_Renderer::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<Simple_Renderer>("Esri.Samples", 1, 0, "Simple_RendererSample");
}

void Simple_Renderer::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create a map using the imagery basemap
  m_map = new Map(Basemap::imagery(this), this);

  // create graphics overlay
  m_graphicsOverlay = new GraphicsOverlay(this);

  // create red cross SimpleMarkerSymbol
  SimpleMarkerSymbol* crossSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Cross, QColor("red"), 12, this);

  // create renderer and set symbol to crossSymbol
  SimpleRenderer* simpleRenderer = new SimpleRenderer(crossSymbol, this);
  // set the SimpleRenderer to the GraphicsOverlay
  m_graphicsOverlay->setRenderer(simpleRenderer);

  // create points to render
  Point oldFaithfulPoint(-110.828140, 44.460458, SpatialReference::wgs84());
  Point cascadeGeyserPoint(-110.829004, 44.462438, SpatialReference::wgs84());
  Point plumeGeyserPoint(-110.829381, 44.462735, SpatialReference::wgs84());

  // create graphics using points and add them to GraphicsOverlay
  addPoint(oldFaithfulPoint);
  addPoint(cascadeGeyserPoint);
  addPoint(plumeGeyserPoint);

  // Set map to map view
  m_mapView->setMap(m_map);

  // set viewpoint using the two farthest points as an envelope with padding
  m_mapView->setViewpointGeometry(Envelope(oldFaithfulPoint, plumeGeyserPoint), 50);

  // add GraphicsOverlay to MapView
  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);
}

void Simple_Renderer::addPoint(Point &point)
{
  // create graphic
  Graphic* graphic = new Graphic(point, this);

  // add graphic to Graphic Overlay
  m_graphicsOverlay->graphics()->append(graphic);
}
