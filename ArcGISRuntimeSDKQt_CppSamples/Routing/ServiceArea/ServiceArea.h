// [WriteFile Name=ServiceArea, Category=Routing]
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

#ifndef SERVICEAREA_H
#define SERVICEAREA_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class GraphicsOverlay;
    class Map;
    class MapQuickView;
    class Point;
    class PolylineBuilder;
    class ServiceAreaTask;
  }
}

#include "ServiceAreaParameters.h"

#include <QList>
#include <QQuickItem>

class ServiceArea : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
  Q_PROPERTY(QString message READ message NOTIFY messageChanged)

public:
  explicit ServiceArea(QQuickItem* parent = nullptr);
  ~ServiceArea() override;

  void componentComplete() override;
  static void init();

  Q_INVOKABLE void setFacilityMode();
  Q_INVOKABLE void setBarrierMode();
  Q_INVOKABLE void solveServiceArea();
  Q_INVOKABLE void reset();
  Q_INVOKABLE void newBarrier();

signals:
  void busyChanged();
  void messageChanged();

private:
  enum class SampleMode {
    Facility,
    Barrier
  };

  bool busy() const;
  QString message() const;

  void setBusy(bool val);

  void setupGraphics();
  void setupRouting();
  void handleFacilityPoint(const Esri::ArcGISRuntime::Point& p);
  void handleBarrierPoint(const Esri::ArcGISRuntime::Point& p);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::ServiceAreaTask* m_task = nullptr;
  Esri::ArcGISRuntime::ServiceAreaParameters m_parameters;
  Esri::ArcGISRuntime::PolylineBuilder* m_barrierBuilder = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_facilitiesOverlay = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_barrierOverlay = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_areasOverlay = nullptr;
  bool m_busy = true;
  QString m_message;
  SampleMode m_mode = SampleMode::Facility;
  QObject* m_graphicParent = nullptr;
};

#endif // SERVICEAREA_H
