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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

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
#include "KmlLineStyle.h"
#include "KmlIcon.h"
#include "KmlIconStyle.h"
#include "KmlPolygonStyle.h"
#include "KmlStyle.h"

using namespace Esri::ArcGISRuntime;

CreateAndSaveKmlFile::CreateAndSaveKmlFile(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::darkGrayCanvasVector(this), this)),
  m_kmlDocument(new KmlDocument(this)),
  m_point(createPoint()),
  m_polyline(createPolyline()),
  m_polygon(createPolygon()),
  m_kmlStyleWithPointStyle(createKmlStyleWithPointStyle()),
  m_kmlStyleWithLineStyle(createKmlStyleWithLineStyle()),
  m_kmlStyleWithPolygonStyle(createKmlStyleWithPolygonStyle())
{
  // set initial viewpoint
  m_map->setInitialViewpoint(Viewpoint(createEnvelope()));

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

KmlStyle* CreateAndSaveKmlFile::createKmlStyleWithPointStyle()
{
  // Create Icon Style
  KmlIcon* icon = new KmlIcon(QUrl("https://static.arcgis.com/images/Symbols/Shapes/BlueStarLargeB.png"), this);
  KmlIconStyle* iconStyle = new KmlIconStyle(icon, 1.0, this);

  // Create KmlStyle
  KmlStyle* kmlStyle = new KmlStyle(this);
  kmlStyle->setIconStyle(iconStyle);

  return kmlStyle;
}

KmlStyle* CreateAndSaveKmlFile::createKmlStyleWithLineStyle()
{
  // Create Line Style
  KmlLineStyle* lineStyle = new KmlLineStyle(QColor(Qt::red), 2, this);

  // Create KmlStyle
  KmlStyle* kmlStyle = new KmlStyle(this);
  kmlStyle->setLineStyle(lineStyle);

  return kmlStyle;
}

KmlStyle* CreateAndSaveKmlFile::createKmlStyleWithPolygonStyle()
{
  // Create Polygon Style
  KmlPolygonStyle* polygonStyle = new KmlPolygonStyle(QColor(Qt::yellow), this);

  // Create KmlStyle
  KmlStyle* kmlStyle = new KmlStyle(this);
  kmlStyle->setPolygonStyle(polygonStyle);

  return kmlStyle;
}

void CreateAndSaveKmlFile::addGraphics()
{
  addToKmlDocument(m_point, m_kmlStyleWithPointStyle);
  addToKmlDocument(m_polyline, m_kmlStyleWithLineStyle);
  addToKmlDocument(m_polygon, m_kmlStyleWithPolygonStyle);

  m_mapView->map()->operationalLayers()->append(m_kmlLayer);
}

void CreateAndSaveKmlFile::addToKmlDocument(const Geometry& geometry, KmlStyle* kmlStyle)
{
  const KmlGeometry kmlGeometry = KmlGeometry(geometry, KmlAltitudeMode::ClampToGround);
  KmlPlacemark* placemark = new KmlPlacemark(kmlGeometry, this);
  placemark->setStyle(kmlStyle);
  m_kmlDocument->childNodesListModel()->append(placemark);
}

void CreateAndSaveKmlFile::saveKml(const QUrl& url)
{
  m_busy = true;
  emit busyChanged();
  connect(m_kmlDocument, &KmlDocument::saveAsCompleted, this, [this]()
  {
    m_busy = false;
    emit kmlSaveCompleted();
    emit busyChanged();
  });

  connect(m_kmlDocument, &KmlDocument::errorOccurred, this, [](Error e)
  {
    if (!e.isEmpty())
    {
      qDebug() << QString("Error: %1 - %2").arg(e.message(), e.additionalMessage());
    }
  });
  // Write the KML document to the chosen path.
  m_kmlDocument->saveAs(url.toLocalFile());
}
