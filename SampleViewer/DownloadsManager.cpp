// [Legal]
// Copyright 2025 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#include "pch.hpp" // IWYU pragma: keep

// ArcGIS Maps SDK headers
#include "ErrorException.h"
#include "MapTypes.h"
#include "NetworkRequestProgress.h"
#include "Portal.h"
#include "PortalItem.h"
#include "PortalTypes.h"

// Qt headers
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>
#include <QStandardPaths>
#include <QTimer>
#include <QUrl>

// STL headers
#include <algorithm>

// Other headers
#include "DataItem.h"
#include "DataItemListModel.h"
#include "DownloadsManager.h"
#include "OfflineDataProjectsModel.h"
#include "Sample.h"
#include "SampleListModel.h"
#include "ZipHelper.h"

using namespace Esri::ArcGISRuntime;

DownloadsManager::DownloadsManager(QObject* parent) :
  QObject(parent),
  m_progressUpdateTimer(new QTimer(this)),
  m_offlineDataProjects(new OfflineDataProjectsModel(this))
{
  m_progressUpdateTimer->setInterval(PROGRESS_UPDATE_INTERVAL_MS);
  connect(m_progressUpdateTimer, &QTimer::timeout, this, [this]()
  {
    // Calculate overall progress across all downloading items
    if (m_dataItemProgress.isEmpty())
    {
      setDownloadProgress(0.0);
      return;
    }

    double totalProgress = 0.0;
    for (const double& progress : m_dataItemProgress)
    {
      totalProgress += progress;
    }
    setDownloadProgress(totalProgress / m_dataItemProgress.size());

    updateOfflineDataProjects();
  });
}

DownloadsManager::~DownloadsManager() = default;

void DownloadsManager::initialize(Portal* portal, const QString& homePath)
{
  if (!portal)
  {
    qWarning() << "DownloadsManager::initialize: Portal is null!";
    return;
  }

  if (homePath.isEmpty())
  {
    qWarning() << "DownloadsManager::initialize: Home path is empty!";
    return;
  }

  m_portal = portal;
  m_homePath = homePath;
}

int DownloadsManager::countDownloadedItems(Sample* sample) const
{
  if (!sample || sample->dataItems()->isEmpty())
  {
    return 0;
  }

  int downloadedItems = 0;
  for (const auto& item : *sample->dataItems())
  {
    if (item->exists())
    {
      downloadedItems++;
    }
  }
  return downloadedItems;
}

bool DownloadsManager::isSampleDownloading(Sample* sample) const
{
  if (!sample || sample->dataItems()->isEmpty())
  {
    return false;
  }

  for (const auto& dataItem : *sample->dataItems())
  {
    QString key = buildDataItemKey(dataItem);
    ;
    if (m_dataItemProgress.contains(key))
    {
      return true;
    }
  }
  return false;
}

void DownloadsManager::deleteDataItemFile(DataItem* dataItem)
{
  if (!dataItem)
  {
    return;
  }

  QJsonObject tracking = loadDownloadTracking();

  if (!tracking.contains(dataItem->itemId()))
  {
    // qWarning() << "No tracking data found for item:" << dataItem->itemId();
    return;
  }

  const QJsonObject itemData = tracking[dataItem->itemId()].toObject();
  const QJsonArray filesArray = itemData["files"].toArray();

  auto reversedFilesArray = filesArray.toVariantList();
  std::reverse(reversedFilesArray.begin(), reversedFilesArray.end());

  QStringList deletedFiles;
  QSet<QString> deletedDirs;

  for (const QVariant& value : reversedFilesArray)
  {
    const QString filePath = value.toString();
    const QFileInfo fileInfo(filePath);
    if (!fileInfo.exists())
    {
      continue;
    }
    if (fileInfo.isDir())
    {
      QDir dir(filePath);
      if (dir.removeRecursively())
      {
        deletedDirs.insert(filePath);
      }
      else
      {
        qWarning() << "Failed to delete directory:" << filePath;
      }
    }
    else if (fileInfo.isFile())
    {
      if (QFile::remove(filePath))
      {
        deletedFiles.append(filePath);
      }
      else
      {
        qWarning() << "Failed to delete file:" << filePath;
      }
    }
  }

  // Clean up empty parent directories
  const QString dataRoot = m_homePath + "/ArcGIS/Runtime/Data";
  QSet<QString> parentPaths;

  for (const QString& filePath : deletedFiles)
  {
    parentPaths.insert(QFileInfo(filePath).dir().absolutePath());
  }

  for (const QString& dirPath : deletedDirs)
  {
    parentPaths.insert(QFileInfo(dirPath).dir().absolutePath());
  }

  QStringList sortedPaths = parentPaths.values();
  std::sort(sortedPaths.begin(), sortedPaths.end(), [](const QString& a, const QString& b)
  {
    return a.length() > b.length();
  });

  for (const QString& parentPath : sortedPaths)
  {
    cleanupEmptyParentDirectories(parentPath, dataRoot);
  }

  // Remove tracking data
  tracking.remove(dataItem->itemId());
  saveDownloadTracking(tracking);
}

