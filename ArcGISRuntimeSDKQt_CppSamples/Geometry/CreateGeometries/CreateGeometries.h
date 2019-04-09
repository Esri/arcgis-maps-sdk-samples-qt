// [WriteFile Name=CreateGeometries, Category=Geometry]
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

#ifndef CREATEGEOMETRIES_H
#define CREATEGEOMETRIES_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class GraphicsOverlay;
}
}

#include "Geometry.h"
#include <QQuickItem>

class CreateGeometries : public QQuickItem
{
  Q_OBJECT

public:
  explicit CreateGeometries(QQuickItem* parent = nullptr);
  ~CreateGeometries() override = default;

  void componentComplete() override;
  static void init();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;

private:
  void addGraphics();
  Esri::ArcGISRuntime::Geometry createEnvelope() const;
  Esri::ArcGISRuntime::Geometry createPoint() const;
  Esri::ArcGISRuntime::Geometry createMultipoint() const;
  Esri::ArcGISRuntime::Geometry createPolyline() const;
  Esri::ArcGISRuntime::Geometry createPolygon() const;
};

#endif // CREATEGEOMETRIES_H
