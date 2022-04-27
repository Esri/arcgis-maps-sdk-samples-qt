// [WriteFile Name=AddGraphicsWithRenderer, Category=DisplayInformation]
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

#ifndef GO_RENDERERS_H
#define GO_RENDERERS_H

namespace Esri
{
namespace ArcGISRuntime
{
class Geometry;
class Graphic;
class Map;
class MapQuickView;
class Symbol;
}
}

#include <QQuickItem>

class AddGraphicsWithRenderer : public QQuickItem
{
  Q_OBJECT

public:
  explicit AddGraphicsWithRenderer(QQuickItem* parent = nullptr);
  ~AddGraphicsWithRenderer() override;

  void componentComplete() override;
  static void init();

private:
  void addGraphicsOverlays();
  void addPointGraphic();
  void addLineGraphic();
  void addPolygonGraphic();
  void addCurveGraphic();
  void addEllipseGraphic();
  void createGraphicsOverlayWithGraphicAndSymbol(Esri::ArcGISRuntime::Graphic* graphic, Esri::ArcGISRuntime::Symbol* symbol);
  Esri::ArcGISRuntime::Geometry createHeart();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
};

#endif // GO_RENDERERS_H

