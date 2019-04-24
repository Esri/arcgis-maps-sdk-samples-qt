// [WriteFile Name=RasterRenderingRule, Category=Layers]
// [Legal]
// Copyright 2017 Esri.

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

#ifndef RasterRenderingRule_H
#define RasterRenderingRule_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Basemap;
    class Map;
    class MapQuickView;
    class RasterLayer;
    class ImageServiceRaster;
  }
}

#include <QQuickItem>

class RasterRenderingRule : public QQuickItem
{
  Q_OBJECT
  Q_PROPERTY(QStringList renderingRuleNames MEMBER m_renderingRuleNames NOTIFY renderingRuleNamesChanged)

public:
  explicit RasterRenderingRule(QQuickItem* parent = nullptr);
  ~RasterRenderingRule() override = default;

  static void init();
  void componentComplete() override;
  Q_INVOKABLE void applyRenderingRule(int index);

signals:
  void renderingRuleNamesChanged();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::RasterLayer* m_rasterLayer = nullptr;
  Esri::ArcGISRuntime::ImageServiceRaster* m_imageServiceRaster = nullptr;
  QStringList m_renderingRuleNames;
  QUrl m_url;
};

#endif // RasterRenderingRule_H
