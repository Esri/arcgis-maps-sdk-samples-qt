// [WriteFile Name=FeatureCollectionLayerQuery, Category=Layers]
// [Legal]
// Copyright 2017 Esri.

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

#ifndef FEATURECOLLECTIONLAYERQUERY_H
#define FEATURECOLLECTIONLAYERQUERY_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class ServiceFeatureTable;
  }
}

#include <QQuickItem>

class FeatureCollectionLayerQuery : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)

public:
  explicit FeatureCollectionLayerQuery(QQuickItem* parent = nullptr);
  ~FeatureCollectionLayerQuery() override = default;

  void componentComplete() override;
  static void init();

signals:
  void busyChanged();

private:
  bool busy() const;
  void setBusy(bool val);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable = nullptr;
  bool m_busy = false;
};

#endif // FEATURECOLLECTIONLAYERQUERY_H
