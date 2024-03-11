// Copyright 2024 ESRI
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

#ifndef ALLROADSLEADTOROME_H
#define ALLROADSLEADTOROME_H

namespace Esri::ArcGISRuntime {
class DirectionManeuverListModel;
class Graphic;
class GraphicsOverlay;
class LocatorTask;
class Map;
class MapQuickView;
class SimpleMarkerSymbol;
class RouteTask;
} // namespace Esri::ArcGISRuntime

#include <QObject>

Q_MOC_INCLUDE("MapQuickView.h")
Q_MOC_INCLUDE("DirectionManeuverListModel.h")

class AllRoadsLeadToRome : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  explicit AllRoadsLeadToRome(QObject* parent = nullptr);
  ~AllRoadsLeadToRome() override;

signals:
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  Esri::ArcGISRuntime::Graphic* m_romeGraphic = nullptr;
  Esri::ArcGISRuntime::Graphic* m_startGraphic = nullptr;
  Esri::ArcGISRuntime::Graphic* m_routeLine = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::LocatorTask* m_locatorTask = nullptr;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::SimpleMarkerSymbol* m_markerSymbol = nullptr;
  Esri::ArcGISRuntime::RouteTask* m_routeTask = nullptr;
};

#endif // ALLROADSLEADTOROME_H
