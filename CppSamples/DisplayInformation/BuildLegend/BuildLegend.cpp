// [WriteFile Name=BuildLegend, Category=DisplayInformation]
// [Legal]
// Copyright 2016 Esri.
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
#include "BuildLegend.h"

// ArcGIS Maps SDK headers
#include "ArcGISMapImageLayer.h"
#include "Basemap.h"
#include "FeatureLayer.h"
#include "LayerListModel.h"
#include "LegendInfoListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "ServiceFeatureTable.h"
#include "SpatialReference.h"
#include "Viewpoint.h"

// Qt headers
#include <QUrl>

using namespace Esri::ArcGISRuntime;

BuildLegend::BuildLegend(QQuickItem* parent) :
  QQuickItem(parent)
{
}

BuildLegend::~BuildLegend() = default;

void BuildLegend::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<BuildLegend>("Esri.Samples", 1, 0, "BuildLegendSample");
  qmlRegisterUncreatableType<QAbstractListModel>("Esri.Samples", 1, 0, "AbstractListModel", "AbstractListModel is uncreateable");
}

void BuildLegend::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // create a new basemap instance
  Basemap* basemap = new Basemap(BasemapStyle::ArcGISTopographic, this);
  // create a new map instance
  m_map = new Map(basemap, this);
  // set map to auto fetch LegendInfo
  m_map->setAutoFetchLegendInfos(true);
  // set initial viewpoint
  m_map->setInitialViewpoint(Viewpoint(Point(-11e6, 6e6, SpatialReference(3857)), 9e7));
  // set map on the map view
  m_mapView->setMap(m_map);

  addLayers();

  // set the legend info list model
  m_legendInfoListModel = m_map->legendInfos();
  emit legendInfoListModelChanged();
}

void BuildLegend::addLayers()
{
  ArcGISMapImageLayer* mapImageLayer = new ArcGISMapImageLayer(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/Census/MapServer"), this);
  m_map->operationalLayers()->append(mapImageLayer);

  ServiceFeatureTable* featureTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/Recreation/FeatureServer/0"), this);
  FeatureLayer* featureLayer = new FeatureLayer(featureTable, this);
  m_map->operationalLayers()->append(featureLayer);
}
