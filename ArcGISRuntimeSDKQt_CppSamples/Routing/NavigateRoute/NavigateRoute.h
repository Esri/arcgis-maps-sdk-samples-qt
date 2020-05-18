// [WriteFile Name=NavigateRoute, Category=Routing]
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

#ifndef NAVIGATEROUTE_H
#define NAVIGATEROUTE_H

#include "Route.h"
#include "RouteResult.h"

namespace Esri
{
namespace ArcGISRuntime
{
class Graphic;
class GraphicsOverlay;
class Map;
class MapQuickView;
class RouteTask;
class RouteTracker;
class SimulatedLocationDataSource;
}
}

#include <QObject>
#include <QString>
class QTextToSpeech;

class NavigateRoute : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool navigationEnabled READ navigationEnabled NOTIFY navigationEnabledChanged)
  Q_PROPERTY(bool recenterEnabled READ recenterEnabled NOTIFY recenterEnabledChanged)
  Q_PROPERTY(QString textString READ textString NOTIFY textStringChanged)

public:
  explicit NavigateRoute(QObject* parent = nullptr);
  ~NavigateRoute();

  static void init();

  Q_INVOKABLE void startNavigation();
  Q_INVOKABLE void recenterMap();

signals:
  void mapViewChanged();
  void navigationEnabledChanged();
  void recenterEnabledChanged();
  void textStringChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  bool navigationEnabled() const;
  bool recenterEnabled() const;
  QString textString() const;
  void connectRouteTaskSignals();
  void connectRouteTrackerSignals();

  Esri::ArcGISRuntime::Graphic* m_routeAheadGraphic = nullptr;
  Esri::ArcGISRuntime::Graphic* m_routeTraveledGraphic = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_routeOverlay = nullptr;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::RouteTask* m_routeTask = nullptr;
  Esri::ArcGISRuntime::Route m_route;
  Esri::ArcGISRuntime::RouteResult m_routeResult;
  Esri::ArcGISRuntime::RouteTracker* m_routeTracker = nullptr;
  Esri::ArcGISRuntime::SimulatedLocationDataSource* m_simulatedLocationDataSource = nullptr;
  QAbstractListModel* m_directions = nullptr;
  bool m_navigationEnabled = false;
  bool m_recenterEnabled = false;
  QString m_textString = "";
  QTextToSpeech* m_speaker = nullptr;
};

#endif // NAVIGATEROUTE_H
