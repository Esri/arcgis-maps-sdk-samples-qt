// [WriteFile Name=CreateAndSaveKmlFile, Category=Layers]
// [Legal]
// Copyright 2019 Esri.

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

#include "CreateAndSaveKmlFile.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "Geometry.h"
#include "Point.h"
#include "Polygon.h"
#include "Polyline.h"
#include "PolygonBuilder.h"
#include "PolylineBuilder.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "KmlDocument.h"
#include "KmlGeometry.h"
#include "KmlPlacemark.h"
#include "GeometryEngine.h"
#include "KmlNodeListModel.h"
#include "KmlGeometryListModel.h"
#include "KmlNode.h"

using namespace Esri::ArcGISRuntime;

CreateAndSaveKmlFile::CreateAndSaveKmlFile(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::topographic(this), this)),
  m_kmlDocument( new KmlDocument(this)),
  m_graphicsOverlay(new GraphicsOverlay(this))

{
  connect(m_map, &Map::doneLoading, this, [this](Error e)
  {
    if(!e.isEmpty())
      return;

    m_mapView->graphicsOverlays()->append(m_graphicsOverlay);
    addGraphics();
  });

}

CreateAndSaveKmlFile::~CreateAndSaveKmlFile() = default;

void CreateAndSaveKmlFile::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<CreateAndSaveKmlFile>("Esri.Samples", 1, 0, "CreateAndSaveKmlFileSample");
}

MapQuickView* CreateAndSaveKmlFile::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void CreateAndSaveKmlFile::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}

Geometry CreateAndSaveKmlFile::createPoint() const
{
  constexpr double x = -117.195800;
  constexpr double y = 34.056295;
  const SpatialReference spatialRef(4326);

  return Point(x, y, spatialRef);
}

Geometry CreateAndSaveKmlFile::createPolyline() const
{
  const SpatialReference spatialRef(4326);
  PolylineBuilder polylineBuilder(spatialRef);

  polylineBuilder.addPoint(-119.992, 41.989);
  polylineBuilder.addPoint(-110.994, 38.994);
  polylineBuilder.addPoint(-114.620, 32.0);

  return polylineBuilder.toGeometry();
}

Geometry CreateAndSaveKmlFile::createPolygon() const
{
  const SpatialReference spatialRef(4326);
  PolygonBuilder polygonBuilder(spatialRef);

  polygonBuilder.addPoint(-109.048, 40.998);
  polygonBuilder.addPoint(-102.047, 40.998);
  polygonBuilder.addPoint(-102.037, 36.989);
  polygonBuilder.addPoint(-109.048, 36.998);

  return polygonBuilder.toGeometry();
}

Geometry CreateAndSaveKmlFile::createEnvelope() const
{
  constexpr double xMin = -123.0;
  constexpr double yMin = 33.5;
  constexpr double xMax = -101.0;
  constexpr double yMax = 42.0;

  const SpatialReference spatialRef(4326);

  return Envelope(xMin, yMin, xMax, yMax, spatialRef);
}

void CreateAndSaveKmlFile::addGraphics()
{
  SimpleMarkerSymbol* simpleMarkerSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, QColor("green"), 10.0f, this);

  SimpleLineSymbol* simpleLineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("blue"), 5.0f, this);

  SimpleFillSymbol* simpleFillSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("red"), this);

  m_point = new Point(createPoint());
  m_graphicsOverlay->graphics()->append(new Graphic(*m_point, simpleMarkerSymbol, this));
  addToKmlDocument(*m_point);

  m_polyline = new Polyline(createPolyline());
  m_graphicsOverlay->graphics()->append(new Graphic(*m_polyline, simpleLineSymbol, this));
  addToKmlDocument(*m_polyline);

  m_polygon = new Polygon(createPolygon());
  m_graphicsOverlay->graphics()->append(new Graphic(*m_polygon, simpleFillSymbol, this));
  addToKmlDocument(*m_polygon);

  m_mapView->setViewpointGeometry(createEnvelope(), 10);
}

void CreateAndSaveKmlFile::addToKmlDocument(const Esri::ArcGISRuntime::Geometry &geometry)
{
  const KmlGeometry* kmlGeometry = new KmlGeometry(geometry, KmlAltitudeMode::ClampToGround);
  KmlPlacemark* kmlPlacemark = new KmlPlacemark(*kmlGeometry);
  m_kmlDocument->childNodesListModel()->append(kmlPlacemark);
  delete kmlGeometry;
  delete kmlPlacemark;
}

void CreateAndSaveKmlFile::saveKml(QUrl url)
{
  m_busy = true;
  emit busyChanged();
  connect(m_kmlDocument, &KmlDocument::saveAsCompleted, this, [this]()
  {
    m_busy = false;
    emit busyChanged();
  });
  m_kmlDocument->saveAs(url.toLocalFile());
}
