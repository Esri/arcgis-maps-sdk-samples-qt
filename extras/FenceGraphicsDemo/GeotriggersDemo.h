// Copyright 2022 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#ifndef GEOTRIGGERSDEMO_H
#define GEOTRIGGERSDEMO_H

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

class GeotriggersDemo : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  explicit GeotriggersDemo(QObject* parent = nullptr);
  ~GeotriggersDemo() override;

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

#endif // GEOTRIGGERSDEMO_H
