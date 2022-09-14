// [WriteFile Name=DensifyAndGeneralize, Category=Geometry]
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

#include "DensifyAndGeneralize.h"

#include "Map.h"
#include "MapQuickView.h"
#include "PointCollection.h"
#include "SpatialReference.h"
#include "Graphic.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleLineSymbol.h"
#include "MultipointBuilder.h"
#include "PolylineBuilder.h"
#include "GeometryEngine.h"
#include "PointCollection.h"

using namespace Esri::ArcGISRuntime;

DensifyAndGeneralize::DensifyAndGeneralize(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void DensifyAndGeneralize::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DensifyAndGeneralize>("Esri.Samples", 1, 0, "DensifyAndGeneralizeSample");
}

void DensifyAndGeneralize::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the streets night vector basemap
  m_map = new Map(BasemapStyle::ArcGISStreetsNight, this);

  // Add a GraphicsOverlay
  m_graphicsOverlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  // Get Points along the river
  PointCollection* pointCollection = createPointCollection();

  // original multipart red graphic
  MultipointBuilder multipointBuilder(pointCollection->spatialReference());
  multipointBuilder.setPoints(pointCollection);
  SimpleMarkerSymbol* originalSms = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("red"), 7.0f /*size*/, this);
  m_originalMultipointGraphic = new Graphic(multipointBuilder.toGeometry(), originalSms, this);
  m_originalMultipointGraphic->setZIndex(0);
  m_graphicsOverlay->graphics()->append(m_originalMultipointGraphic);

  // original red dotted line graphic
  PolylineBuilder polylineBuilder(pointCollection->spatialReference());
  const int pointCollectionSize = pointCollection->size();
  for (int i = 0; i < pointCollectionSize; i++)
  {
    polylineBuilder.addPoint(pointCollection->point(i));
  }
  SimpleLineSymbol* originalSls = new SimpleLineSymbol(SimpleLineSymbolStyle::Dot, QColor("red"), 3.0f /*size*/, this);
  m_originalLineGraphic = new Graphic(polylineBuilder.toGeometry(), originalSls, this);
  m_originalLineGraphic->setZIndex(1);
  m_graphicsOverlay->graphics()->append(m_originalLineGraphic);

  // resulting (densified and generalized) multipart magenta graphic
  SimpleMarkerSymbol* resultSms = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("magenta"), 7.0f /*size*/, this);
  m_resultMultipointGraphic = new Graphic(this);
  m_resultMultipointGraphic->setSymbol(resultSms);
  m_resultMultipointGraphic->setZIndex(2);
  m_graphicsOverlay->graphics()->append(m_resultMultipointGraphic);

  // resulting (densified and generalized) multipart magenta graphic
  SimpleLineSymbol* resultSls = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("magenta"), 3.0f /*size*/, this);
  m_resultLineGraphic = new Graphic(this);
  m_resultLineGraphic->setSymbol(resultSls);
  m_resultLineGraphic->setZIndex(3);
  m_graphicsOverlay->graphics()->append(m_resultLineGraphic);

  // Set map to map view
  m_mapView->setMap(m_map);

  // set viewpoint
  m_mapView->setViewpointGeometry(m_originalMultipointGraphic->geometry().extent(), 100);
}

void DensifyAndGeneralize::updateGeometry(bool densify, double maxSegmentLength, bool generalize, double maxDeviation)
{
  if (!m_originalLineGraphic)
    return;

  // Get the initial Geometry
  Polyline polyline = geometry_cast<Polyline>(m_originalLineGraphic->geometry());
  if (polyline.isEmpty())
    return;

  // Generalize the polyline
  if (generalize)
    polyline = geometry_cast<Polyline>(GeometryEngine::generalize(polyline, maxDeviation, true));

  // Densify the polyline
  if (densify)
    polyline = geometry_cast<Polyline>(GeometryEngine::densify(polyline, maxSegmentLength));

  // Update the line graphic
  m_resultLineGraphic->setGeometry(polyline);

  // Update the multipoint graphic
  if (polyline.parts().size() < 1)
    return;

  MultipointBuilder multipointBuilder(polyline.spatialReference());
  PointCollection* pointCollection = new PointCollection(polyline.spatialReference(), this);

  ImmutablePointCollection polylinePoints = polyline.parts().part(0).points();
  const int polylinePointsSize = polylinePoints.size();
  for (int i = 0; i < polylinePointsSize; i++)
  {
    pointCollection->addPoint(polylinePoints.point(i));
  }
  multipointBuilder.setPoints(pointCollection);
  m_resultMultipointGraphic->setGeometry(multipointBuilder.toGeometry());
}

PointCollection* DensifyAndGeneralize::createPointCollection()
{
  SpatialReference sr(32126);
  PointCollection* pointCollection = new PointCollection(sr, this);
  pointCollection->addPoint(2330611.130549, 202360.002957, 0.000000);
  pointCollection->addPoint(2330583.834672, 202525.984012, 0.000000);
  pointCollection->addPoint(2330574.164902, 202691.488009, 0.000000);
  pointCollection->addPoint(2330689.292623, 203170.045888, 0.000000);
  pointCollection->addPoint(2330696.773344, 203317.495798, 0.000000);
  pointCollection->addPoint(2330691.419723, 203380.917080, 0.000000);
  pointCollection->addPoint(2330435.065296, 203816.662457, 0.000000);
  pointCollection->addPoint(2330369.500800, 204329.861789, 0.000000);
  pointCollection->addPoint(2330400.929891, 204712.129673, 0.000000);
  pointCollection->addPoint(2330484.300447, 204927.797132, 0.000000);
  pointCollection->addPoint(2330514.469919, 205000.792463, 0.000000);
  pointCollection->addPoint(2330638.099138, 205271.601116, 0.000000);
  pointCollection->addPoint(2330725.315888, 205631.231308, 0.000000);
  pointCollection->addPoint(2330755.640702, 206433.354860, 0.000000);
  pointCollection->addPoint(2330680.644719, 206660.240923, 0.000000);
  pointCollection->addPoint(2330386.957926, 207340.947204, 0.000000);
  pointCollection->addPoint(2330485.861737, 207742.298501, 0.000000);
  return pointCollection;
}

void DensifyAndGeneralize::showResults(bool show)
{
  m_resultMultipointGraphic->setVisible(show);
  m_resultLineGraphic->setVisible(show);
}
