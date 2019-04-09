// [WriteFile Name=RasterFunctionFile, Category=Layers]
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

#ifndef RASTERFUNCTIONFILE_H
#define RASTERFUNCTIONFILE_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class Raster;
    class RasterFunction;
  }
}

#include <QQuickItem>

class RasterFunctionFile : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)

public:
  explicit RasterFunctionFile(QQuickItem* parent = nullptr);
  ~RasterFunctionFile() override = default;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void applyRasterFunction();

signals:
  void readyChanged();

private:
  Esri::ArcGISRuntime::RasterFunction* createRasterFunction();
  bool ready() const;

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Raster* m_raster = nullptr;
  QString m_dataPath;
  QString m_rasterPath;
};

#endif // RASTERFUNCTIONFILE_H
