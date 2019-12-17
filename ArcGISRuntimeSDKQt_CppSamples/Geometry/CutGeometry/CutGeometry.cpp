// [WriteFile Name=CutGeometry, Category=Geometry]
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

#include "CutGeometry.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "GeometryEngine.h"
#include "SpatialReference.h"
#include "PolylineBuilder.h"
#include "PolygonBuilder.h"
#include "SimpleLineSymbol.h"
#include "SimpleFillSymbol.h"

using namespace Esri::ArcGISRuntime;

CutGeometry::CutGeometry(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void CutGeometry::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<CutGeometry>("Esri.Samples", 1, 0, "CutGeometrySample");
}

void CutGeometry::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the topographic basemap
  m_map = new Map(Basemap::topographic(this), this);

  // Set map to map view
  m_mapView->setMap(m_map);

  // Add a GraphicsOverlay
  m_overlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(m_overlay);

  // Create initial graphics
  createGraphics();

  // Set Viewpoint
  m_mapView->setViewpointGeometry(m_lakeSuperiorGraphic->geometry());
}

// Cuts the Lake Superior polygon into two and symbolizes each output graphic
void CutGeometry::cutPolygon()
{
  // perform the cut
  QList<Geometry> geoms = GeometryEngine::cut(m_lakeSuperiorGraphic->geometry(), m_borderGraphic->geometry());

  // create graphics for the U.S. side
  SimpleLineSymbol* outline = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("blue"), 1.0f, this);
  SimpleFillSymbol* usSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::ForwardDiagonal, QColor("#40ff00"), outline, this);
  Graphic* usSide = new Graphic(geoms.at(0), usSymbol, this);

  // create graphics for the Canada side
  SimpleFillSymbol* canadaSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::ForwardDiagonal, QColor("#ffff00"), outline, this);
  Graphic* canadaSide = new Graphic(geoms.at(1), canadaSymbol, this);

  // add graphics
  m_overlay->graphics()->append(canadaSide);
  m_overlay->graphics()->append(usSide);
}

// Create the two baseline graphics
void CutGeometry::createGraphics()
{
  // Lake Superior Graphic
  SimpleLineSymbol* outline = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("blue"), 1.0f, this);
  SimpleFillSymbol* usSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor(198, 204, 255, 178), outline, this);
  m_lakeSuperiorGraphic = new Graphic(createLakeSuperiorPolygon(), usSymbol, this);
  m_overlay->graphics()->append(m_lakeSuperiorGraphic);

  // Border Graphic
  SimpleLineSymbol* borderSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Dot, QColor("red"), 5.0f, this);
  m_borderGraphic = new Graphic(createBorderPolyline(), borderSymbol, this);
  m_borderGraphic->setZIndex(10);
  m_overlay->graphics()->append(m_borderGraphic);
}

// Create the Lake Superior Polygon
Polygon CutGeometry::createLakeSuperiorPolygon()
{
  PolygonBuilder* polygonBuilder = new PolygonBuilder(SpatialReference::webMercator(), this);
  polygonBuilder->addPoint(-10254374.668616, 5908345.076380);
  polygonBuilder->addPoint(-10178382.525314, 5971402.386779);
  polygonBuilder->addPoint(-10118558.923141, 6034459.697178);
  polygonBuilder->addPoint(-9993252.729399, 6093474.872295);
  polygonBuilder->addPoint(-9882498.222673, 6209888.368416);
  polygonBuilder->addPoint(-9821057.766387, 6274562.532928);
  polygonBuilder->addPoint(-9690092.583250, 6241417.023616);
  polygonBuilder->addPoint(-9605207.742329, 6206654.660191);
  polygonBuilder->addPoint(-9564786.389509, 6108834.986367);
  polygonBuilder->addPoint(-9449989.747500, 6095091.726408);
  polygonBuilder->addPoint(-9462116.153346, 6044160.821855);
  polygonBuilder->addPoint(-9417652.665244, 5985145.646738);
  polygonBuilder->addPoint(-9438671.768711, 5946341.148031);
  polygonBuilder->addPoint(-9398250.415891, 5922088.336339);
  polygonBuilder->addPoint(-9419269.519357, 5855797.317714);
  polygonBuilder->addPoint(-9467775.142741, 5858222.598884);
  polygonBuilder->addPoint(-9462924.580403, 5902686.086985);
  polygonBuilder->addPoint(-9598740.325877, 5884092.264688);
  polygonBuilder->addPoint(-9643203.813979, 5845287.765981);
  polygonBuilder->addPoint(-9739406.633691, 5879241.702350);
  polygonBuilder->addPoint(-9783061.694736, 5922896.763395);
  polygonBuilder->addPoint(-9844502.151022, 5936640.023354);
  polygonBuilder->addPoint(-9773360.570059, 6019099.583107);
  polygonBuilder->addPoint(-9883306.649729, 5968977.105610);
  polygonBuilder->addPoint(-9957681.938918, 5912387.211662);
  polygonBuilder->addPoint(-10055501.612742, 5871965.858842);
  polygonBuilder->addPoint(-10116942.069028, 5884092.264688);
  polygonBuilder->addPoint(-10111283.079633, 5933406.315128);
  polygonBuilder->addPoint(-10214761.742852, 5888134.399970);
  polygonBuilder->addPoint(-10254374.668616, 5901877.659929);
  return polygonBuilder->toGeometry();
}

// Create the U.S./Canada Border Polyline
Polyline CutGeometry::createBorderPolyline()
{
  PolylineBuilder* polylineBuilder = new PolylineBuilder(SpatialReference::webMercator(), this);
  polylineBuilder->addPoint(-9981328.687124, 6111053.281447);
  polylineBuilder->addPoint(-9946518.044066, 6102350.620682);
  polylineBuilder->addPoint(-9872545.427566, 6152390.920079);
  polylineBuilder->addPoint(-9838822.617103, 6157830.083057);
  polylineBuilder->addPoint(-9446115.050097, 5927209.572793);
  polylineBuilder->addPoint(-9430885.393759, 5876081.440801);
  polylineBuilder->addPoint(-9415655.737420, 5860851.784463);
  return polylineBuilder->toGeometry();
}
