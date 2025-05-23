// [WriteFile Name=IdentifyGraphics, Category=DisplayInformation]
// [Legal]
// Copyright 2016 Esri.
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

#ifndef IDENTIFY_GRAPHICS_H
#define IDENTIFY_GRAPHICS_H

// Qt headers
#include <QQuickItem>

namespace Esri::ArcGISRuntime
{
  class Map;
  class MapQuickView;
  class GraphicsOverlay;
}

class IdentifyGraphics : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(int identifiedGraphicsCount READ identifiedGraphicsCount NOTIFY identifiedGraphicsCountChanged)

public:
  explicit IdentifyGraphics(QQuickItem* parent = nullptr);
  ~IdentifyGraphics() override;

  void componentComplete() override;
  static void init();

signals:
  void identifiedGraphicsCountChanged();

private:
  void addPolygonGraphic();
  void connectSignals();
  int identifiedGraphicsCount();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  int m_identifiedGraphicsCount = 0;
};

#endif // IDENTIFY_GRAPHICS_H
