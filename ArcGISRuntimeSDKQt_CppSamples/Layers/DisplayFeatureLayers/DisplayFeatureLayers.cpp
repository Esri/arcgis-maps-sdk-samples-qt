// [WriteFile Name=DisplayFeatureLayers, Category=Layers]
// [Legal]
// Copyright 2022 Esri.

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

#include "DisplayFeatureLayers.h"

#include "Envelope.h"
#include "Error.h"
#include "FeatureLayer.h"
#include "Geodatabase.h"
#include "GeodatabaseFeatureTable.h"
#include "GeoPackage.h"
#include "GeoPackageFeatureTable.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "Portal.h"
#include "PortalItem.h"
#include "ServiceFeatureTable.h"
#include "ShapefileFeatureTable.h"
#include "SpatialReference.h"
#include "Viewpoint.h"

#include <QStandardPaths>

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
QString defaultDataPath()
{
  QString dataPath;

#ifdef Q_OS_IOS
  dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
  dataPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif

  return dataPath + "/ArcGIS/Runtime/Data/";
}
} // namespace

DisplayFeatureLayers::DisplayFeatureLayers(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISTopographic, this))
{
}

DisplayFeatureLayers::~DisplayFeatureLayers() = default;

void DisplayFeatureLayers::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DisplayFeatureLayers>("Esri.Samples", 1, 0, "DisplayFeatureLayersSample");
}

MapQuickView* DisplayFeatureLayers::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void DisplayFeatureLayers::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}

void DisplayFeatureLayers::setLayerMode(FeatureLayerMode featureLayerMode)
{
  if (!m_map)
    return;

  m_map->operationalLayers()->clear();

  m_currentLayerMode = featureLayerMode;

  switch (featureLayerMode) {
    case FeatureLayerMode::Geodatabase:
      setGeodatabaseLayer();
      break;
    case FeatureLayerMode::Geopackage:
      setGeopackageLayer();
      break;
    case FeatureLayerMode::PortalItem:
      setPortalItemLayer();
      break;
    case FeatureLayerMode::ServiceFeatureTable:
      setServiceFeatureTableLayer();
      break;
    case FeatureLayerMode::Shapefile:
      setShapefileLayer();
      break;
    default:
      break;
  }
}

void DisplayFeatureLayers::setGeodatabaseLayer()
{
  Geodatabase* gdb = new Geodatabase(defaultDataPath() + "geodatabase/LA_Trails.geodatabase", this);
  connect(gdb, &Geodatabase::doneLoading, this, [gdb, this](Error e)
  {
    // If the current layer mode has changed while loading, abort this process
    if (m_currentLayerMode != FeatureLayerMode::Geodatabase)
      return;

    if (!e.isEmpty())
    {
      qDebug() << e.message() << e.additionalMessage();
      return;
    }

    GeodatabaseFeatureTable* gdbFeatureTable = gdb->geodatabaseFeatureTable("Trailheads");
    m_featureLayer = new FeatureLayer(gdbFeatureTable, this);
    m_map->operationalLayers()->append(m_featureLayer);

    m_mapView->setViewpointAndWait(Viewpoint(Point(-13214155, 4040194, SpatialReference(3857)), 35e4));
  });
  gdb->load();
}

void DisplayFeatureLayers::setGeopackageLayer()
{
  GeoPackage* gpkg = new GeoPackage(defaultDataPath() + "gpkg/AuroraCO.gpkg", this);

  connect(gpkg, &GeoPackage::doneLoading, this, [this, gpkg](Error e)
  {
    // If the current layer mode has changed while loading, abort this process
    if (m_currentLayerMode != FeatureLayerMode::Geopackage)
      return;

    if (!e.isEmpty())
    {
      qDebug() << e.message() << e.additionalMessage();
      return;
    }

    if (!(gpkg->geoPackageFeatureTables().size() > 0))
      return;

    GeoPackageFeatureTable* gpkgFeatureTable = gpkg->geoPackageFeatureTables().at(0);
    m_featureLayer = new FeatureLayer(gpkgFeatureTable, this);
    m_map->operationalLayers()->append(m_featureLayer);

    m_mapView->setViewpointAndWait(Viewpoint(Point(-104.8319, 39.7294, SpatialReference(4326)), 200'000));

    connect(m_featureLayer, &FeatureLayer::doneLoading, this, [this](Error e)
    {
      // If the current layer mode has changed while loading, abort this process
      if (m_currentLayerMode != FeatureLayerMode::Geopackage)
        return;

      if (!e.isEmpty())
      {
        qDebug() << e.message() << e.additionalMessage();
        return;
      }

      m_mapView->setViewpointAndWait(Viewpoint(m_featureLayer->fullExtent()));
    });
  });

  gpkg->load();
}

void DisplayFeatureLayers::setPortalItemLayer()
{
  Portal* portal = new Portal(this);
  PortalItem* portalItem = new PortalItem(portal, "1759fd3e8a324358a0c58d9a687a8578", this);
  m_featureLayer = new FeatureLayer(portalItem, 0, this);
  m_map->operationalLayers()->append(m_featureLayer);

  m_mapView->setViewpointAndWait(Viewpoint(45.5266, -122.6219, 6000));
}

void DisplayFeatureLayers::setServiceFeatureTableLayer()
{
  ServiceFeatureTable* serviceFeatureTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/Energy/Geology/FeatureServer/9"), this);
  m_featureLayer = new FeatureLayer(serviceFeatureTable, this);
  m_map->operationalLayers()->append(m_featureLayer);

  m_mapView->setViewpointAndWait(Viewpoint(Point(-13176752, 4090404, SpatialReference(102100)), 300000));
}

void DisplayFeatureLayers::setShapefileLayer()
{
  ShapefileFeatureTable* shpFeatureTable = new ShapefileFeatureTable(defaultDataPath() + "shp/Public_Art.shp", this);
  m_featureLayer = new FeatureLayer(shpFeatureTable, this);
  m_map->operationalLayers()->append(m_featureLayer);

  m_mapView->setViewpointAndWait(Viewpoint(Point(-104.8319, 39.7294, SpatialReference(4326)), 200'000));
}
