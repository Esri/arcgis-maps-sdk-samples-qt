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
#include "PolygonBuilder.h"
#include "PolylineBuilder.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "KmlDocument.h"
#include "KmlPlacemark.h"
#include "KmlNodeListModel.h"
#include "KmlDataset.h"
#include "KmlLayer.h"

using namespace Esri::ArcGISRuntime;

CreateAndSaveKmlFile::CreateAndSaveKmlFile(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::darkGrayCanvasVector(this), this)),
  m_kmlDocument(new KmlDocument(this)),
  m_point(createPoint()),
  m_polyline(createPolyline()),
  m_polygon(createPolygon())
{
  connect(m_map, &Map::doneLoading, this, [this](Error e)
  {
    if (!e.isEmpty())
      return;

    m_kmlDataset = new KmlDataset(m_kmlDocument, this);
    m_kmlLayer = new KmlLayer(m_kmlDataset, this);
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
  polylineBuilder.addPoint(-119.994, 38.994);
  polylineBuilder.addPoint(-114.620, 35.0);

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
  addToKmlDocument(m_point);
  addToKmlDocument(m_polyline);
  addToKmlDocument(m_polygon);

  m_mapView->map()->operationalLayers()->append(m_kmlLayer);
  m_mapView->setViewpointGeometry(createEnvelope(), 10);
}

void CreateAndSaveKmlFile::addToKmlDocument(const Esri::ArcGISRuntime::Geometry& geometry)
{
  const KmlGeometry kmlGeometry = KmlGeometry(geometry, KmlAltitudeMode::ClampToGround);
  m_kmlDocument->childNodesListModel()->append(new KmlPlacemark(kmlGeometry, this));
}

void CreateAndSaveKmlFile::saveKml(const QUrl& url)
{
  m_busy = true;
  emit busyChanged();
  connect(m_kmlDocument, &KmlDocument::saveAsCompleted, this, [this]()
  {
    m_busy = false;
    emit busyChanged();
  });
  // Write the KML document to the chosen path.
  m_kmlDocument->saveAs(url.toLocalFile());
}