void DownloadsManager::cancelDataItem(const QString& dataItemKey)
{
  PortalItem* portalItem = nullptr;
  if (m_activeDownloads.contains(dataItemKey))
  {
    portalItem = m_activeDownloads[dataItemKey];
  }

  m_dataItemProgress.remove(dataItemKey);
  m_activeDownloads.remove(dataItemKey);

  if (portalItem)
  {
    portalItem->cancelLoad();
  }
}

void DownloadsManager::cleanupEmptyParentDirectories(const QString& startPath, const QString& boundary)
{
  QDir currentDir{startPath};

  while (currentDir.absolutePath() != boundary && currentDir.absolutePath().startsWith(boundary))
  {
    if (currentDir.isEmpty())
    {
      const QString pathToRemove = currentDir.absolutePath();

      if (!currentDir.cdUp())
      {
        break;
      }

      if (!QDir().rmdir(pathToRemove))
      {
        break;
      }
    }
    else
    {
      break;
    }
  }
}

QString DownloadsManager::buildDataItemKey(const DataItem* dataItem) const
{
  return dataItem->itemId() + "|" + m_homePath + dataItem->path().mid(1);
}

QJsonObject DownloadsManager::loadDownloadTracking()
{
  const QString trackingFile = m_homePath + "/ArcGIS/Runtime/Data/SampleDownloads.json";
  QFile file(trackingFile);

  if (!file.open(QIODevice::ReadOnly))
  {
    return QJsonObject();
  }

  const QByteArray data = file.readAll();
  file.close();

  QJsonParseError parseError;
  QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

  if (parseError.error != QJsonParseError::NoError)
  {
    qWarning() << "Failed to parse tracking file, starting fresh";
    return QJsonObject();
  }

  return doc.object();
}

void DownloadsManager::saveDownloadTracking(const QJsonObject& data)
{
  const QString trackingFile = m_homePath + "/ArcGIS/Runtime/Data/SampleDownloads.json";

  const QFileInfo fileInfo(trackingFile);
  QDir dir = fileInfo.dir();
  if (!dir.exists())
  {
    dir.mkpath(".");
  }

  QFile file(trackingFile);
  if (!file.open(QIODevice::WriteOnly))
  {
    qWarning() << "Failed to save tracking file";
    return;
  }

  QJsonDocument doc(data);
  file.write(doc.toJson(QJsonDocument::Indented));
  file.close();
}

void DownloadsManager::trackFilesForItem(const QString& itemId, const QStringList& files)
{
  QJsonObject tracking = loadDownloadTracking();

  QJsonObject itemData;
  if (tracking.contains(itemId))
  {
    itemData = tracking[itemId].toObject();
  }

  QJsonArray filesArray;
  if (itemData.contains("files"))
  {
    filesArray = itemData["files"].toArray();
  }

  QSet<QString> existingFiles;
  for (const QJsonValue& val : filesArray)
  {
    existingFiles.insert(val.toString());
  }

  for (const QString& file : files)
  {
    if (!existingFiles.contains(file))
    {
      filesArray.append(file);
    }
  }

  itemData["files"] = filesArray;
  itemData["lastModified"] = QDateTime::currentDateTime().toString(Qt::ISODate);

  tracking[itemId] = itemData;
  saveDownloadTracking(tracking);
}

