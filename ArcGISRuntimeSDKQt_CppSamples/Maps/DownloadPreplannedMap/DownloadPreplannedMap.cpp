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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "DownloadPreplannedMap.h"

#include "DownloadPreplannedOfflineMapJob.h"
#include "DownloadPreplannedOfflineMapResult.h"
#include "Graphic.h"
#include "Map.h"
#include "MapQuickView.h"
#include "OfflineMapTask.h"
#include "PreplannedMapArea.h"
#include "PreplannedMapAreaListModel.h"
#include "SimpleLineSymbol.h"
#include "SimpleRenderer.h"

using namespace Esri::ArcGISRuntime;

DownloadPreplannedMap::DownloadPreplannedMap(QObject* parent /* = nullptr */):
  QObject(parent),
  m_graphicsOverlay(new GraphicsOverlay(this)),
  m_portalItem(new PortalItem("acc027394bc84c2fb04d1ed317aac674", this)),
  m_lineSymbol(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(Qt::red), 5, this)),
  m_busy(true)
{
  emit busyChanged();

  m_graphicsOverlay->setRenderer(new SimpleRenderer(m_lineSymbol, this));

  connect(m_portalItem, &PortalItem::doneLoading, this, [this] ()
  {
    m_offlineMapTask = new OfflineMapTask(m_map, this);
    m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

    connect(m_offlineMapTask, &OfflineMapTask::doneLoading, this, [this] ()
    {
      connect(m_offlineMapTask, &OfflineMapTask::preplannedMapAreasCompleted, this, &DownloadPreplannedMap::loadPreplannedMapAreas);

      // fetch preplanned map areas from the portal item
      m_offlineMapTask->preplannedMapAreas();
    });

    connect(m_offlineMapTask, &OfflineMapTask::createDefaultDownloadPreplannedOfflineMapParametersCompleted, this, [this] (QUuid ,const DownloadPreplannedOfflineMapParameters& parameters)
    {

      m_params = parameters;

      /* Set the update mode to not receive updates.
       * Other options:
       * SyncWithFeatureServices - changes will be synced directly with the
       * underlying feature services.
       * DownloadScheduledUpdates - schedulded, read-only updates will be
       * downloaded and applied to the local geodatabase. */
      m_params.setUpdateMode(PreplannedUpdateMode::NoUpdates);

      m_preplannedMapJob = m_offlineMapTask->downloadPreplannedOfflineMap(m_params, m_path);
      connect(m_preplannedMapJob, &DownloadPreplannedOfflineMapJob::progressChanged, this, [this] ()
      {
        m_percentDownloaded = .01 * m_preplannedMapJob->progress();
        emit percentDownloadedChanged();
      });
      connect(m_preplannedMapJob, &DownloadPreplannedOfflineMapJob::jobDone, this, &DownloadPreplannedMap::onDownloadPreplannedMapJobCompleted);

      // Start job to take preplanned map area offline.
      m_preplannedMapJob->start();
      m_busy = false;
      emit busyChanged();
    });

    // Load offline map task.
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
  qmlRegisterUncreatableType<PreplannedMapAreaListModel>("Esri.Samples", 1, 0, "AbstractListModel", "DownloadPreplannedMapSample");
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

void DownloadPreplannedMap::checkIfMapAreaIsLoaded(int index)
{
  if (!m_offlineMapTask)
    return;

  if (m_offlineMapTask->preplannedMapAreaList()->isEmpty())
    return;

  PreplannedMapArea* mapArea = m_offlineMapTask->preplannedMapAreaList()->at(index);

  if (!mapArea || mapArea->loadStatus() != LoadStatus::Loaded)
    return;

  m_busy = true;
  m_viewingOnlineMaps = false;
  emit busyChanged();
  emit viewingOnlineMapsChanged();

  if(m_graphicsOverlay->isVisible())
    m_graphicsOverlay->setVisible(false);

  loadSelectedMap(index);
}

void DownloadPreplannedMap::loadSelectedMap(int index)
{
  checkIfMapExists(index);

  if (m_preplannedMapExists)
    loadExistingPreplannedMap();
  else
    m_offlineMapTask->createDefaultDownloadPreplannedOfflineMapParameters(m_offlineMapTask->preplannedMapAreaList()->at(index));
}

void DownloadPreplannedMap::checkIfMapExists(int index)
{
  if (!m_offlineMapTask)
    return;

  if (m_offlineMapTask->preplannedMapAreaList()->isEmpty())
    return;

  PreplannedMapArea* mapArea = m_offlineMapTask->preplannedMapAreaList()->at(index);

  if (!mapArea || mapArea->loadStatus() != LoadStatus::Loaded)
    return;

  m_path = QString(m_tempPath.path() + "/" + mapArea->portalItem()->title());
  QDir tempDir(m_path);

  m_preplannedMapExists = tempDir.exists();
  emit preplannedMapExistsChanged();

  if (m_viewingOnlineMaps)
    m_mapView->setViewpointGeometry(mapArea->areaOfInterest());
}

void DownloadPreplannedMap::showOnlineMap(int index)
{
  m_viewingOnlineMaps = true;
  emit viewingOnlineMapsChanged();
  m_mapView->setMap(m_map);
  const Geometry areaOfInterest = m_offlineMapTask->preplannedMapAreaList()->at(index)->areaOfInterest();
  m_mapView->setViewpointGeometry(areaOfInterest);
  m_graphicsOverlay->setVisible(true);
}

void DownloadPreplannedMap::onDownloadPreplannedMapJobCompleted()
{
  if (m_preplannedMapJob->jobStatus() != JobStatus::Succeeded)
    return;

  m_mapView->setMap(m_preplannedMapJob->result()->offlineMap());
  m_preplannedMapExists = true;
  emit preplannedMapExistsChanged();
  m_viewingOnlineMaps = false;
  emit viewingOnlineMapsChanged();
}

void DownloadPreplannedMap::loadPreplannedMapAreas()
{
  m_preplannedList = m_offlineMapTask->preplannedMapAreaList();
  m_busy = false;
  emit preplannedListChanged();
  emit busyChanged();

  for (PreplannedMapArea* mapArea : *m_preplannedList)
  {
    connect(mapArea, &PreplannedMapArea::doneLoading, this, [this, mapArea] (Error e)
    {
      if (!e.isEmpty())
      {
        qDebug() << e.message() << " - " << e.additionalMessage();
        return;
      }

      Graphic* areaOfInterest = new Graphic(mapArea->areaOfInterest(), this);
      m_graphicsOverlay->graphics()->append(areaOfInterest);
    });

    mapArea->load();
  }
}

void DownloadPreplannedMap::loadExistingPreplannedMap()
{
  if (m_mmpk)
  {
    delete m_mmpk;
    m_mmpk = nullptr;
  }

  m_mmpk = new MobileMapPackage(m_path, this);

  connect(m_mmpk, &MobileMapPackage::doneLoading, this, [this] (Error e)
  {
    if (!e.isEmpty())
      qDebug() << e.message() << " - " << e.additionalMessage();

    m_mapView->setMap(m_mmpk->maps().at(0));
    m_busy = false;
    emit busyChanged();
    m_graphicsOverlay->setVisible(false);
    m_viewingOnlineMaps = false;
    emit viewingOnlineMapsChanged();
  });

  m_mmpk->load();
}
