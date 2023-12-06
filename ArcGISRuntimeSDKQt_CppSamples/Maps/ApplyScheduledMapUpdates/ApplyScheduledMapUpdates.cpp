// [WriteFile Name=ApplyScheduledMapUpdates, Category=Maps]
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

#include "ApplyScheduledMapUpdates.h"

#include "Map.h"
#include "MapQuickView.h"
#include "MobileMapPackage.h"
#include "OfflineMapSyncTask.h"
#include "OfflineMapUpdatesInfo.h"
#include "OfflineMapSyncParameters.h"
#include "OfflineMapSyncJob.h"
#include "Error.h"
#include "OfflineMapUpdatesInfo.h"
#include "OfflineMapTypes.h"
#include "OfflineMapSyncResult.h"

#include <QFile>
#include <QFileInfo>
#include <QFuture>
#include <QtCore/qglobal.h>
#include <QUuid>
#include <QStandardPaths>

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
QString defaultDataPath()
{
  QString dataPath;

#ifdef Q_OS_IOS
  dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
  dataPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif

  return dataPath + "/ArcGIS/Runtime/Data/mmpk";
}

// sample MMPK location
const QString sampleMmpk { "canyonlands" };

// helper to copy directory from source to destination
bool copyDir(const QString &srcPath, const QString &dstPath)
{
  const QDir parentDstDir(QFileInfo(dstPath).path());
  if (!parentDstDir.mkdir(QFileInfo(dstPath).fileName()))
    return false;

  const QDir srcDir(srcPath);
  const auto infos = srcDir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
  for (const QFileInfo &info : infos)
  {
    const QString srcItemPath = srcPath + "/" + info.fileName();
    const QString dstItemPath = dstPath + "/" + info.fileName();
    if (info.isDir())
    {
      if (!copyDir(srcItemPath, dstItemPath))
      {
        return false;
      }
    }
    else if (info.isFile())
    {
      if (!QFile::copy(srcItemPath, dstItemPath))
      {
        return false;
      }
    }
    else
    {
      qDebug() << "Unhandled item" << info.filePath() << "in copyDir";
    }
  }
  return true;
}
}

ApplyScheduledMapUpdates::ApplyScheduledMapUpdates(QObject* parent /* = nullptr */):
  QObject(parent)
{
  // For the purposes of demonstrating the sample,
  // create a temporary copy of the local offline map files,
  // so that updating does not overwrite them permanently

  copyDir(defaultDataPath() + "/" + sampleMmpk, m_TempDir.path() + "/" + sampleMmpk);

  // create MMPK
  m_mobileMapPackage = new MobileMapPackage(m_TempDir.path() + "/" + sampleMmpk, this);

  // load mmpk
  connect(m_mobileMapPackage, &MobileMapPackage::doneLoading, this, &ApplyScheduledMapUpdates::onMmpkDoneLoading);
  m_mobileMapPackage->load();
}

ApplyScheduledMapUpdates::~ApplyScheduledMapUpdates()
{
  if (m_mobileMapPackage)
  {
    m_mobileMapPackage->close();
  }
}

void ApplyScheduledMapUpdates::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ApplyScheduledMapUpdates>("Esri.Samples", 1, 0, "ApplyScheduledMapUpdatesSample");
}

void ApplyScheduledMapUpdates::updateMap()
{
  if (!m_offlineSyncTask)
    return;

  m_offlineSyncTask->createDefaultOfflineMapSyncParametersAsync().then(this,
  [this](OfflineMapSyncParameters parameters)
  {
    // set the parameters to download all updates for the mobile map packages
    parameters.setPreplannedScheduledUpdatesOption(PreplannedScheduledUpdatesOption::DownloadAllUpdates);

    // delete and disconnect previous jobs
    if (m_syncJob)
    {
      delete m_syncJob;
      disconnect(m_syncJobConnection);
    }

    // create sync job
    m_syncJob = m_offlineSyncTask->syncOfflineMap(parameters);

    // connect to know when job is complete
    m_syncJobConnection = connect(m_syncJob, &OfflineMapSyncJob::jobDone, this, [this]()
    {
      if (m_syncJob->result()->isMobileMapPackageReopenRequired())
      {
        // close mmpk out
        m_mobileMapPackage->close();
        delete m_mobileMapPackage;

        // load mmpk again with new instance
        m_mobileMapPackage = new MobileMapPackage(m_TempDir.path() + "/" + sampleMmpk, this);
        connect(m_mobileMapPackage, &MobileMapPackage::doneLoading, this, &ApplyScheduledMapUpdates::onMmpkDoneLoading);
        m_mobileMapPackage->load();
      }

      // re-check if updates are available
      m_offlineSyncTask->checkForUpdatesAsync().then(this,
      [this](const OfflineMapUpdatesInfo* info)
      {
        if (info->downloadAvailability() == OfflineUpdateAvailability::Available)
        {
          emit updateUi(true, "Updates Available", QString("Updates size: %1 bytes").arg(info->scheduledUpdatesDownloadSize()));
        }
        else
        {
          emit updateUi(false, "No updates available", "The preplanned map area is up to date");
        }

        delete info;
      });
    });

    // start the job
    m_syncJob->start();
  });
}

MapQuickView* ApplyScheduledMapUpdates::mapView() const
{
  return m_mapView;
}

void ApplyScheduledMapUpdates::setMapToMapView()
{
  if (!m_map || !m_mapView)
    return;

  m_mapView->setMap(m_map);
}

void ApplyScheduledMapUpdates::onMmpkDoneLoading(const Error& e)
{
  // check if successful
  if (!e.isEmpty())
    return;

  // make sure there are valid maps
  if (m_mobileMapPackage->maps().isEmpty())
    return;

  // set the map on the map view
  m_map = m_mobileMapPackage->maps().at(0);
  setMapToMapView();

  m_offlineSyncTask = new OfflineMapSyncTask(m_map, this);

  // check for updates
  m_offlineSyncTask->checkForUpdatesAsync().then(this,
  [this](OfflineMapUpdatesInfo* info)
  {
    if (info->downloadAvailability() == OfflineUpdateAvailability::Available)
    {
      emit updateUi(true, "Updates Available", QString("Updates size: %1 bytes").arg(info->scheduledUpdatesDownloadSize()));
    }
    else
    {
      emit updateUi(false, "No updates available", "The preplanned map area is up to date");
    }

    delete info;
  });
}

// Set the view (created in QML)
void ApplyScheduledMapUpdates::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;

  setMapToMapView();

  emit mapViewChanged();
}
