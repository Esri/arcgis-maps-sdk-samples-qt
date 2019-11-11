// [WriteFile Name=DownloadPreplannedMap, Category=Maps]
// [Legal]
// Copyright 2019 Esri.

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

#ifndef DOWNLOADPREPLANNEDMAP_H
#define DOWNLOADPREPLANNEDMAP_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class GraphicsOverlay;
class PortalItem;
class OfflineMapTask;
class PreplannedMapArea;
}
}

#include <QObject>

class DownloadPreplannedMap : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  explicit DownloadPreplannedMap(QObject* parent = nullptr);
  ~DownloadPreplannedMap();

  static void init();

signals:
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::OfflineMapTask* m_offlineMapTask = nullptr;
  Esri::ArcGISRuntime::OfflineMapTask* m_portalItem = nullptr;
};

#endif // DOWNLOADPREPLANNEDMAP_H
