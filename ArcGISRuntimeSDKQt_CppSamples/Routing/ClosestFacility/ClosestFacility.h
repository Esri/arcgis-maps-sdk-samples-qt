// [WriteFile Name=ClosestFacility, Category=Routing]
// [Legal]
// Copyright 2017 Esri.

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

#ifndef CLOSESTFACILITY_H
#define CLOSESTFACILITY_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class ClosestFacilityTask;
    class Facility;
    class GraphicsOverlay;
    class Graphic;
    class Map;
    class MapQuickView;
    class Point;
  }
}

#include "ClosestFacilityParameters.h"

#include <QList>
#include <QQuickItem>
#include <QUrl>

class ClosestFacility : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
  Q_PROPERTY(QString message READ message NOTIFY messageChanged)

public:
  explicit ClosestFacility(QQuickItem* parent = nullptr);
  ~ClosestFacility() override;

  void componentComplete() override;
  static void init();

signals:
  void busyChanged();
  void messageChanged();

private:
  bool busy() const;
  QString message() const;

  void setBusy(bool val);
  void createFacilities();
  void createGraphics();
  void setupRouting();
  void solveRoute(const Esri::ArcGISRuntime::Point& incidentPoint);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  QList<Esri::ArcGISRuntime::Facility> m_facilities;
  Esri::ArcGISRuntime::ClosestFacilityTask* m_task = nullptr;
  Esri::ArcGISRuntime::ClosestFacilityParameters m_facilityParams;
  Esri::ArcGISRuntime::GraphicsOverlay* m_facilityOverlay = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_incidentsOverlay = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_resultsOverlay = nullptr;
  Esri::ArcGISRuntime::Graphic* m_routeGraphic = nullptr;
  Esri::ArcGISRuntime::Graphic* m_incidentGraphic = nullptr;
  bool m_busy = false;
  QString m_message;  

  static const QUrl facilityImageUrl;
  static const QUrl sanDiegoRegion;
};

#endif // CLOSESTFACILITY_H
