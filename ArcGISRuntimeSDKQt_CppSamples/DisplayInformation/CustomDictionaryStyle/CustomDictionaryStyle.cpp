// [WriteFile Name=CustomDictionaryStyle, Category=DisplayInformation]
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

#include "CustomDictionaryStyle.h"

#include "DictionaryRenderer.h"
#include "DictionarySymbolStyle.h"
#include "FeatureLayer.h"
#include "Map.h"
#include "MapQuickView.h"
#include "ServiceFeatureTable.h"
#include "Viewpoint.h"

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

CustomDictionaryStyle::CustomDictionaryStyle(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::streetsVector(this), this))
{
  // Set an initial viewpoint
  Viewpoint vp(Point(-1.304630524635E7, 4036698.1412000023, SpatialReference(3857)), 5000);
  m_map->setInitialViewpoint(vp);

  // Create a feature layer from a feature service and it to the map
  ServiceFeatureTable* featureTable = new ServiceFeatureTable(QUrl("https://services2.arcgis.com/ZQgQTuoyBrtmoGdP/arcgis/rest/services/Redlands_Restaurants/FeatureServer/0"), this);
  FeatureLayer* featureLayer = new FeatureLayer(featureTable, this);
  m_map->operationalLayers()->append(featureLayer);

  // Set a Dictionary Renderer on the Feature Layer
  DictionarySymbolStyle* dictionaryStyle = DictionarySymbolStyle::createFromFile(defaultDataPath() + "/ArcGIS/Runtime/Data/styles/arcade_style/Restaurant.stylx", this);
  DictionaryRenderer* dictionaryRenderer = new DictionaryRenderer(dictionaryStyle, this);
  featureLayer->setRenderer(dictionaryRenderer);
}

CustomDictionaryStyle::~CustomDictionaryStyle() = default;

void CustomDictionaryStyle::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<CustomDictionaryStyle>("Esri.Samples", 1, 0, "CustomDictionaryStyleSample");
}

MapQuickView* CustomDictionaryStyle::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void CustomDictionaryStyle::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}
