// [WriteFile Name=GOSymbols, Category=DisplayInformation]
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

#ifndef GO_SYMBOLS_H
#define GO_SYMBOLS_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class Geometry;
    class GraphicsOverlay;
  }
}

#include <QQuickItem>

class GOSymbols : public QQuickItem
{
  Q_OBJECT

public:
  explicit GOSymbols(QQuickItem* parent = nullptr);
  ~GOSymbols() override;

  void componentComplete() override;
  static void init();

private:
  void addBuoyPoints(Esri::ArcGISRuntime::GraphicsOverlay* graphicsOverlay);
  void addBoatTrip(Esri::ArcGISRuntime::GraphicsOverlay* graphicsOverlay);
  void addNestingGround(Esri::ArcGISRuntime::GraphicsOverlay* graphicsOverlay);
  void addText(Esri::ArcGISRuntime::GraphicsOverlay* graphicsOverlay);
  Esri::ArcGISRuntime::Geometry createNestingGround();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
};

#endif // GOSYMBOLS_H

