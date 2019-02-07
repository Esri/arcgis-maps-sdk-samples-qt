// [WriteFile Name=UpdateGeometryFeatureService, Category=EditData]
// [Legal]
// Copyright 2016 Esri.

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

#ifndef UPDATE_GEOMETRY_FEATURE_SERVICE_H
#define UPDATE_GEOMETRY_FEATURE_SERVICE_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class FeatureLayer;
    class ServiceFeatureTable;
    class Feature;
  }
}

#include <QQuickItem>

class UpdateGeometryFeatureService : public QQuickItem
{
  Q_OBJECT

public:
  explicit UpdateGeometryFeatureService(QQuickItem* parent = nullptr);
  ~UpdateGeometryFeatureService() override;

  void componentComplete() override;
  static void init();

private:
  void connectSignals();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable = nullptr;
  Esri::ArcGISRuntime::Feature* m_selectedFeature = nullptr;
  bool m_featureSelected = false;
};

#endif // UPDATE_GEOMETRY_FEATURE_SERVICE_H

