// [WriteFile Name=FenceGraphicsDemo, Category=Analysis]
// [Legal]
// Copyright 2022 Esri.

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

#ifndef FENCEGRAPHICSDEMO_H
#define FENCEGRAPHICSDEMO_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class Graphic;
class GraphicsOverlay;
class LocationGeotriggerFeed;
class SimulatedLocationDataSource;
class SimpleFillSymbol;
}
}

#include <QObject>
#include <Point.h>
#include <Polygon.h>
#include <Polyline.h>

class FenceGraphicsDemo : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  explicit FenceGraphicsDemo(QObject* parent = nullptr);
  ~FenceGraphicsDemo();

  static void init();

signals:
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void initGeometries();
  void usePolygonFence();
  void usePointBufferFence();
  void runGeotriggers();
  void initializeSimulatedLocationDisplay();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

  Esri::ArcGISRuntime::Polygon m_polygon;
  Esri::ArcGISRuntime::Point m_point;
  Esri::ArcGISRuntime::Polyline m_path;

  Esri::ArcGISRuntime::Graphic* m_fenceGraphic = nullptr;
  Esri::ArcGISRuntime::SimpleFillSymbol* m_bufferSymbol = nullptr;

  int m_horizontalAccuracy = 0;

  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;

  Esri::ArcGISRuntime::SimulatedLocationDataSource* m_simulatedLocationDataSource = nullptr;

  Esri::ArcGISRuntime::LocationGeotriggerFeed* m_geotriggerFeed = nullptr;

  int m_bufferSize;
};

#endif // FENCEGRAPHICSDEMO_H