DownloadsManager::SampleDownloadState DownloadsManager::getSampleDownloadState(Sample* sample) const
{
  SampleDownloadState state;

  if (!sample || sample->dataItems()->isEmpty())
  {
    return state;
  }

  state.totalItems = sample->dataItems()->size();
  state.downloadedItems = countDownloadedItems(sample);
  state.downloaded = (state.downloadedItems == state.totalItems);
  state.isDownloading = isSampleDownloading(sample);
  state.progress = calculateSampleDownloadProgress(sample);

  return state;
}

bool DownloadsManager::downloadInProgress() const
{
  return m_downloadInProgress;
}

QString DownloadsManager::downloadText() const
{
  return m_downloadText;
}

double DownloadsManager::downloadProgress() const
{
  return m_downloadProgress;
}

bool DownloadsManager::cancelDownload() const
{
  return m_cancelDownload;
}

bool DownloadsManager::downloadFailed() const
{
  return m_downloadFailed;
}

bool DownloadsManager::isBulkDownload() const
{
  return m_isBulkDownload;
}

OfflineDataProjectsModel* DownloadsManager::offlineDataProjects() const
{
  return m_offlineDataProjects;
}

void DownloadsManager::setDownloadInProgress(bool inProgress)
{
  if (m_downloadInProgress == inProgress)
  {
    return;
  }
  m_downloadInProgress = inProgress;
  emit downloadInProgressChanged();
}

void DownloadsManager::setIsBulkDownload(bool isBulk)
{
  if (m_isBulkDownload == isBulk)
  {
    return;
  }
  m_isBulkDownload = isBulk;
  emit isBulkDownloadChanged();
}

void DownloadsManager::setDownloadText(const QString& downloadText)
{
  if (m_downloadText == downloadText)
  {
    return;
  }
  m_downloadText = downloadText;
  emit downloadTextChanged();
}

void DownloadsManager::setDownloadProgress(double progress)
{
  if (m_downloadProgress == progress)
  {
    return;
  }
  m_downloadProgress = progress;
  emit downloadProgressChanged();
}

void DownloadsManager::setDownloadFailed(bool didFail)
{
  if (m_downloadFailed == didFail)
  {
    return;
  }
  m_downloadFailed = didFail;
  emit downloadFailedChanged();
}

void DownloadsManager::setCancelDownload(bool cancel)
{
  if (m_cancelDownload == cancel)
  {
    return;
  }
  m_cancelDownload = cancel;
  emit cancelDownloadChanged();
}

void DownloadsManager::downloadAllDataItems()
{
  if (!m_samples)
  {
    qWarning() << "DownloadsManager::downloadAllDataItems: samples is null!";
    return;
  }

  setDownloadFailed(false);
  setIsBulkDownload(true);

  const int totalSamples = m_samples->size();

  QSet<QString> uniqueDataItems;

  for (int i = 0; i < totalSamples; ++i)
  {
    Sample* sample = m_samples->at(i);
    const int dataItemCount = sample->dataItems()->size();

    for (int j = 0; j < dataItemCount; ++j)
    {
      DataItem* dataItem = sample->dataItems()->at(j);

      if (dataItem->exists())
      {
        continue;
      }

      const QString key = buildDataItemKey(dataItem);

      if (uniqueDataItems.contains(key))
      {
        continue;
      }

      uniqueDataItems.insert(key);
      m_dataItems.enqueue(dataItem);
    }
  }

  downloadNextDataItem();
}

void DownloadsManager::downloadDataItemsForSample(Sample* sample)
{
  if (!sample)
  {
    qWarning() << "DownloadsManager::downloadDataItemsForSample: sample is null!";
    return;
  }

  if (!m_samples)
  {
    qWarning() << "DownloadsManager::downloadDataItemsForSample: samples is null!";
    return;
  }

  setDownloadFailed(false);
  setIsBulkDownload(false); // Individual sample downloads use inline progress

  const int dataItemCount = sample->dataItems()->size();
  for (int j = 0; j < dataItemCount; ++j)
  {
    DataItem* dataItem = sample->dataItems()->at(j);
    if (!dataItem->exists())
    {
      m_dataItems.enqueue(dataItem);
    }
  }

  downloadNextDataItem();
}

