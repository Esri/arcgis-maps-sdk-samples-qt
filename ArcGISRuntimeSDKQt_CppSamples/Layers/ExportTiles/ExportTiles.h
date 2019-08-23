// [WriteFile Name=ExportTiles, Category=Layers]
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

#ifndef EXPORT_TILES
#define EXPORT_TILES

// C++ API headers
#include "ExportTileCacheParameters.h"

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class ExportTileCacheTask;
    class TileCache;
  }
}

#include <QQuickItem>
#include <QTemporaryDir>

class ExportTiles : public QQuickItem
{
  Q_OBJECT

public:
  explicit ExportTiles(QQuickItem* parent = nullptr);
  ~ExportTiles() override;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void exportTileCacheFromCorners(double xCorner1, double yCorner1, double xCorner2, double yCorner2);

signals:
  void updateStatus(QString status);
  void hideWindow(int time, bool success);

private:
  void displayOutputTileCache(Esri::ArcGISRuntime::TileCache* tileCache);

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::ExportTileCacheTask* m_exportTileCacheTask = nullptr;
  Esri::ArcGISRuntime::ExportTileCacheParameters m_parameters;
  QUrl m_serviceUrl = QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/World_Street_Map/MapServer");
  QTemporaryDir m_tempPath;
};

#endif // EXPORT_TILES
