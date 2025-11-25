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

#ifndef DOWNLOADSMANAGER_H
#define DOWNLOADSMANAGER_H

class DataItem;
class OfflineDataProjectsModel;
class Sample;
class SampleListModel;
class QTimer;

namespace Esri::ArcGISRuntime
{
  class Portal;
  class PortalItem;
} // namespace Esri::ArcGISRuntime

#include <QMap>
#include <QObject>
#include <QQueue>
#include <QString>

Q_MOC_INCLUDE("OfflineDataProjectsModel.h")

class DownloadsManager : public QObject
{
  Q_OBJECT

  Q_PROPERTY(bool downloadInProgress READ downloadInProgress NOTIFY downloadInProgressChanged)
  Q_PROPERTY(bool isBulkDownload READ isBulkDownload NOTIFY isBulkDownloadChanged)
  Q_PROPERTY(QString downloadText READ downloadText NOTIFY downloadTextChanged)
  Q_PROPERTY(double downloadProgress READ downloadProgress NOTIFY downloadProgressChanged)
  Q_PROPERTY(bool cancelDownload READ cancelDownload WRITE setCancelDownload NOTIFY cancelDownloadChanged)
  Q_PROPERTY(bool downloadFailed READ downloadFailed NOTIFY downloadFailedChanged)
  Q_PROPERTY(OfflineDataProjectsModel* offlineDataProjects READ offlineDataProjects NOTIFY offlineDataStateChanged)

public:
  explicit DownloadsManager(QObject* parent = nullptr);
  ~DownloadsManager() override;

  // Initialization
  void initialize(Esri::ArcGISRuntime::Portal* portal, const QString& homePath);

  // Download operations
  void downloadAllDataItems();
  void downloadDataItemsForSample(Sample* sample);
  void cancelAllDownloads();

  // Delete operations
  bool deleteAllOfflineData();
  bool deleteProjectOfflineData(const QString& sampleName);

  // Query methods
  bool hasOfflineData(const QString& sampleName);
  OfflineDataProjectsModel* getOfflineDataProjects();
  bool hasAnyDataToDownload();
  bool hasAnyDataToDelete();

  // State getters
  bool downloadInProgress() const;
  bool isBulkDownload() const;
  QString downloadText() const;
  double downloadProgress() const;
  bool cancelDownload() const;
  bool downloadFailed() const;
  OfflineDataProjectsModel* offlineDataProjects() const;

  // State setters
  void setCancelDownload(bool cancel);

  void setSamples(SampleListModel* samples)
  {
    m_samples = samples;
  }

signals:
  void downloadInProgressChanged();
  void isBulkDownloadChanged();
  void downloadTextChanged();
  void downloadProgressChanged();
  void cancelDownloadChanged();
  void downloadFailedChanged();
  void offlineDataStateChanged();
  void doneDownloadingChanged();

private:
  static constexpr int PROGRESS_UPDATE_INTERVAL_MS = 200;

  struct SampleDownloadState
  {
    int totalItems = 0;
    int downloadedItems = 0;
    bool downloaded = false;
    bool isDownloading = false;
    double progress = 0.0;
  };

  SampleDownloadState getSampleDownloadState(Sample* sample) const;

  int countDownloadedItems(Sample* sample) const;
  bool isSampleDownloading(Sample* sample) const;
  void deleteDataItemFile(DataItem* dataItem);
  void cancelDataItem(const QString& dataItemKey);
  void cleanupEmptyParentDirectories(const QString& startPath, const QString& boundary);

  QJsonObject loadDownloadTracking();
  void saveDownloadTracking(const QJsonObject& data);
  void trackFilesForItem(const QString& itemId, const QStringList& files);

  void downloadNextDataItem();
  void fetchPortalItemData(const QString& itemId, const QString& outputPath);
  void updateOfflineDataProjects();
  double calculateSampleDownloadProgress(Sample* sample) const;
  QString formattedPath(const QString& outputPath, const QString& folderName = QString());

  void setDownloadInProgress(bool inProgress);
  void setIsBulkDownload(bool isBulk);
  void setDownloadText(const QString& text);
  void setDownloadProgress(double progress);
  void setDownloadFailed(bool failed);

  QQueue<DataItem*> m_dataItems;
  QMap<QString, double> m_dataItemProgress;
  QMap<QString, Esri::ArcGISRuntime::PortalItem*> m_activeDownloads;
  QTimer* m_progressUpdateTimer = nullptr;

  Esri::ArcGISRuntime::Portal* m_portal = nullptr;
  QString m_homePath;
  SampleListModel* m_samples = nullptr;

  OfflineDataProjectsModel* m_offlineDataProjects = nullptr;

  // Download state
  bool m_downloadInProgress = false;
  bool m_isBulkDownload = false;
  QString m_downloadText = QString("Downloading");
  double m_downloadProgress = 0.0;
  bool m_cancelDownload = false;
  bool m_downloadFailed = false;
};

#endif // DOWNLOADSMANAGER_H
