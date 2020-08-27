// [WriteFile Name=ShowLocationHistory, Category=Maps]
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

#ifndef SHOWLOCATIONHISTORY_H
#define SHOWLOCATIONHISTORY_H

#include "Point.h"

#include <QObject>

namespace Esri
{
namespace ArcGISRuntime
{
class Graphic;
class GraphicsOverlay;
class Map;
class MapQuickView;
class PolylineBuilder;
class SimulatedLocationDataSource;
}
}

class ShowLocationHistory : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool trackingEnabled MEMBER m_trackingEnabled NOTIFY trackingEnabledChanged)

public:
  explicit ShowLocationHistory(QObject* parent = nullptr);
  ~ShowLocationHistory();

  static void init();

signals:
  void mapViewChanged();
  void trackingEnabledChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void handleLocationChanges();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_locationHistoryOverlay = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_locationHistoryLineOverlay = nullptr;
  Esri::ArcGISRuntime::Graphic* m_locationHistoryLineGraphic = nullptr;
  Esri::ArcGISRuntime::PolylineBuilder* m_polylineBuilder = nullptr;
  Esri::ArcGISRuntime::SimulatedLocationDataSource* m_simulatedLocationDataSource = nullptr;
  Esri::ArcGISRuntime::Point m_lastPosition;
  bool m_trackingEnabled = false;
};

#endif // SHOWLOCATIONHISTORY_H
