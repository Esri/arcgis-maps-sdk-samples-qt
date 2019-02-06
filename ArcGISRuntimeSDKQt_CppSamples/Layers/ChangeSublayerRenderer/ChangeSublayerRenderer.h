// [WriteFile Name=ChangeSublayerRenderer, Category=Layers]
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

#ifndef CHANGESUBLAYERRENDERER_H
#define CHANGESUBLAYERRENDERER_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class Renderer;
class ClassBreaksRenderer;
class ArcGISMapImageSublayer;
class ClassBreak;
}
}

#include <QQuickItem>

class ChangeSublayerRenderer : public QQuickItem
{
  Q_OBJECT

public:
  explicit ChangeSublayerRenderer(QQuickItem* parent = nullptr);
  ~ChangeSublayerRenderer() override = default;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void applyRenderer();
  Q_INVOKABLE void resetRenderer();

signals:
  void sublayerLoaded();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Renderer* m_originalRenderer = nullptr;
  Esri::ArcGISRuntime::ClassBreaksRenderer* m_classBreaksRenderer = nullptr;
  Esri::ArcGISRuntime::ArcGISMapImageSublayer* m_sublayer = nullptr;
  void createClassBreaksRenderer();
  Esri::ArcGISRuntime::ClassBreak* createClassBreak(const QColor& color, double min, double max);
};

#endif // CHANGESUBLAYERRENDERER_H
