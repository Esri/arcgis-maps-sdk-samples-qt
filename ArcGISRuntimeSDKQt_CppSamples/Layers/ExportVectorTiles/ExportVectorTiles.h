// [WriteFile Name=ExportVectorTiles, Category=Layers]
// [Legal]
// Copyright 2022 Esri.

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

#ifndef EXPORTVECTORTILES_H
#define EXPORTVECTORTILES_H

namespace Esri
{
namespace ArcGISRuntime
{
class ExportVectorTilesJob;
class Graphic;
class GraphicsOverlay;
class Map;
class MapQuickView;
}
}

#include <QObject>
#include <QTemporaryDir>

class ExportVectorTiles : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(int exportProgress READ exportProgress NOTIFY exportProgressChanged)

public:
  explicit ExportVectorTiles(QObject* parent = nullptr);
  ~ExportVectorTiles();

  static void init();

  Q_INVOKABLE void startExport(double xSW, double ySW, double xNE, double yNE);
  Q_INVOKABLE void cancelExport();

signals:
  void mapViewChanged();
  void exportProgressChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  int exportProgress() const;

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  QScopedPointer<QObject> m_graphicsParent;
  Esri::ArcGISRuntime::Graphic* m_exportAreaGraphic = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::ExportVectorTilesJob* m_exportJob = nullptr;

  int m_exportProgress = 0;

  QTemporaryDir m_tempDir;
};

#endif // EXPORTVECTORTILES_H
