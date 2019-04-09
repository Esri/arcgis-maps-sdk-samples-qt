// [WriteFile Name=Buffer, Category=Geometry]
// [Legal]
// Copyright 2018 Esri.

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

#ifndef BUFFER_H
#define BUFFER_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class GraphicsOverlay;
class SimpleMarkerSymbol;
class SimpleFillSymbol;
}
}

#include <QMouseEvent>
#include <QQuickItem>

class Buffer : public QQuickItem
{
  Q_OBJECT

public:
  explicit Buffer(QQuickItem* parent = nullptr);
  ~Buffer() override = default;

  Q_PROPERTY(int bufferSize READ bufferSize WRITE setBufferSize NOTIFY bufferSizeChanged)

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void clear();

signals:
  void bufferSizeChanged();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlayPlanar = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlayGeodesic = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlayPoints = nullptr;
  int m_bufferSize = 1000;

private:
  int bufferSize() const;
  void setBufferSize(int size);

private slots:
  void onMouseClicked(QMouseEvent& mouse);
};

#endif // BUFFER_H
