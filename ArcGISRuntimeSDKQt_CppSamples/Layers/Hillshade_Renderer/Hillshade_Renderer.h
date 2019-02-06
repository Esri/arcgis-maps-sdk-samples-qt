// [WriteFile Name=Hillshade_Renderer, Category=Layers]
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

#ifndef HILLSHADE_RENDERER_H
#define HILLSHADE_RENDERER_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class MapQuickView;
    class RasterLayer;
  }
}

#include <QQuickItem>

class Hillshade_Renderer : public QQuickItem
{
  Q_OBJECT

public:
  explicit Hillshade_Renderer(QQuickItem* parent = nullptr);
  ~Hillshade_Renderer() override;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void applyHillshadeRenderer(double altitude, double azimuth, int slope);

private:
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::RasterLayer* m_rasterLayer = nullptr;
};

#endif // HILLSHADE_RENDERER_H
