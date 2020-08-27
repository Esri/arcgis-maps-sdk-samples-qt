// [WriteFile Name=FindServiceAreasForMultipleFacilities, Category=Routing]
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

#ifndef FINDSERVICEAREASFORMULTIPLEFACILITIES_H
#define FINDSERVICEAREASFORMULTIPLEFACILITIES_H

#include "TaskWatcher.h"

namespace Esri
{
namespace ArcGISRuntime
{
class FeatureLayer;
class GraphicsOverlay;
class Map;
class MapQuickView;
class ServiceAreaTask;
class ServiceFeatureTable;
class SimpleFillSymbol;
}
}

#include <QObject>

class FindServiceAreasForMultipleFacilities : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool taskRunning READ taskRunning NOTIFY taskRunningChanged)

public:
  explicit FindServiceAreasForMultipleFacilities(QObject* parent = nullptr);
  ~FindServiceAreasForMultipleFacilities();

  static void init();

  Q_INVOKABLE void findServiceAreas();

signals:
  void mapViewChanged();
  void taskRunningChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  bool taskRunning() const;
  void connectServiceAreaTaskSignals();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_facilitiesFeatureLayer = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_serviceAreasOverlay = nullptr;
  Esri::ArcGISRuntime::ServiceAreaTask* m_serviceAreaTask = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_facilitiesTable = nullptr;
  QList<Esri::ArcGISRuntime::SimpleFillSymbol*> m_fillSymbols;
  bool m_taskRunning = false;
  Esri::ArcGISRuntime::TaskWatcher m_taskWatcher;
};

#endif // FINDSERVICEAREASFORMULTIPLEFACILITIES_H
