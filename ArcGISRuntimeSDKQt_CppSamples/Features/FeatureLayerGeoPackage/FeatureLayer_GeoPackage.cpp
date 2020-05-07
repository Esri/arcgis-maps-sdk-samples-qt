// [WriteFile Name=FeatureLayer_GeoPackage, Category=Features]
// [Legal]
// Copyright 2017 Esri.

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

#include "FeatureLayer_GeoPackage.h"

#include "Map.h"
#include "MapQuickView.h"
#include "GeoPackage.h"
#include "GeoPackageFeatureTable.h"
#include "FeatureLayer.h"
#include "Point.h"
#include "Viewpoint.h"

#include <QUrl>
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

FeatureLayer_GeoPackage::FeatureLayer_GeoPackage(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void FeatureLayer_GeoPackage::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<FeatureLayer_GeoPackage>("Esri.Samples", 1, 0, "FeatureLayer_GeoPackageSample");
}

void FeatureLayer_GeoPackage::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the light gray basemap
  m_map = new Map(Basemap::lightGrayCanvas(this), this);

  // Set an initial viewpoint
  const Point pt(-104.8319, 39.7294, SpatialReference(4326));
  const Viewpoint vp(pt, 200000);
  m_map->setInitialViewpoint(vp);

  // Create the GeoPackage
  const QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/gpkg/";
  GeoPackage* gpkg = new GeoPackage(dataPath + "AuroraCO.gpkg", this);

  // Connect to GeoPackage::doneLoading
  connect(gpkg, &GeoPackage::doneLoading, this, [this, gpkg](Error e)
  {
    if (!e.isEmpty())
      return;

    if (!(gpkg->geoPackageFeatureTables().size() > 0))
      return;

    GeoPackageFeatureTable* gpkgFeatureTable = gpkg->geoPackageFeatureTables().at(0);
    FeatureLayer* featureLayer = new FeatureLayer(gpkgFeatureTable, this);
    m_map->operationalLayers()->append(featureLayer);
  });

  // Connect to Map::doneLoading
  connect(m_map, &Map::doneLoading, this, [gpkg](Error e)
  {
    if (!e.isEmpty())
      return;

    // load the GeoPackage
    gpkg->load();
  });

  // Set map to map view
  m_mapView->setMap(m_map);
}
