// [WriteFile Name=RasterFunctionService, Category=Layers]
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

#ifndef RASTERFUNCTIONSERVICE_H
#define RASTERFUNCTIONSERVICE_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Basemap;
    class Map;
    class MapQuickView;
    class ImageServiceRaster;
    class RasterLayer;
  }
}

#include <QQuickItem>

class RasterFunctionService : public QQuickItem
{
  Q_OBJECT

public:
  explicit RasterFunctionService(QQuickItem* parent = nullptr);
  ~RasterFunctionService() override = default;

  static void init();
  Q_INVOKABLE void applyRasterFunction();

  void componentComplete() override;

private:

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::ImageServiceRaster* m_imageServiceRaster = nullptr;
  Esri::ArcGISRuntime::RasterLayer* m_rasterLayer = nullptr;
  QString m_dataPath;
};

#endif // RASTERFUNCTIONSERVICE_H
