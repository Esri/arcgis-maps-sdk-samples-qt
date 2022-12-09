// [WriteFile Name=FeatureLayerSelection, Category=Features]
// [Legal]
// Copyright 2022 Esri.

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

#ifndef FEATURELAYERSELECTION_H
#define FEATURELAYERSELECTION_H

namespace Esri::ArcGISRuntime
{
class FeatureLayer;
class Map;
class MapQuickView;
class ServiceFeatureTable;
}

#include <QObject>

Q_MOC_INCLUDE("MapQuickView.h");

class FeatureLayerSelection : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QString selectedFeatureText READ selectedFeatureText NOTIFY selectedFeatureTextChanged)

public:
  explicit FeatureLayerSelection(QObject* parent = nullptr);
  ~FeatureLayerSelection() override;

  static void init();

signals:
  void selectedFeatureTextChanged();
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  QString selectedFeatureText() const;

  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void connectSignals();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable = nullptr;

  QString m_selectedFeatureText = QStringLiteral("Click or tap to select features.");
};

#endif // FEATURELAYERSELECTION_H
