// [WriteFile Name=DisplayFeatureLayers, Category=Layers]
// [Legal]
// Copyright 2022 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "DisplayFeatureLayers.h"

// C++ API headers
#include "Envelope.h"
#include "Error.h"
#include "FeatureLayer.h"
#include "GeoPackage.h"
#include "GeoPackageFeatureTable.h"
#include "Geodatabase.h"
#include "GeodatabaseFeatureTable.h"
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

// Qt headers
#include <QFuture>
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
  // Each of these methods creates and adds a feature layer of the specified type to the map's operationalLayers LayerListModel
  addGeodatabaseLayer();
  addGeopackageLayer();
  addPortalItemLayer();
  addServiceFeatureTableLayer();
  addShapefileLayer();
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

void DisplayFeatureLayers::setLayerVisibility(FeatureLayerType featureLayerType)
{
  if (!m_map || !m_mapView)
    return;

  // Hide all other layers
  for (Layer* layer : *m_map->operationalLayers())
  {
    layer->setVisible(false);
  }

  // Make the selected feature layer visible and set the viewpoint to show the layer
  switch (featureLayerType) {
    case FeatureLayerType::Geodatabase:
      m_gdbFeatureLayer->setVisible(true);
      m_mapView->setViewpointGeometryAsync(m_gdbFeatureLayer->fullExtent());
      break;
    case FeatureLayerType::Geopackage:
      m_gpkgFeatureLayer->setVisible(true);
      m_mapView->setViewpointGeometryAsync(m_gpkgFeatureLayer->fullExtent());
      break;
    case FeatureLayerType::PortalItem:
      m_portalItemFeatureLayer->setVisible(true);
      // We can set padding on the viewpoint geometry. A negative value will zoom in.
      m_mapView->setViewpointGeometryAsync(m_portalItemFeatureLayer->fullExtent(), -10'000);
      break;
    case FeatureLayerType::ServiceFeatureTable:
      m_serviceFeatureTableFeatureLayer->setVisible(true);
      // The extent of this layer is very large so we can set the viewpoint to a specific point
      m_mapView->setViewpointAndWait(Viewpoint(Point(-13176752, 4090404, SpatialReference(102100)), 300'000));
      break;
    case FeatureLayerType::Shapefile:
      m_shpFeatureLayer->setVisible(true);
      m_mapView->setViewpointGeometryAsync(m_shpFeatureLayer->fullExtent());
      break;
    default:
      break;
  }
}

void DisplayFeatureLayers::addGeodatabaseLayer()
{
  Geodatabase* gdb = new Geodatabase(defaultDataPath() + "geodatabase/LA_Trails.geodatabase", this);
  connect(gdb, &Geodatabase::doneLoading, this, [gdb, this](const Error& e)
  {
    if (!e.isEmpty())
    {
      qDebug() << e.message() << e.additionalMessage();
      return;
    }

    GeodatabaseFeatureTable* gdbFeatureTable = gdb->geodatabaseFeatureTable("Trailheads");
    m_gdbFeatureLayer = new FeatureLayer(gdbFeatureTable, this);
    m_map->operationalLayers()->append(m_gdbFeatureLayer);

    // Because this feature layer is added asynchronously, we may need to set its visibility even after the initial setLayerVisibility() method is called
    m_gdbFeatureLayer->setVisible(false);
  });

  gdb->load();
}

void DisplayFeatureLayers::addGeopackageLayer()
{
  GeoPackage* gpkg = new GeoPackage(defaultDataPath() + "gpkg/AuroraCO.gpkg", this);

  connect(gpkg, &GeoPackage::doneLoading, this, [this, gpkg](const Error& e)
  {
    if (!e.isEmpty())
    {
      qDebug() << e.message() << e.additionalMessage();
      return;
    }

    if (!(gpkg->geoPackageFeatureTables().size() > 0))
      return;

    GeoPackageFeatureTable* gpkgFeatureTable = gpkg->geoPackageFeatureTables().at(0);
    m_gpkgFeatureLayer = new FeatureLayer(gpkgFeatureTable, this);
    m_map->operationalLayers()->append(m_gpkgFeatureLayer);

    // Because this feature layer is added asynchronously, we may need to set its visibility even after the initial setLayerVisibility() method is called
    m_gpkgFeatureLayer->setVisible(false);
  });

  gpkg->load();
  // Note that you must call close() on GeoPackage to allow other processes to access it
}

void DisplayFeatureLayers::addPortalItemLayer()
{
  Portal* portal = new Portal(this);
  PortalItem* portalItem = new PortalItem(portal, "1759fd3e8a324358a0c58d9a687a8578", this);

  // A portal item can be many things from files to web maps.
  // We can check its type to ensure we're instantiating a feature layer with the
  // correct portal item type, but the portal item must first be loaded explicitly.
  // Here, the portal item is automatically loaded when it is used to instantiate a feature layer.

  m_portalItemFeatureLayer = new FeatureLayer(portalItem, 0, this);
  m_map->operationalLayers()->append(m_portalItemFeatureLayer);
}

void DisplayFeatureLayers::addServiceFeatureTableLayer()
{
  ServiceFeatureTable* serviceFeatureTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/Energy/Geology/FeatureServer/9"), this);
  m_serviceFeatureTableFeatureLayer = new FeatureLayer(serviceFeatureTable, this);
  m_map->operationalLayers()->append(m_serviceFeatureTableFeatureLayer);
}

void DisplayFeatureLayers::addShapefileLayer()
{
  ShapefileFeatureTable* shpFeatureTable = new ShapefileFeatureTable(defaultDataPath() + "shp/ScottishWildlifeTrust_ReserveBoundaries_20201102.shp", this);
  m_shpFeatureLayer = new FeatureLayer(shpFeatureTable, this);
  m_map->operationalLayers()->append(m_shpFeatureLayer);
}
