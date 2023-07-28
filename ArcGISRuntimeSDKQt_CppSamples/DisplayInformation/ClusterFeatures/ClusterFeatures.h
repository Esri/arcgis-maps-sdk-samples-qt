// [WriteFile Name=ClusterFeatures, Category=DisplayInformation]
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

#ifndef CLUSTERFEATURES_H
#define CLUSTERFEATURES_H

namespace Esri::ArcGISRuntime
{
  class FeatureLayer;
  class Map;
  class MapQuickView;
  class PopupManager;
}

#include <QObject>
#include <QQmlListProperty>
#include <QMouseEvent>

Q_MOC_INCLUDE("MapQuickView.h");

class ClusterFeatures : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QQmlListProperty<Esri::ArcGISRuntime::PopupManager> popupManagers READ popupManagers NOTIFY popupManagersChanged)
  Q_PROPERTY(bool taskRunning READ taskRunning NOTIFY taskRunningChanged)

public:
  explicit ClusterFeatures(QObject* parent = nullptr);
  ~ClusterFeatures() override;

  Q_INVOKABLE bool toggleClustering();

  static void init();

signals:
  void mapViewChanged();
  void popupManagersChanged();
  void taskRunningChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  QQmlListProperty<Esri::ArcGISRuntime::PopupManager> popupManagers();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

  Esri::ArcGISRuntime::FeatureLayer* m_powerPlantsLayer = nullptr;

  Esri::ArcGISRuntime::PopupManager* m_popupManager = nullptr;
  QList<Esri::ArcGISRuntime::PopupManager*> m_popupManagers;



  bool taskRunning() const;
  bool m_taskRunning = true;
};

#endif // CLUSTERFEATURES_H
