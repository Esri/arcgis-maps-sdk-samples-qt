// [WriteFile Name=FeatureLayerQuery, Category=Features]
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

#ifndef FEATURE_LAYER_QUERY_H
#define FEATURE_LAYER_QUERY_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class FeatureLayer;
    class ServiceFeatureTable;
  }
}

#include <QQuickItem>

class FeatureLayerQuery : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(bool layerInitialized READ layerInitialized NOTIFY layerInitializedChanged)
  Q_PROPERTY(int queryResultsCount READ queryResultsCount NOTIFY queryResultsCountChanged)

public:
  explicit FeatureLayerQuery(QQuickItem* parent = nullptr);
  ~FeatureLayerQuery() override;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void runQuery(const QString& stateName);

signals:
  void layerInitializedChanged();
  void queryResultsCountChanged();

private:
  void connectSignals();
  bool layerInitialized() const;
  int queryResultsCount() const;

private:
  QString formatStateNameForQuery(const QString& stateName) const;

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable = nullptr;
  bool m_initialized = false;
  int m_queryResultsCount = 0;
};

#endif // FEATURE_LAYER_QUERY_H
