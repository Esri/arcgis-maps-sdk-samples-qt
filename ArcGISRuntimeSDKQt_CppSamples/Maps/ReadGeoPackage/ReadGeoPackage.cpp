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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "ReadGeoPackage.h"

#include "Map.h"
#include "Layer.h"
#include "GeoPackage.h"
#include "RasterLayer.h"
#include "MapQuickView.h"
#include "FeatureTable.h"
#include "FeatureLayer.h"
#include "GeoPackageRaster.h"
#include "GeoPackageFeatureTable.h"

#include <QDir>
#include <QtCore/qglobal.h>

#ifdef Q_OS_IOS
#include <QStandardPaths>
#endif // Q_OS_IOS

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
  QString defaultDataPath()
  {
    QString dataPath;

  #ifdef Q_OS_ANDROID
    dataPath = "/sdcard";
  #elif defined Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  #else
    dataPath = QDir::homePath();
  #endif

    return dataPath;
  }
} // namespace

ReadGeoPackage::ReadGeoPackage(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_dataPath(defaultDataPath() + "/ArcGIS/Runtime/Data")
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

// Read the GeoPackage and create lists for the names and layer data
void ReadGeoPackage::readGeoPackage()
{
  // Load the GeoPackage at the beginning
  GeoPackage* auroraGpkg = new GeoPackage(m_dataPath + "/gpkg/AuroraCO.gpkg", this);

  // Make sure there are no errors in loading the GeoPackage before interacting with it
  connect(auroraGpkg, &GeoPackage::doneLoading, this, [auroraGpkg, this](Error error)
  {
    if (error.isEmpty())
    {      
      // For each raster, create a raster layer and add the layer to the map
      for (GeoPackageRaster* rasterItm : auroraGpkg->geoPackageRasters())
      {
        RasterLayer* rasterLyr = new RasterLayer(rasterItm, this);
        rasterLyr->setOpacity(0.55f);
        m_map->operationalLayers()->append(rasterLyr);
      }

      // For each feature table, create a feature layer and add the layer to the map
      for (GeoPackageFeatureTable* featureTbl : auroraGpkg->geoPackageFeatureTables())
      {
        FeatureLayer* layer = new FeatureLayer(featureTbl, this);
        m_map->operationalLayers()->append(layer);
      }
    }
  });

  auroraGpkg->load();
}
