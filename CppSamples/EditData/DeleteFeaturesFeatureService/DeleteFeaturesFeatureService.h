// [WriteFile Name=DeleteFeaturesFeatureService, Category=EditData]
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

#ifndef DELETE_FEATURES_FEATURE_SERVICE_H
#define DELETE_FEATURES_FEATURE_SERVICE_H

// Qt headers
#include <QQuickItem>

namespace Esri::ArcGISRuntime
{
  class Map;
  class MapQuickView;
  class FeatureLayer;
  class ServiceFeatureTable;
  class Feature;
  class IdentifyLayerResult;
  class FeatureQueryResult;
}

class QString;

class DeleteFeaturesFeatureService : public QQuickItem
{
  Q_OBJECT

public:
  explicit DeleteFeaturesFeatureService(QQuickItem* parent = nullptr);
  ~DeleteFeaturesFeatureService() override;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void deleteSelectedFeature();

signals:
  void featureSelected();
  void hideWindow();

private:
  void connectSignals();
  void onIdentifyLayerCompleted_(Esri::ArcGISRuntime::IdentifyLayerResult* rawIdentifyResult);
  void onSelectFeaturesCompleted_(Esri::ArcGISRuntime::FeatureQueryResult* rawFeatureQueryResult);

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable = nullptr;
  Esri::ArcGISRuntime::Feature* m_selectedFeature = nullptr;
};

#endif // DELETE_FEATURES_FEATURE_SERVICE_H
