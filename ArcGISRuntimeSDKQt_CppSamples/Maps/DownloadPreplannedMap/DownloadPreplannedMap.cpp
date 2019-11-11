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

#include "DownloadPreplannedMap.h"

#include "Map.h"
#include "MapQuickView.h"
#include "OfflineMapTask.h"
#include "PreplannedMapArea.h"
#include "PreplannedMapAreaListModel.h"

using namespace Esri::ArcGISRuntime;

DownloadPreplannedMap::DownloadPreplannedMap(QObject* parent /* = nullptr */):
  QObject(parent),
  m_portalItem(new PortalItem("acc027394bc84c2fb04d1ed317aac674", this))
{
  connect(m_portalItem, &PortalItem::doneLoading, this, [this] ()
  {
    m_offlineMapTask = new OfflineMapTask(m_map, this);

    connect(m_offlineMapTask, &OfflineMapTask::doneLoading, this, [this] ()
    {
      connect(m_offlineMapTask, &OfflineMapTask::preplannedMapAreasCompleted, this, [this] ()
      {
        m_preplannedList = m_offlineMapTask->preplannedMapAreaList();
        emit preplannedListChanged();
      });
     m_offlineMapTask->preplannedMapAreas();
    });
    m_offlineMapTask->load();
  });
  m_map = new Map(m_portalItem, this);

}

DownloadPreplannedMap::~DownloadPreplannedMap() = default;

void DownloadPreplannedMap::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DownloadPreplannedMap>("Esri.Samples", 1, 0, "DownloadPreplannedMapSample");
}

MapQuickView* DownloadPreplannedMap::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void DownloadPreplannedMap::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}
