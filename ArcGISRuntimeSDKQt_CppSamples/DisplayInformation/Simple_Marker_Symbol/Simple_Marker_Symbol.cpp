// [WriteFile Name=Simple_Marker_Symbol, Category=DisplayInformation]
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

#include "Simple_Marker_Symbol.h"

#include "Basemap.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "Point.h"
#include "Map.h"
#include "MapQuickView.h"
#include "SimpleMarkerSymbol.h"
#include "SpatialReference.h"
#include "Viewpoint.h"

using namespace Esri::ArcGISRuntime;

Simple_Marker_Symbol::Simple_Marker_Symbol(QQuickItem* parent) :
  QQuickItem(parent)
{
}

Simple_Marker_Symbol::~Simple_Marker_Symbol() = default;

void Simple_Marker_Symbol::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<Simple_Marker_Symbol>("Esri.Samples", 1, 0, "Simple_Marker_SymbolSample");
}

void Simple_Marker_Symbol::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // create a map using the imagery basemap
  m_map = new Map(Basemap::imagery(this), this);

  // set initial viewpoint
  m_map->setInitialViewpoint(Viewpoint(Point(-226773, 6550477, SpatialReference::webMercator()), 7500));

  // create a GraphicsOverlay
  m_graphicsOverlay = new GraphicsOverlay(this);

  // create red circle graphic
  Graphic* redCircle = new Graphic(Point(-226773, 6550477, SpatialReference::webMercator()), this);
  SimpleMarkerSymbol* redCircleSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("red"), 12, this);
  redCircle->setSymbol(redCircleSymbol);

  // append to graphics overlay
  m_graphicsOverlay->graphics()->append(redCircle);

  // add the GraphicsOverlay to map view
  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  // set map to map view
  m_mapView->setMap(m_map);
}
