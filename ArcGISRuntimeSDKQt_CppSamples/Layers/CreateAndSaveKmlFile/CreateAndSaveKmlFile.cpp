// [WriteFile Name=CreateAndSaveKmlFile, Category=Geometry]
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
#include "QDir"

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

    qDebug() << m_kmlDocument->childNodesListModel()->size();

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
  // Create a polyline builder
  const SpatialReference spatialRef(4326);
  PolylineBuilder polylineBuilder(spatialRef);

  // add points to the builder that approximates the border between California and Nevada.
  polylineBuilder.addPoint(-119.992, 41.989);
  polylineBuilder.addPoint(-110.994, 38.994);
  polylineBuilder.addPoint(-114.620, 32.0);

  // Return the geometry.
  return polylineBuilder.toGeometry();
}

Geometry CreateAndSaveKmlFile::createPolygon() const
{
  // Create a polygon builder
  const SpatialReference spatialRef(4326);
  PolygonBuilder polygonBuilder(spatialRef);

  // add points to the builder that approximates the boundary of Colorado.
  polygonBuilder.addPoint(-109.048, 40.998);
  polygonBuilder.addPoint(-102.047, 40.998);
  polygonBuilder.addPoint(-102.037, 36.989);
  polygonBuilder.addPoint(-109.048, 36.998);

  // Return the geometry.
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

  // Create a simple marker symbol - used to render a map point where the Esri headquarters is located.
  SimpleMarkerSymbol* simpleMarkerSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, QColor("green"), 10.0f, this);

  // Create a simple line symbol - used to render a polyline border between California and Nevada.
  SimpleLineSymbol* simpleLineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("blue"), 5.0f, this);

  // Create a simple fill symbol - used to render a polygon covering Colorado.
  SimpleFillSymbol* simpleFillSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("red"), this);

  m_point = new Point(createPoint());
  Geometry projectPoint = GeometryEngine::project(*m_point, SpatialReference(SpatialReference::wgs84()));
  KmlGeometry* kmlPoint = new KmlGeometry(projectPoint, KmlAltitudeMode::ClampToGround);
  if(kmlPoint->isEmpty())
    qDebug() << "is empty";

  if(kmlPoint->type() != KmlGeometryType::Unknown)
    qDebug() << "geom type: " << (int)kmlPoint->type();

  KmlPlacemark* kmlPlacemark = new KmlPlacemark(*kmlPoint);
  qDebug() << kmlPlacemark->geometriesListModel()->size();

  m_kmlDocument->childNodesListModel()->append(kmlPlacemark);
  qDebug() << m_kmlDocument->childNodesListModel()->size();

  m_polyline = new Polyline(createPolyline());
  m_polygon = new Polygon(createPolygon());

  m_graphicsOverlay->graphics()->append(new Graphic(*m_point, simpleMarkerSymbol, this));
  m_graphicsOverlay->graphics()->append(new Graphic(*m_polyline, simpleLineSymbol, this));
  m_graphicsOverlay->graphics()->append(new Graphic(*m_polygon, simpleFillSymbol, this));

  m_mapView->setViewpointGeometry(createEnvelope(), 10);
}

void CreateAndSaveKmlFile::saveKml(QString url) {

  qDebug() << url;
  qDebug() << m_kmlDocument->childNodesListModel()->size();
  connect(m_kmlDocument, &KmlDocument::saveAsCompleted, this, []()
  {
    qDebug() << "save completed?";
  });

  const auto savePath = "/Users/jare8800/ArcGIS/Runtime/Data/kml/temp.kmz";
  m_kmlDocument->saveAs(savePath);
//  KmlNode* temp = m_kmlDocument->childNodesListModel()->at(0);
//  temp->saveAs(url);
}
