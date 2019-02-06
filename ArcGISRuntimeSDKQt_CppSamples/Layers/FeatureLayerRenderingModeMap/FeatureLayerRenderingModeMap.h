// [WriteFile Name=FeatureLayerRenderingModeMap, Category=Layers]
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

#ifndef FEATURELAYERRENDERINGMODEMAP_H
#define FEATURELAYERRENDERINGMODEMAP_H

namespace Esri
{
namespace ArcGISRuntime
{
class MapQuickView;
class Map;
}
}

class QTimer;

#include "Viewpoint.h"
#include <QQuickItem>

class FeatureLayerRenderingModeMap : public QQuickItem
{
  Q_OBJECT

public:
  explicit FeatureLayerRenderingModeMap(QQuickItem* parent = nullptr);
  ~FeatureLayerRenderingModeMap() override = default;

  void componentComplete() override;
  static void init();

  Q_INVOKABLE void startAnimation();
  Q_INVOKABLE void stopAnimation();

private slots:
  void animate();

private:
  void addFeatureLayers(Esri::ArcGISRuntime::Map* map);

  Esri::ArcGISRuntime::MapQuickView* m_topMapView = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_bottomMapView = nullptr;
  Esri::ArcGISRuntime::Viewpoint m_zoomOutViewpoint;
  Esri::ArcGISRuntime::Viewpoint m_zoomInViewpoint;
  QTimer* m_timer = nullptr;
  const QString m_featureServiceUrl = "https://sampleserver6.arcgisonline.com/arcgis/rest/services/Energy/Geology/FeatureServer";
  bool m_isZoomedOut = true;
};

#endif // FEATURELAYERRENDERINGMODEMAP_H
