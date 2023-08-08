// [WriteFile Name=DisplayPointsUsingClusteringFeatureReduction, Category=DisplayInformation]
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

#ifndef DISPLAYPOINTSUSINGCLUSTERINGFEATUREREDUCTION_H
#define DISPLAYPOINTSUSINGCLUSTERINGFEATUREREDUCTION_H

namespace Esri::ArcGISRuntime
{
class FeatureLayer;
class Map;
class MapQuickView;
}

#include <QObject>

Q_MOC_INCLUDE("MapQuickView.h");

class DisplayPointsUsingClusteringFeatureReduction : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool taskRunning READ taskRunning NOTIFY taskRunningChanged)
  Q_PROPERTY(QString calloutText READ calloutText NOTIFY calloutTextChanged)

public:
  explicit DisplayPointsUsingClusteringFeatureReduction(QObject* parent = nullptr);
  ~DisplayPointsUsingClusteringFeatureReduction() override;

  Q_INVOKABLE bool toggleClustering();

  static void init();

signals:
  void mapViewChanged();
  void taskRunningChanged();
  void calloutTextChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  QString calloutText() const;

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

  Esri::ArcGISRuntime::FeatureLayer* m_powerPlantsLayer = nullptr;

  bool taskRunning() const;
  bool m_taskRunning = true;
  QString m_calloutText = "";
};

#endif // DISPLAYPOINTSUSINGCLUSTERINGFEATUREREDUCTION_H
