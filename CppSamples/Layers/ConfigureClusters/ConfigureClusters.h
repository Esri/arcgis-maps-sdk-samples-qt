// [WriteFile Name=ConfigureClusters, Category=Layers]
// [Legal]
// Copyright 2023 Esri.
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

#ifndef ConfigureClusters_H
#define ConfigureClusters_H

// Qt headers
#include "Popup.h"
#include <QObject>

namespace Esri::ArcGISRuntime
{
  class AggregateGeoElement;
  class ClusteringFeatureReduction;
  class FeatureLayer;
  class Map;
  class MapQuickView;
  class Popup;
}

class QMouseEvent;

Q_MOC_INCLUDE("MapQuickView.h");

class ConfigureClusters : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(double mapScale READ mapScale NOTIFY mapScaleChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::Popup* popup READ popup NOTIFY popupChanged)

public:
  explicit ConfigureClusters(QObject* parent = nullptr);
  ~ConfigureClusters() override;

  static void init();

  Q_INVOKABLE void clearSelection() const;
  Q_INVOKABLE void displayLabels(bool checked);
  Q_INVOKABLE void drawClusters();
  Q_INVOKABLE void setClusterRadius(double clusterRadius);
  Q_INVOKABLE void setMaxScale(double maxScale);

signals:
  void mapViewChanged();
  void mapScaleChanged();
  void popupChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  double mapScale() const;

  void createCustomFeatureReduction();

  void mouseClicked(QMouseEvent& mouseEvent);


  Esri::ArcGISRuntime::Popup* popup();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_layer = nullptr;
  Esri::ArcGISRuntime::ClusteringFeatureReduction* m_clusteringFeatureReduction = nullptr;
  Esri::ArcGISRuntime::Popup* m_popup = nullptr;

  QScopedPointer<QObject> m_resultParent;
  Esri::ArcGISRuntime::AggregateGeoElement* m_aggregateGeoElement = nullptr;
};

#endif // ConfigureClusters_H
