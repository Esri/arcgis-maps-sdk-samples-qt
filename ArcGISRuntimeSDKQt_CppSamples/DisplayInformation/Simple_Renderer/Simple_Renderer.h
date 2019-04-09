// [WriteFile Name=Simple_Renderer, Category=DisplayInformation]
// [Legal]
// Copyright 2016 Esri.

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

#ifndef SIMPLE_RENDERER_H
#define SIMPLE_RENDERER_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class GraphicsOverlay;
    class Point;
  }
}

#include <QQuickItem>

class Simple_Renderer : public QQuickItem
{
  Q_OBJECT

public:
  explicit Simple_Renderer(QQuickItem* parent = nullptr);
  ~Simple_Renderer() override;

  void componentComplete() override;
  static void init();

private:
  void addPoint(Esri::ArcGISRuntime::Point &point);

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
};

#endif // SIMPLE_RENDERER_H
