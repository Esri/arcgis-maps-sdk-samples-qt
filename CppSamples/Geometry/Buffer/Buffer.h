// [WriteFile Name=Buffer, Category=Geometry]
// [Legal]
// Copyright 2022 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifndef BUFFER_H
#define BUFFER_H

// Qt headers
#include <QMouseEvent>
#include <QObject>

namespace Esri::ArcGISRuntime
{
class GraphicsOverlay;
class Map;
class MapQuickView;
class SimpleMarkerSymbol;
class SimpleFillSymbol;
}

Q_MOC_INCLUDE("MapQuickView.h");

class Buffer : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(int bufferSize READ bufferSize WRITE setBufferSize NOTIFY bufferSizeChanged)

public:
  explicit Buffer(QObject* parent = nullptr);
  ~Buffer() override;

  static void init();

  Q_INVOKABLE void clear();

signals:
  void mapViewChanged();
  void bufferSizeChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  int bufferSize() const;
  void setBufferSize(int size);
  void onMouseClicked(QMouseEvent& mouse);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlayPlanar = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlayGeodesic = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlayPoints = nullptr;
  int m_bufferSize = 1000;
};

#endif // BUFFER_H