void DownloadsManager::cancelAllDownloads()
{
  // Clear the queue to prevent new downloads from starting
  m_dataItems.clear();

  // Set cancel flag to prevent downloadNextDataItem from processing more
  setCancelDownload(true);

  if (m_dataItemProgress.isEmpty())
  {
    // This function perfroms checks for the cancel flag
    downloadNextDataItem();
  }
}

bool DownloadsManager::deleteAllOfflineData()
{
  if (!m_samples)
  {
    qWarning() << "DownloadsManager::deleteAllOfflineData: samples is null!";
    return false;
  }

  const int totalSamples = m_samples->size();
  bool success = true;

  for (int i = 0; i < totalSamples; ++i)
  {
    Sample* sample = m_samples->at(i);
    const int dataItemCount = sample->dataItems()->size();
    for (int j = 0; j < dataItemCount; ++j)
    {
      DataItem* dataItem = sample->dataItems()->at(j);
      deleteDataItemFile(dataItem);
    }
  }

  updateOfflineDataProjects();

  return success;
}

bool DownloadsManager::deleteProjectOfflineData(const QString& sampleName)
{
  if (!m_samples)
  {
    qWarning() << "DownloadsManager::deleteProjectOfflineData: samples is null!";
    return false;
  }

  if (sampleName.isEmpty())
  {
    qWarning() << "DownloadsManager::deleteProjectOfflineData: sampleName is empty!";
    return false;
  }

  const int totalSamples = m_samples->size();
  bool success = false;

  for (int i = 0; i < totalSamples; ++i)
  {
    Sample* sample = m_samples->at(i);
    if (sample->name().toString() == sampleName)
    {
      const int dataItemCount = sample->dataItems()->size();
      for (int j = 0; j < dataItemCount; ++j)
      {
        DataItem* dataItem = sample->dataItems()->at(j);
        deleteDataItemFile(dataItem);
      }
      success = true;
      break;
    }
  }

  // Refresh the offline data projects list
  updateOfflineDataProjects();

  return success;
}

bool DownloadsManager::hasOfflineData(const QString& sampleName)
{
  if (!m_samples)
  {
    qWarning() << "DownloadsManager::hasOfflineData: samples is null!";
    return false;
  }

  if (sampleName.isEmpty())
  {
    qWarning() << "DownloadsManager::hasOfflineData: sampleName is empty!";
    return false;
  }

  const int totalSamples = m_samples->size();

  for (int i = 0; i < totalSamples; ++i)
  {
    Sample* sample = m_samples->at(i);
    if (sample->name().toString() == sampleName)
    {
      const int dataItemCount = sample->dataItems()->size();
      for (int j = 0; j < dataItemCount; ++j)
      {
        DataItem* dataItem = sample->dataItems()->at(j);
        if (dataItem->exists())
        {
          return true;
        }
      }
      return false;
    }
  }
  return false;
}

OfflineDataProjectsModel* DownloadsManager::getOfflineDataProjects()
{
  if (!m_samples)
  {
    qWarning() << "DownloadsManager::getOfflineDataProjects: samples is null!";
    return m_offlineDataProjects;
  }

  updateOfflineDataProjects();
  return m_offlineDataProjects;
}

bool DownloadsManager::hasAnyDataToDownload()
{
  if (!m_samples)
  {
    qWarning() << "DownloadsManager::hasAnyDataToDownload: samples is null!";
    return false;
  }

  const int totalSamples = m_samples->size();

  for (int i = 0; i < totalSamples; ++i)
  {
    Sample* sample = m_samples->at(i);
    const int dataItemCount = sample->dataItems()->size();
    for (int j = 0; j < dataItemCount; ++j)
    {
      DataItem* dataItem = sample->dataItems()->at(j);
      if (!dataItem->exists())
      {
        return true;
      }
    }
  }
  return false;
}

bool DownloadsManager::hasAnyDataToDelete()
{
  if (!m_samples)
  {
    qWarning() << "DownloadsManager::hasAnyDataToDelete: samples is null!";
    return false;
  }

  const int totalSamples = m_samples->size();

  for (int i = 0; i < totalSamples; ++i)
  {
    Sample* sample = m_samples->at(i);
    const int dataItemCount = sample->dataItems()->size();
    for (int j = 0; j < dataItemCount; ++j)
    {
      DataItem* dataItem = sample->dataItems()->at(j);
      if (dataItem->exists())
      {
        return true;
      }
    }
  }
  return false;
}

