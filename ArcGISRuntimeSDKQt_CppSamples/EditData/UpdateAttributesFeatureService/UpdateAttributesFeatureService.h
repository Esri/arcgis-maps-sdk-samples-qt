// [WriteFile Name=UpdateAttributesFeatureService, Category=EditData]
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

#ifndef UPDATE_ATTRIBUTES_FEATURE_SERVICE_H
#define UPDATE_ATTRIBUTES_FEATURE_SERVICE_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class FeatureLayer;
    class ServiceFeatureTable;
    class ArcGISFeature;
  }
}

class QString;

#include <QQuickItem>

class UpdateAttributesFeatureService : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(QString featureType READ featureType NOTIFY featureTypeChanged)

public:
  explicit UpdateAttributesFeatureService(QQuickItem* parent = nullptr);
  ~UpdateAttributesFeatureService() override;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void updateSelectedFeature(QString fieldVal);

signals:  
  void featureSelected();
  void featureTypeChanged();
  void hideWindow();

private:
  void connectSignals();
  QString featureType() const;

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable = nullptr;
  Esri::ArcGISRuntime::ArcGISFeature* m_selectedFeature = nullptr;
  QString m_featureType;
  QMetaObject::Connection m_featureLoadStatusChangedConnection;
};

#endif // UPDATE_ATTRIBUTES_FEATURE_SERVICE_H

