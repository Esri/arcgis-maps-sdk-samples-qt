// [WriteFile Name=ReadGeoPackage, Category=Maps]
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

#include "ReadGeoPackage.h"

#include "Map.h"
#include "MapQuickView.h"
#include "GeoPackage.h"
#include "Layer.h"
#include "RasterLayer.h"
#include "FeatureTable.h"
#include "FeatureLayer.h"
#include "GeoPackageRaster.h"
#include "GeoPackageFeatureTable.h"

#include <QDir>

using namespace Esri::ArcGISRuntime;
using namespace std;

ReadGeoPackage::ReadGeoPackage(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void ReadGeoPackage::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ReadGeoPackage>("Esri.Samples", 1, 0, "ReadGeoPackageSample");
}

void ReadGeoPackage::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the topographic basemap
  m_map = new Map(Basemap::topographic(this), this);

  // Set map to map view
  m_mapView->setMap(m_map);

  // Create the initial view area
  const Viewpoint initialViewpoint(39.7294, -104.8319, 500000);
  m_mapView->setViewpoint(initialViewpoint);

  // Initialize the read operation
  readGeoPackage();
}

// Read the geoPackage and create lists for the names and layer data
void ReadGeoPackage::readGeoPackage()
{
  // Load the geoPackage at the beginning
  const QString path = "/Users/andr9668/ArcGIS/Runtime/Data/gpkg/AuroraCO.gpkg";
  GeoPackage* auroraGpkg = new GeoPackage(path, this);
  m_layerList.clear();

  // Make sure there are no errors in loading the geoPackage before interacting with it
  connect(auroraGpkg, &GeoPackage::doneLoading, this, [auroraGpkg, this](Error error)
  {
    if (error.isEmpty())
    {
      // Create two QLists for the different types of layer data within the geoPackage (these are combined into m_layerList later on)
      QList<GeoPackageRaster*> gpkgRasters = auroraGpkg->geoPackageRasters();
      QList<GeoPackageFeatureTable*> gpkgFeatures = auroraGpkg->geoPackageFeatureTables();

      // Loop through both lists to get the name and layer data. The name data is only used to populate the menu.
      for (const auto& featureTbl : gpkgFeatures)
      {
        QVariantMap layerMap;
        FeatureLayer* layer = new FeatureLayer(featureTbl, this);
        layerMap["name"] = layer->name();
        layerMap["lyr"] = QVariant::fromValue<Layer*>(layer);
        m_layerList << layerMap;
      }
      for (const auto& rasterItm : gpkgRasters)
      {
        QVariantMap rasterMap;
        RasterLayer* rasterLyr = new RasterLayer(rasterItm, this);
        rasterMap["name"] = rasterLyr->raster()->path().mid(55, 25);
        rasterMap["lyr"] = QVariant::fromValue<Layer*>(rasterLyr);
        m_layerList << rasterMap;
      }
      emit layerListChanged();
    }
  });
  auroraGpkg->load();
}

// Getter for the menu names data
QVariantList ReadGeoPackage::layerList() const
{
  return m_layerList;
}

// Called by the QML to toggle visibility at a certain index
void ReadGeoPackage::addOrShowLayer(int index, bool visible)
{
  auto opLayers = m_map->operationalLayers();
  auto layer = qvariant_cast<Layer*>(m_layerList[index].toMap()["lyr"]);
  int indexOfLayer = opLayers->indexOf(layer);
  if (indexOfLayer != -1 && !visible)
  {
    opLayers->at(indexOfLayer)->setVisible(false);
  }
  else
  {
    opLayers->insert(index, layer);
    layer->setVisible(true);
  }
}

