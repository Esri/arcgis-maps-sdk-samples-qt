// [WriteFile Name=AddClusteringFeatureReductionToAPointFeatureLayer, Category=Layers]
// [Legal]
// Copyright 2023 Esri.

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

#ifndef AddClusteringFeatureReductionToAPointFeatureLayer_H
#define AddClusteringFeatureReductionToAPointFeatureLayer_H

namespace Esri::ArcGISRuntime
{
  class ClusteringFeatureReduction;
  class FeatureLayer;
  class Map;
  class MapQuickView;
  class PopupManager;
}

#include <QObject>

Q_MOC_INCLUDE("MapQuickView.h");

class AddClusteringFeatureReductionToAPointFeatureLayer : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(qreal mapScale READ mapScale NOTIFY mapScaleChanged)
  Q_PROPERTY(QQmlListProperty<Esri::ArcGISRuntime::PopupManager> popupManagers READ popupManagers NOTIFY popupManagersChanged)

public:
  explicit AddClusteringFeatureReductionToAPointFeatureLayer(QObject* parent = nullptr);
  ~AddClusteringFeatureReductionToAPointFeatureLayer() override;

  static void init();

  Q_INVOKABLE void displayLabels(bool checked);
  Q_INVOKABLE void drawClusters();
  Q_INVOKABLE void setClusterRadius(qreal clusterRadius);
  Q_INVOKABLE void setMaxScale(qreal maxScale);

signals:
  void mapViewChanged();
  void mapScaleChanged();
  void popupManagersChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  qreal mapScale() const;

  void createCustomFeatureReduction();

  void mouseClicked(QMouseEvent& mouseEvent);

  QQmlListProperty<Esri::ArcGISRuntime::PopupManager> popupManagers();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_layer = nullptr;
  Esri::ArcGISRuntime::ClusteringFeatureReduction* m_clusteringFeatureReduction = nullptr;
  QList<Esri::ArcGISRuntime::PopupManager*> m_popupManagers;
};

#endif // AddClusteringFeatureReductionToAPointFeatureLayer_H
