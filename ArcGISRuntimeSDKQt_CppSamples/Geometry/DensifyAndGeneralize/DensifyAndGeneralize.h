// [WriteFile Name=DensifyAndGeneralize, Category=Geometry]
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

#ifndef DENSIFYANDGENERALIZE_H
#define DENSIFYANDGENERALIZE_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class GraphicsOverlay;
class Graphic;
class PointCollection;
}
}

#include <QQuickItem>
#include <PointCollection.h>

class DensifyAndGeneralize : public QQuickItem
{
  Q_OBJECT

public:
  explicit DensifyAndGeneralize(QQuickItem* parent = nullptr);
  ~DensifyAndGeneralize() override = default;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void updateGeometry(bool densify, double maxSegmentLength, bool generalize, double maxDeviation);
  Q_INVOKABLE void showResults(bool show);

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::Graphic* m_originalMultipointGraphic = nullptr;
  Esri::ArcGISRuntime::Graphic* m_originalLineGraphic = nullptr;
  Esri::ArcGISRuntime::Graphic* m_resultMultipointGraphic = nullptr;
  Esri::ArcGISRuntime::Graphic* m_resultLineGraphic = nullptr;

  Esri::ArcGISRuntime::PointCollection* createPointCollection();
};

#endif // DENSIFYANDGENERALIZE_H
