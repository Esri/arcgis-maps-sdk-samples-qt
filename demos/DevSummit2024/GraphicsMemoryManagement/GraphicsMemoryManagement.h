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

#ifndef GRAPHICSMEMORYMANAGEMENT_H
#define GRAPHICSMEMORYMANAGEMENT_H

namespace Esri::ArcGISRuntime {
class GraphicsOverlay;
class Map;
class MapQuickView;
} // namespace Esri::ArcGISRuntime

#include <QObject>

Q_MOC_INCLUDE("MapQuickView.h")

class GraphicsMemoryManagement : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(int graphicsOverlayCount READ graphicsOverlayCount NOTIFY appUpdated)

public:
  explicit GraphicsMemoryManagement(QObject* parent = nullptr);
  ~GraphicsMemoryManagement() override;

  Q_INVOKABLE void badMemoryManagement();
  Q_INVOKABLE void properMemoryManagement();
  Q_INVOKABLE void clearGraphics();

signals:
  void mapViewChanged();
  void appUpdated();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

  int graphicsOverlayCount() const;

  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;

  std::unique_ptr<QObject> m_graphicsParent;
};

#endif // GRAPHICSMEMORYMANAGEMENT_H
