// [WriteFile Name=PerformValveIsolationTrace, Category=UtilityNetwork]
// [Legal]
// Copyright 2020 Esri.

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

#ifndef PERFORMVALVEISOLATIONTRACE_H
#define PERFORMVALVEISOLATIONTRACE_H

namespace Esri
{
namespace ArcGISRuntime
{
class FeatureLayer;
class GraphicsOverlay;
class Map;
class MapQuickView;
class UtilityCategory;
class UtilityElement;
class UtilityNetwork;
class UtilityTraceConfiguration;
}
}

#include <QObject>

class PerformValveIsolationTrace : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QStringList categoriesList READ categoriesList NOTIFY categoriesListChanged)
  Q_PROPERTY(int selectedIndex MEMBER m_selectedIndex NOTIFY selectedIndexChanged)
  Q_PROPERTY(bool isolateFeatures MEMBER m_isolateFeatures NOTIFY isolateFeaturesChanged)
  Q_PROPERTY(bool tasksRunning READ tasksRunning NOTIFY tasksRunningChanged)
  Q_PROPERTY(bool noResults READ noResults NOTIFY noResultsChanged)

public:
  explicit PerformValveIsolationTrace(QObject* parent = nullptr);
  ~PerformValveIsolationTrace();

  static void init();
  Q_INVOKABLE void performTrace();

signals:
  void mapViewChanged();
  void categoriesListChanged();
  void selectedIndexChanged();
  void isolateFeaturesChanged();
  void tasksRunningChanged();
  void noResultsChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  QStringList categoriesList() const;
  bool tasksRunning() const;
  bool noResults() const;
  void connectSignals();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_startingLocationOverlay = nullptr;
  Esri::ArcGISRuntime::UtilityElement* m_startingLocation = nullptr;
  Esri::ArcGISRuntime::UtilityNetwork* m_utilityNetwork = nullptr;
  Esri::ArcGISRuntime::UtilityTraceConfiguration* m_traceConfiguration = nullptr;
  QStringList m_categoriesList;
  int m_selectedIndex = -1;
  bool m_isolateFeatures = false;
  bool m_tasksRunning = false;
  bool m_traceRunning = false;
  bool m_noResults = false;
};

#endif // PERFORMVALVEISOLATIONTRACE_H
