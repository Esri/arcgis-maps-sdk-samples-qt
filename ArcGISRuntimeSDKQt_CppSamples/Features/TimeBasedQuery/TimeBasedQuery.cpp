// [WriteFile Name=TimeBasedQuery, Category=Features]
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

#include "TimeBasedQuery.h"

#include "Map.h"
#include "MapQuickView.h"
#include "ServiceFeatureTable.h"
#include "FeatureLayer.h"
#include "TimeExtent.h"
#include "QueryParameters.h"

#include <QDateTime>

using namespace Esri::ArcGISRuntime;

TimeBasedQuery::TimeBasedQuery(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void TimeBasedQuery::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<TimeBasedQuery>("Esri.Samples", 1, 0, "TimeBasedQuerySample");
}

void TimeBasedQuery::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the oceans basemap
  m_map = new Map(Basemap::oceans(this), this);

  // Create feature table
  ServiceFeatureTable* hurricaneTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/Hurricanes/MapServer/0"), this);
  hurricaneTable->setFeatureRequestMode(FeatureRequestMode::ManualCache); // set the cache mode to manual

  // Create the feature layer
  FeatureLayer* hurricaneLayer = new FeatureLayer(hurricaneTable, this);
  m_map->operationalLayers()->append(hurricaneLayer);

  // Create DateTime ranges anything prior to Sep 16, 2000
  QDateTime startTime(QDate(0, 0, 0));
  QDateTime endTime(QDate(2000, 9, 16));
  TimeExtent timeExtent(startTime, endTime);

  // Create Query Parameters and set time extent
  QueryParameters queryParams;
  queryParams.setTimeExtent(timeExtent);

  // Populate the feature table
  constexpr bool clearCache = true;
  const QStringList outFields{"*"};
  hurricaneTable->populateFromService(queryParams, clearCache, outFields);

  // Set map to map view
  m_mapView->setMap(m_map);
}
