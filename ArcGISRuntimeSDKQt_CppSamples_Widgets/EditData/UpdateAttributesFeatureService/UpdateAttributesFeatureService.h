// [WriteFile Name=UpdateAttributesFeatureService, Category=EditData]
// [Legal]
// Copyright 2015 Esri.

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

#ifndef UPDATE_ATTRIBUTES_FEATURES_FEATURE_SERVICE_H
#define UPDATE_ATTRIBUTES_FEATURES_FEATURE_SERVICE_H

#include <QWidget>

namespace Esri {
    namespace ArcGISRuntime {
        class Map;
        class MapGraphicsView;
        class FeatureLayer;
        class ServiceFeatureTable;
        class Feature;
    }
}

class QInputDialog;

class UpdateAttributesFeatureService : public QWidget
{
  Q_OBJECT

public:
  explicit UpdateAttributesFeatureService(QWidget* parent = 0);
  ~UpdateAttributesFeatureService();

private:
  void createUi();
  void connectSignals();

private:
  Esri::ArcGISRuntime::Map* m_map;
  Esri::ArcGISRuntime::MapGraphicsView* m_mapView;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable;
  Esri::ArcGISRuntime::Feature* m_selectedFeature;
  QInputDialog* m_inputDialog;
};

#endif // UPDATE_ATTRIBUTES_FEATURES_FEATURE_SERVICE_H
