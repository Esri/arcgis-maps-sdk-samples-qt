// [WriteFile Name=ReadGeopackage, Category=Maps]
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

#include "ReadGeopackage.h"

#include "Map.h"
#include "MapQuickView.h"

#include <QDir>

using namespace Esri::ArcGISRuntime;
using namespace std;

ReadGeopackage::ReadGeopackage(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void ReadGeopackage::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ReadGeopackage>("Esri.Samples", 1, 0, "ReadGeopackageSample");
}

void ReadGeopackage::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the topographic basemap
  m_map = new Map(Basemap::topographic(this), this);

  // Set map to map view
  m_mapView->setMap(m_map);

  // Create a member variable to contain the layers
  QList<Layer*> m_layerList;

  // Create the initial view area
  Viewpoint initialViewpoint(39.7294, -104.8319, 500000);
  m_mapView->setViewpoint(initialViewpoint);

  // Initialize the read operation
  listMaps();
}

// Read the geopackage and create lists for the names and layer data
void ReadGeopackage::listMaps()
{

  // Load the geopackagea the beginning
  QString homedir = QDir::homePath();
  const QString path = homedir + "/ArcGIS/Runtime/Data/gpkg/AuroraCO.gpkg";
  GeoPackage* auroraGpkg = new GeoPackage(path, this);
  auroraGpkg->load();
  m_layerNamesList.clear();

  // Make sure there are no errors in loading the geopackage before interacting with it
  connect(auroraGpkg, &GeoPackage::doneLoading, this, [auroraGpkg, this](Error error)
  {
    if (error.isEmpty())
    {

      // Create two QLists for the different types of layer data within the geopackage (these are combined into m_layerList later on)
      QList<GeoPackageRaster*> gpkgRasters = auroraGpkg->geoPackageRasters();
      QList<GeoPackageFeatureTable*> gpkgFeatures = auroraGpkg->geoPackageFeatureTables();

      // Loop through both lists to get the name and layer data. The name data is only used to populate the menu.
      for (const auto& featureTbl : gpkgFeatures)
      {
        QVariantMap layerMap;
        FeatureLayer* layer = new FeatureLayer(featureTbl, this);
        layerMap["name"] = layer->name();
        m_layerList.append(layer);
        m_layerNamesList << layerMap;
      }
      for (const auto& rasterItm : gpkgRasters)
      {
        QVariantMap rasterMap;
        RasterLayer* rasterLyr = new RasterLayer(rasterItm, this);
        rasterMap["name"] = rasterLyr->raster()->path().mid(55, 25);
        m_layerList.append(rasterLyr);
        m_layerNamesList << rasterMap;
      }
      emit layerNamesListChanged();
    }
  });
}

// Getter for the menu names data
QVariantList ReadGeopackage::layerNamesList() const
{
  return m_layerNamesList;
}

// Called by the QML to toggle visibility at a certain index
void ReadGeopackage::addOrShowLayer(int index, bool onOff)
{
  auto layer = m_layerList[index];
  int indexOfLayer = m_map->operationalLayers()->indexOf(layer);
  if(indexOfLayer != -1 && onOff == false)
  {
    m_map->operationalLayers()->at(indexOfLayer)->setVisible(false);
  } else {
    m_map->operationalLayers()->insert(index, layer);
    layer->setVisible(true);
  }
}

