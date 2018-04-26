// [WriteFile Name=Buffer, Category=Geometry]
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

#include "Buffer.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Envelope.h"
#include "GeometryEngine.h"
#include "GraphicsOverlay.h"
#include "Graphic.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "Envelope.h"
#include "Viewpoint.h"
#include "SpatialReference.h"

using namespace Esri::ArcGISRuntime;

Buffer::Buffer(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_bufferSize(5)
{
  emit bufferSizeChanged();
}

void Buffer::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<Buffer>("Esri.Samples", 1, 0, "BufferSample");
}

void Buffer::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the topographic basemap
  m_map = new Map(Basemap::topographic(this), this);

  // Set an initial viewpoint
  constexpr double xMin = -10863035.97;
  constexpr double yMin = 3838021.34;
  constexpr double xMax = -10744801.344;
  constexpr double yMax = 3887145.299;
  const SpatialReference sr(3857);
  const Envelope env(xMin, yMin, xMax, yMax, sr);
  m_map->setInitialViewpoint(Viewpoint(env));

  // add a graphics overlay
  m_graphicsOverlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  // Set map to map view
  m_mapView->setMap(m_map);

  // create symbol for the buffer result
  SimpleLineSymbol* outline = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("green"), 5.0f /*width*/, this);
  m_bufferSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor(0, 255, 0, 122), outline, this);

  // create symbol for the mouse click
  m_pointSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("red"), 5.0f /*width*/, this);

  // connect to mouse clicked signal
  connect(m_mapView, SIGNAL(mouseClicked(QMouseEvent&)), this, SLOT(onMouseClicked(QMouseEvent&)));
}

// handle the mouse click - perform a buffer on click
void Buffer::onMouseClicked(QMouseEvent& mouse)
{
  // get the map point from the mouse click
  const Point point = m_mapView->screenToLocation(mouse.x(), mouse.y());

  // Create a variable to be the buffer size in meters. There are 1609.34 meters in one mile.
  const int bufferInMeters = bufferSize() * 1609.34;

  // Buffer the point
  Geometry buffer = GeometryEngine::buffer(point, bufferInMeters);

  // Add the result buffer as a graphic
  Graphic* resultGraphic = new Graphic(buffer, m_bufferSymbol, this);
  m_graphicsOverlay->graphics()->append(resultGraphic);

  // Add the clicked point as a graphic
  Graphic* clickedPointGraphic = new Graphic(point, m_pointSymbol, this);
  m_graphicsOverlay->graphics()->append(clickedPointGraphic);
}

int Buffer::bufferSize() const
{
  return m_bufferSize;
}

void Buffer::setBufferSize(int size)
{
  qDebug() << "sizee";
  if (size == m_bufferSize)
    return;

  m_bufferSize = size;
}
