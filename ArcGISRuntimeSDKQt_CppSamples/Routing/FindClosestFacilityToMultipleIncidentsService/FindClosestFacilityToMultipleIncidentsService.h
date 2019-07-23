// [WriteFile Name=FindClosestFacilityToMultipleIncidentsService, Category=Routing]
// [Legal]
// Copyright 2019 Esri.

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

#ifndef FINDCLOSESTFACILITYTOMULTIPLEINCIDENTSSERVICE_H
#define FINDCLOSESTFACILITYTOMULTIPLEINCIDENTSSERVICE_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class ServiceFeatureTable;
class FeatureLayer;
class GraphicsOverlay;
class Graphic;
class ClosestFacilityTask;
class Facility;
class Incident;
class PictureMarkerSymbol;
class SimpleLineSymbol;
class QueryParameters;
}
}

#include "ClosestFacilityParameters.h"
#include "TaskWatcher.h"
#include <QObject>

class FindClosestFacilityToMultipleIncidentsService : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool busy MEMBER m_busy NOTIFY busyChanged)
  Q_PROPERTY(bool solveButton MEMBER m_solveButtonEnabled NOTIFY solveButtonChanged)
  Q_PROPERTY(bool resetButton MEMBER m_resetButtonEnabled NOTIFY resetButtonChanged)

public:
  explicit FindClosestFacilityToMultipleIncidentsService(QObject* parent = nullptr);
  ~FindClosestFacilityToMultipleIncidentsService();

  static void init();
  Q_INVOKABLE void solveRoute();
  Q_INVOKABLE void resetGO();

signals:
  void mapViewChanged();
  void busyChanged();
  void solveButtonChanged();
  void resetButtonChanged();

private slots:
  void setViewpointGeometry(const Esri::ArcGISRuntime::Error& e);

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void createSymbols();
  void createFeatureLayers();
  void setupRouting();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_facilitiesFeatureTable = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_incidentsFeatureTable = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_facilitiesFeatureLayer = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_incidentsFeatureLayer = nullptr;
  Esri::ArcGISRuntime::PictureMarkerSymbol* m_facilitySymbol = nullptr;
  Esri::ArcGISRuntime::PictureMarkerSymbol* m_incidentSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleLineSymbol* m_routeSymbol = nullptr;
  Esri::ArcGISRuntime::ClosestFacilityTask* m_task = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_resultsOverlay = nullptr;
  Esri::ArcGISRuntime::TaskWatcher setViewpointTaskWatcher;
  Esri::ArcGISRuntime::ClosestFacilityParameters m_facilityParams;

  bool m_busy = false;
  bool m_solveButtonEnabled = false;
  bool m_resetButtonEnabled = false;
};

#endif // FINDCLOSESTFACILITYTOMULTIPLEINCIDENTSSERVICE_H
