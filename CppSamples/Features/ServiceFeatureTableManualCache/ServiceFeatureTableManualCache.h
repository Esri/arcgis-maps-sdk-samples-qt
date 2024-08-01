// [WriteFile Name=ServiceFeatureTableManualCache, Category=Features]
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

#ifndef SERVICE_FEATURE_TABLE_MANUAL_CACHE_H
#define SERVICE_FEATURE_TABLE_MANUAL_CACHE_H

// Qt headers
#include <QQuickItem>

namespace Esri::ArcGISRuntime
{
  class Map;
  class MapQuickView;
  class FeatureLayer;
  class ServiceFeatureTable;
}

class ServiceFeatureTableManualCache : public QQuickItem
{
  Q_OBJECT

public:
  explicit ServiceFeatureTableManualCache(QQuickItem* parent = nullptr);
  ~ServiceFeatureTableManualCache() override;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void populate();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable = nullptr;
};

#endif // SERVICE_FEATURE_TABLE_MANUAL_CACHE_H
