// [WriteFile Name=WMTS_Layer, Category=Layers]
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

#ifndef WMTS_LAYER_H
#define WMTS_LAYER_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class MapQuickView;
    class WmtsService;
  }
}

#include <QQuickItem>

class WMTS_Layer : public QQuickItem
{
  Q_OBJECT

public:
  explicit WMTS_Layer(QQuickItem* parent = nullptr);
  ~WMTS_Layer() override = default;

  void componentComplete() override;
  static void init();

private:
  void createWmtsLayer();

private:
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::WmtsService* m_service = nullptr;
  const QUrl m_wmtsServiceUrl = QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/WorldTimeZones/MapServer/WMTS");
};

#endif // WMTS_LAYER_H
