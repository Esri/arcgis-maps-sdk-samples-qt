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
#include "PortalItem.h"

#include <QtCore/qglobal.h>

#ifdef Q_OS_ANDROID
#elif defined Q_OS_IOS
#include <QStandardPaths>
#else
#include <QDir>
#endif

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
  m_map(new Map(BasemapStyle::ArcGISTopographic, this))
{
  // Set an initial viewpoint
  Viewpoint viewpoint(Point(-13'046'305, 4'036'698, SpatialReference(3857)), 5000);
  m_map->setInitialViewpoint(viewpoint);

  // Create a feature layer from a feature service and it to the map
  ServiceFeatureTable* featureTable = new ServiceFeatureTable(QUrl("https://services2.arcgis.com/ZQgQTuoyBrtmoGdP/arcgis/rest/services/Redlands_Restaurants/FeatureServer/0"), this);
  m_featureLayer = new FeatureLayer(featureTable, this);
  m_map->operationalLayers()->append(m_featureLayer);

  // Create a DictionaryRenderer using the local .stylx file
  DictionarySymbolStyle* localDictionaryStyle = DictionarySymbolStyle::createFromFile(defaultDataPath() + "/ArcGIS/Runtime/Data/styles/arcade_style/Restaurant.stylx", this);
  m_localDictionaryRenderer = new DictionaryRenderer(localDictionaryStyle, this);

  // Set initial FeatureLayer renderer to the local DictionaryRenderer
  m_featureLayer->setRenderer(m_localDictionaryRenderer);

  // Create a DictionarySymbolStyle from a portal item, using the default arcgis.com path
  PortalItem* portalItem = new PortalItem("adee951477014ec68d7cf0ea0579c800", this);
  DictionarySymbolStyle* dictSymbStyleFromPortal = new DictionarySymbolStyle(portalItem, this);

  // The source feature layer fields do not match those of the the DictionarySymbolStyle so we create a fieldMap to correct this
  QMap<QString, QString> fieldMap;
  // With the following override, the feature layer's "inspection" field will be mapped to the dictionary symbol style's "healthgrade" field
  fieldMap["healthgrade"] = "inspection";
  m_webDictionaryRenderer = new DictionaryRenderer(dictSymbStyleFromPortal, fieldMap, fieldMap, this);
}

void CustomDictionaryStyle::changeDictionarySymbolStyleSource()
{
  m_featureLayer->setRenderer(m_featureLayer->renderer() == m_webDictionaryRenderer ? m_localDictionaryRenderer : m_webDictionaryRenderer);
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
