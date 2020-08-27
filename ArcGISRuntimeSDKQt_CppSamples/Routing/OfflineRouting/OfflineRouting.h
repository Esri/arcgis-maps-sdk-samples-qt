// [WriteFile Name=OfflineRouting, Category=Routing]
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

#ifndef OFFLINEROUTING_H
#define OFFLINEROUTING_H

#include "Envelope.h"
#include "RouteParameters.h"
#include "TaskWatcher.h"

namespace Esri
{
namespace ArcGISRuntime
{
class Graphic;
class GraphicsOverlay;
class Map;
class MapQuickView;
class PictureMarkerSymbol;
class RouteTask;
}
}

#include <QObject>

class OfflineRouting : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QStringList travelModeNames READ travelModeNames NOTIFY travelModeNamesChanged)
  Q_PROPERTY(int travelModeIndex READ travelModeIndex WRITE setTravelModeIndex NOTIFY travelModeIndexChanged)

public:
  explicit OfflineRouting(QObject* parent = nullptr);
  ~OfflineRouting();

  static void init();

  Q_INVOKABLE void findRoute();
  Q_INVOKABLE void resetMap();

  void connectSignals();

signals:
  void mapViewChanged();
  void travelModeNamesChanged();
  void travelModeIndexChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  QStringList travelModeNames() const;
  int travelModeIndex() const;
  void setTravelModeIndex(int index);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

  Esri::ArcGISRuntime::PictureMarkerSymbol* m_pinSymbol = nullptr;
  Esri::ArcGISRuntime::Graphic* m_selectedGraphic = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_stopsOverlay = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_routeOverlay = nullptr;
  Esri::ArcGISRuntime::RouteTask* m_routeTask = nullptr;
  Esri::ArcGISRuntime::RouteParameters m_routeParameters;
  Esri::ArcGISRuntime::TaskWatcher m_taskWatcher;
  int m_travelModeIndex = -1;
  Esri::ArcGISRuntime::Envelope m_routableArea;
};

#endif // OFFLINEROUTING_H