double DownloadsManager::calculateSampleDownloadProgress(Sample* sample) const
{
  if (!sample || sample->dataItems()->isEmpty())
  {
    return 0.0;
  }

  double totalProgress = 0.0;
  int itemCount = 0;

  // Calculate aggregate progress for all data items in this sample
  for (int i = 0; i < sample->dataItems()->size(); ++i)
  {
    DataItem* dataItem = sample->dataItems()->at(i);
    const QString key = buildDataItemKey(dataItem);

    // Get downlaod progress
    if (m_dataItemProgress.contains(key))
    {
      totalProgress += m_dataItemProgress[key];
    }
    else if (dataItem->exists())
    {
      totalProgress += 100.0;
    }
    itemCount++;
  }

  return itemCount > 0 ? totalProgress / itemCount : 0.0;
}

void DownloadsManager::updateOfflineDataProjects()
{
  if (!m_samples)
  {
    qWarning() << "DownloadsManager::updateOfflineDataProjects: samples is null!";
    return;
  }

  const int totalSamples = m_samples->size();

  // Build list from scratch if empty (first time)
  if (m_offlineDataProjects->rowCount() == 0)
  {
    for (int i = 0; i < totalSamples; ++i)
    {
      Sample* sample = m_samples->at(i);
      if (sample->dataItems()->isEmpty())
      {
        continue;
      }
      // Use helper to get complete download state
      SampleDownloadState state = getSampleDownloadState(sample);
      m_offlineDataProjects->addProject(sample, state.downloaded, state.isDownloading, state.progress, state.downloadedItems, state.totalItems);
    }
  }
  else
  {
    // Update existing items in place
    int projectIndex = 0;
    for (int i = 0; i < totalSamples && projectIndex < m_offlineDataProjects->rowCount(); ++i)
    {
      Sample* sample = m_samples->at(i);
      if (sample->dataItems()->isEmpty())
      {
        continue;
      }
      SampleDownloadState state = getSampleDownloadState(sample);
      m_offlineDataProjects->updateProject(projectIndex, state.downloaded, state.isDownloading, state.progress, state.downloadedItems,
                                           state.totalItems);
      projectIndex++;
    }
  }

  // Notify QML that button states may need to update
  emit offlineDataStateChanged();
}

void DownloadsManager::downloadNextDataItem()
{
  if (cancelDownload())
  {
    // Check if there are still active downloads
    if (m_dataItemProgress.isEmpty())
    {
      // All active downloads are done, perform cleanup
      setDownloadText("Downloads cancelled");
      setDownloadProgress(0.0);
      setDownloadInProgress(false);
      setIsBulkDownload(false);

      if (m_progressUpdateTimer && m_progressUpdateTimer->isActive())
      {
        m_progressUpdateTimer->stop();
      }

      setCancelDownload(false);
      updateOfflineDataProjects();
      emit offlineDataStateChanged();
    }
    return;
  }

  if (!m_dataItems.isEmpty())
  {
    if (!m_portal)
    {
      qWarning() << "DownloadsManager: Portal not initialized. Call initialize() first.";
      return;
    }

    setDownloadText(QString("Remaining items in queue: %1").arg(m_dataItems.size()));
    setDownloadInProgress(true);

    // Start update timer
    if (m_progressUpdateTimer && !m_progressUpdateTimer->isActive())
    {
      m_progressUpdateTimer->start();
    }

    auto item = m_dataItems.dequeue();
    fetchPortalItemData(item->itemId(), m_homePath + item->path().mid(1));
  }
  else
  {
    setDownloadText("Downloads complete");

    setDownloadProgress(0.0);
    setDownloadInProgress(false);
    setIsBulkDownload(false);

    // Stop update timer
    if (m_progressUpdateTimer && m_progressUpdateTimer->isActive())
    {
      m_progressUpdateTimer->stop();
    }

    emit doneDownloadingChanged();
  }
}

