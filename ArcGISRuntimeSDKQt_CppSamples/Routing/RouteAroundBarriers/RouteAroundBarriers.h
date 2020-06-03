// [WriteFile Name=RouteAroundBarriers, Category=Routing]
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

#ifndef ROUTEAROUNDBARRIERS_H
#define ROUTEAROUNDBARRIERS_H

#include "PolygonBarrier.h"
#include "RouteParameters.h"
#include "Stop.h"

namespace Esri
{
namespace ArcGISRuntime
{
class GraphicsOverlay;
class Map;
class MapQuickView;
class PictureMarkerSymbol;
class RouteTask;
class SimpleFillSymbol;
}
}

#include <QObject>

class RouteAroundBarriers : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool addStops MEMBER m_addStops NOTIFY addStopsChanged)
  Q_PROPERTY(bool addBarriers MEMBER m_addBarriers NOTIFY addBarriersChanged)
  Q_PROPERTY(bool findBestSequence MEMBER m_findBestSequence NOTIFY findBestSequenceChanged)
  Q_PROPERTY(bool preserveFirstStop MEMBER m_preserveFirstStop NOTIFY preserveFirstStopChanged)
  Q_PROPERTY(bool preserveLastStop MEMBER m_preserveLastStop NOTIFY preserveLastStopChanged)
  Q_PROPERTY(QAbstractListModel* directions MEMBER m_directions NOTIFY directionsChanged)

public:
  explicit RouteAroundBarriers(QObject* parent = nullptr);
  ~RouteAroundBarriers();

  static void init();

  Q_INVOKABLE void createAndDisplayRoute();
  Q_INVOKABLE void clearRouteAndGraphics();
  Q_INVOKABLE void clearDirections();

signals:
  void mapViewChanged();
  void addStopsChanged();
  void addBarriersChanged();
  void findBestSequenceChanged();
  void preserveFirstStopChanged();
  void preserveLastStopChanged();
  void directionsChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void connectRouteSignals();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_routeOverlay = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_stopsOverlay = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_barriersOverlay = nullptr;
  Esri::ArcGISRuntime::PictureMarkerSymbol* m_pinSymbol = nullptr;
  Esri::ArcGISRuntime::RouteParameters m_routeParameters;
  Esri::ArcGISRuntime::RouteTask* m_routeTask = nullptr;
  Esri::ArcGISRuntime::SimpleFillSymbol* m_barrierSymbol = nullptr;

  QList<Esri::ArcGISRuntime::Stop> m_stopsList;
  QList<Esri::ArcGISRuntime::PolygonBarrier> m_barriersList;
  QAbstractListModel* m_directions = nullptr;
  bool m_addStops = true;
  bool m_addBarriers = false;
  bool m_findBestSequence = false;
  bool m_preserveFirstStop = false;
  bool m_preserveLastStop = false;
};

#endif // ROUTEAROUNDBARRIERS_H
