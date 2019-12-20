// [WriteFile Name=ClipGeometry, Category=Geometry]
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

#include "ClipGeometry.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Geometry.h"
#include "Graphic.h"
#include "GeometryEngine.h"
#include "Viewpoint.h"
#include "GraphicsOverlay.h"
#include "SimpleLineSymbol.h"
#include "SimpleFillSymbol.h"

using namespace Esri::ArcGISRuntime;

ClipGeometry::ClipGeometry(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void ClipGeometry::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ClipGeometry>("Esri.Samples", 1, 0, "ClipGeometrySample");
}

void ClipGeometry::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the topographic basemap
  m_map = new Map(Basemap::topographic(this), this);

  // Create a graphics overlays
  m_coloradoOverlay = new GraphicsOverlay(this);
  m_envelopesOverlay = new GraphicsOverlay(this);
  m_clippedAreasOverlay = new GraphicsOverlay(this);

  // Create an envelope to be used as the Colorado geometry
  m_coloradoEnvelope = Envelope(-12138232.018408, 4441198.773776, -11362327.128340, 5012861.290274, SpatialReference(3857));

  // Create an envelope for each clipping area
  m_outsideEnvelope = Envelope(-12201990.219681, 5147942.225174, -11858344.321294, 5297071.577304, SpatialReference(3857));
  m_intersectingEnvelope = Envelope(-12260345.183558, 4332053.378376, -11962086.479298, 4566553.881363, SpatialReference(3857));
  m_insideEnvelope = Envelope(-11655182.595204, 4593570.068343, -11431488.567009, 4741618.772994, SpatialReference(3857));

  // Set initial Viewpoint
  const Viewpoint initialViewpoint(Point(-11655182, 4741618, SpatialReference(3857)), 15151632);
  m_map->setInitialViewpoint(initialViewpoint);

  // Set map to map view and append the two initial graphics overlays
  m_mapView->setMap(m_map);
  m_mapView->graphicsOverlays()->append(m_coloradoOverlay);
  m_mapView->graphicsOverlays()->append(m_envelopesOverlay);

  // Create the graphics
  createGraphics();
}

// Creates the visual elements and adds them to their respective overlays
void ClipGeometry::createGraphics()
{
  // Colorado outline + fill
  m_coloradoOutline = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("#FF0000FF"), 2.0f, this);
  m_coloradoFill = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("#220000FF"), m_coloradoOutline, this);

  // Create and add the Colorado graphic to its overlay
  m_coloradoGraphic = new Graphic(m_coloradoEnvelope, m_coloradoFill, this);
  m_coloradoOverlay->graphics()->append(m_coloradoGraphic);

  // Clipping envelopes outline
  SimpleLineSymbol* envelopeOutline = new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, QColor("#FFFF0000"), 3.0f, this);

  // Create and add the clipping envelope graphics to their overlay
  m_outsideGraphic = new Graphic(m_outsideEnvelope, envelopeOutline, this);
  m_intersectingGraphic = new Graphic(m_intersectingEnvelope, envelopeOutline, this);
  m_insideGraphic = new Graphic(m_insideEnvelope, envelopeOutline, this);
  m_envelopesOverlay->graphics()->append(m_outsideGraphic);
  m_envelopesOverlay->graphics()->append(m_intersectingGraphic);
  m_envelopesOverlay->graphics()->append(m_insideGraphic);

  // Append the (empty) final graphicsOverlay to the mapView to render. Later on, clipAreas() will populate this with the clipped graphics
  m_mapView->graphicsOverlays()->append(m_clippedAreasOverlay);
}

// Clips the overlapping geometries and displays them on the map
void ClipGeometry::clipAreas()
{
  // Immediately hide the reference graphic
  m_coloradoGraphic->setVisible(false);

  // Get the graphicsListModel from the overlay containing the clipping envelopes
  GraphicListModel* graphics = m_envelopesOverlay->graphics();

  /* Iterate through the graphics' geometries and call the static 'clip' function, passing in
   * the reference graphic's geometry.
   * If the return value is valid, create a new graphic using the returned clipped geometry and the
   * fillSymbol of the reference graphic as arguments. Append the new graphic to its respective
   * graphicsOverlay. */
  const int graphicsSize = graphics->size();
  const Geometry coloradoGeometry = m_coloradoGraphic->geometry();
  for (int i = 0; i < graphicsSize; i++)
  {
    m_clippedGeometry = GeometryEngine::clip(coloradoGeometry, graphics->at(i)->geometry().extent());
    if (!m_clippedGeometry.isEmpty())
    {
      Graphic* clippedGraphic = new Graphic(m_clippedGeometry, m_coloradoFill, this);
      m_clippedAreasOverlay->graphics()->append(clippedGraphic);
    }
  }
}