void DownloadsManager::fetchPortalItemData(const QString& itemId, const QString& outputPath)
{
  const QString dataItemKey = itemId + "|" + outputPath;
  m_dataItemProgress[dataItemKey] = 0.0;

  auto portalItem = new PortalItem(m_portal, itemId, this);
  m_activeDownloads[dataItemKey] = portalItem;

  connect(portalItem, &PortalItem::doneLoading, this, [this, portalItem, outputPath, dataItemKey]()
  {
    onPortalItemLoaded(portalItem, outputPath, dataItemKey);
  });

  connect(portalItem, &PortalItem::fetchDataProgressChanged, this, [this, dataItemKey](const NetworkRequestProgress& progress)
  {
    if (m_dataItemProgress.contains(dataItemKey))
    {
      m_dataItemProgress[dataItemKey] = progress.progressPercentage();
    }
  });

  portalItem->load();
}

void DownloadsManager::onPortalItemLoaded(PortalItem* portalItem, const QString& outputPath, const QString& dataItemKey)
{
  if (portalItem->loadStatus() != LoadStatus::Loaded)
  {
    return;
  }

  const auto folder = portalItem->type() == PortalItemType::CodeSample ? portalItem->name() : QString();
  const QString formattedPath = this->formattedPath(outputPath, folder);

  portalItem->fetchDataAsync(formattedPath)
    .then(this,
          [this, portalItem, formattedPath, dataItemKey]()
  {
    setDownloadProgress(0.0);
    m_dataItemProgress[dataItemKey] = 100.0;

    if (QFileInfo(formattedPath).suffix() == "zip")
    {
      const QString extractPath = QFileInfo(formattedPath).dir().absolutePath();
      const QDir extractDir(extractPath);
      const QStringList filesBefore = extractDir.entryList(QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name);

      auto zipHelper = new ZipHelper(formattedPath, this);
      connect(zipHelper, &ZipHelper::extractCompleted, this, [this, formattedPath, extractPath, filesBefore, portalItem, zipHelper, dataItemKey]()
      {
        const QDir extractDir(extractPath);
        const QStringList filesAfter = extractDir.entryList(QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name);

        QStringList newFiles;
        newFiles.append(formattedPath);

        for (const QString& file : filesAfter)
        {
          if (filesBefore.contains(file))
          {
            continue;
          }

          const QString fullPath = extractPath + "/" + file;
          newFiles.append(fullPath);

          const QFileInfo fileInfo(fullPath);
          if (!fileInfo.isDir())
          {
            continue;
          }

          QDirIterator it(fullPath, QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
          while (it.hasNext())
          {
            newFiles.append(it.next());
          }
        }

        const QString itemId = dataItemKey.split("|").first();
        trackFilesForItem(itemId, newFiles);

        m_dataItemProgress.remove(dataItemKey);
        m_activeDownloads.remove(dataItemKey);
        downloadNextDataItem();
        portalItem->deleteLater();
        zipHelper->deleteLater();
      });
      zipHelper->extractAll(extractPath);
    }
    else
    {
      const QString itemId = dataItemKey.split("|").first();
      trackFilesForItem(itemId, QStringList() << formattedPath);

      m_dataItemProgress.remove(dataItemKey);
      m_activeDownloads.remove(dataItemKey);
      downloadNextDataItem();
      portalItem->deleteLater();
    }
  })
    .onFailed([this, portalItem, dataItemKey](const ErrorException&)
  {
    setDownloadFailed(true);
    setDownloadText(QString("Download failed for item %1").arg(portalItem->itemId()));
    setDownloadInProgress(false);
    setIsBulkDownload(false);

    if (m_progressUpdateTimer && m_progressUpdateTimer->isActive())
    {
      m_progressUpdateTimer->stop();
    }

    m_dataItemProgress.remove(dataItemKey);
    m_activeDownloads.remove(dataItemKey);
  });
}

QString DownloadsManager::formattedPath(const QString& outputPath, const QString& folderName)
{
  // first make sure output path exists
  if (!QFile::exists(outputPath))
  {
    const QFileInfo fileInfo = QFileInfo(outputPath);
    QDir dir = fileInfo.dir();
    if (!dir.exists())
    {
      dir.mkpath(".");
    }
  }

  // create the download data task
  QString formattedFilePath = outputPath;

  // check if the item is code sample
  if (!folderName.isEmpty())
  {
    const QFileInfo fileInfo = QFileInfo(outputPath);
    const QString dir = fileInfo.dir().absolutePath();
    formattedFilePath = dir + "/" + folderName;
  }

  return formattedFilePath;
}
