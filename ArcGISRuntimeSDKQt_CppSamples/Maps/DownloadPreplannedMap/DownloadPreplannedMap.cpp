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
#include "DownloadPreplannedOfflineMapJob.h"
#include "DownloadPreplannedOfflineMapResult.h"

using namespace Esri::ArcGISRuntime;

DownloadPreplannedMap::DownloadPreplannedMap(QObject* parent /* = nullptr */):
  QObject(parent),
  m_portalItem(new PortalItem("acc027394bc84c2fb04d1ed317aac674", this))
{
  m_tempPath.setAutoRemove(false);
  m_busy = true;
  emit busyChanged();



  connect(m_portalItem, &PortalItem::doneLoading, this, [this] ()
  {
    m_offlineMapTask = new OfflineMapTask(m_map, this);

    connect(m_offlineMapTask, &OfflineMapTask::doneLoading, this, [this] ()
    {
      connect(m_offlineMapTask, &OfflineMapTask::preplannedMapAreasCompleted, this, [this] ()
      {
        m_preplannedList = m_offlineMapTask->preplannedMapAreaList();
        m_busy = false;
        emit preplannedListChanged();
        emit busyChanged();

        for (PreplannedMapArea* mapArea : *m_offlineMapTask->preplannedMapAreaList())
        {
          mapArea->load();
        }
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

void DownloadPreplannedMap::downloadMapArea(int index)
{
  if (!m_offlineMapTask)
    return;

  if (m_offlineMapTask->preplannedMapAreaList()->isEmpty())
    return;

  if (m_offlineMapTask->preplannedMapAreaList()->at(index)->loadStatus() != LoadStatus::Loaded)
    return;

  m_busy = true;
  emit busyChanged();

  qDebug() << "download Map area";

  QString path = QString(m_tempPath.path() + "_" +  m_offlineMapTask->preplannedMapAreaList()->at(index)->portalItem()->title() + ".mmpk");
  qDebug() << path;

  QDir tempDir(path);

//  if (tempDir.exists())
  if (m_mapExists)
  {
    if (m_mmpk)
    {
      delete m_mmpk;
      m_mmpk = nullptr;
    }

    m_mmpk = new MobileMapPackage(path, this);

    connect(m_mmpk, &MobileMapPackage::doneLoading, this, [this](Error e)
    {
      qDebug() << "already exists";
      if (!e.isEmpty())
        qDebug() << e.message() << " - " << e.additionalMessage();

      m_mapView->setMap(m_mmpk->maps().at(0));
      m_busy = false;
      emit busyChanged();
    });
    m_mmpk->load();
    return;
  }

  connect(m_offlineMapTask, &OfflineMapTask::createDefaultDownloadPreplannedOfflineMapParametersCompleted, this, [this, path] (QUuid ,const DownloadPreplannedOfflineMapParameters &parameters)
  {

    m_params = DownloadPreplannedOfflineMapParameters(parameters);
    m_params.setUpdateMode(PreplannedUpdateMode::NoUpdates);


    // something wrong with parameters...
//   m_preplannedMapJob = m_offlineMapTask->downloadPreplannedOfflineMap(params, m_tempPath.path() + "_" +  m_offlineMapTask->preplannedMapAreaList()->at(index)->portalItem()->title() + ".mmpk");
    m_preplannedMapJob = m_offlineMapTask->downloadPreplannedOfflineMap(m_params, path);


   connect(m_preplannedMapJob, &DownloadPreplannedOfflineMapJob::progressChanged, this, [this] ()
   {
     qDebug() << m_preplannedMapJob->progress();
   });

   connect(m_preplannedMapJob, &DownloadPreplannedOfflineMapJob::jobDone, this, [this] ()
   {
     if(m_preplannedMapJob->jobStatus() != JobStatus::Succeeded)
       return;

     m_mapView->setMap(m_preplannedMapJob->result()->offlineMap());
     m_offlineMapTask->disconnect();
     m_busy = false;
     emit busyChanged();
     m_mapExists = true;
     emit mapExistsChanged();
   });

   m_preplannedMapJob->start();

   qDebug () << "job started";

//   qDebug() << m_tempPath.path() + "_" + m_offlineMapTask->preplannedMapAreaList()->at(index)->portalItem()->title() + ".mmpk";
//   mapJob = m_offlineMapTask->downloadPreplannedOfflineMap(params, m_tempPath.path() + "_" + m_offlineMapTask->preplannedMapAreaList()->at(index)->portalItem()->title() + ".mmpk");

  });

  qDebug() << index;
  m_offlineMapTask->createDefaultDownloadPreplannedOfflineMapParameters(m_offlineMapTask->preplannedMapAreaList()->at(index));
}

void DownloadPreplannedMap::checkIfMapExists(const int index)
{
  if (!m_offlineMapTask)
    return;

  if (m_offlineMapTask->preplannedMapAreaList()->isEmpty())
    return;

  if (m_offlineMapTask->preplannedMapAreaList()->at(index)->loadStatus() != LoadStatus::Loaded)
    return;

  qDebug() << "checkMap";

  QString path = QString(m_tempPath.path() + "_" +  m_offlineMapTask->preplannedMapAreaList()->at(index)->portalItem()->title() + ".mmpk");
  QDir tempDir(path);

  if (tempDir.exists())
  {
    m_mapExists = true;
    emit mapExistsChanged();
  }
  else
  {
    m_mapExists = false;
    emit mapExistsChanged();
  }
}

void DownloadPreplannedMap::showOnlineMap()
{
  m_map = new Map(m_portalItem, this);
}
