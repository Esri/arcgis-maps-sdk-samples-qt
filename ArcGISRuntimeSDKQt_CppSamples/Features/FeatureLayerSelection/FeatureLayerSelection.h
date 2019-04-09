// [WriteFile Name=FeatureLayerSelection, Category=Features]
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

#ifndef FEATURE_LAYER_SELECTION_H
#define FEATURE_LAYER_SELECTION_H

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

class QString;

#include <QQuickItem>

class FeatureLayerSelection : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(QString selectedFeatureText READ selectedFeatureText NOTIFY selectedFeatureTextChanged)

public:
  explicit FeatureLayerSelection(QQuickItem* parent = nullptr);
  ~FeatureLayerSelection() override;

  void componentComplete() override;
  static void init();

signals:
  void selectedFeatureTextChanged();

private:
  void connectSignals();
  QString selectedFeatureText() const;

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable = nullptr;
  QString m_selectedFeatureText = QStringLiteral("Click or tap to select features.");
};

#endif // FEATURE_LAYER_SELECTION_H
