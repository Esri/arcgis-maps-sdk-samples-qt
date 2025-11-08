// [Legal]
// Copyright 2022 Esri.
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

#ifndef SAMPLEMANAGER_H
#define SAMPLEMANAGER_H

class QAbstractItemModel;
class CategoryListModel;
class DataItem;
class QTemporaryDir;
class SampleListModel;
class SourceCode;
class SampleCategory;
class Sample;
class OfflineDataProjectsModel;

namespace Esri::ArcGISRuntime
{
  class Portal;
  class PortalItem;
}

namespace Esri::ArcGISRuntime::Authentication
{
  class ArcGISAuthenticationChallengeHandler;
}

#include <QDir>
#include <QJsonDocument>
#include <QObject>
#include <QQueue>
#include <QString>
#include <QNetworkInformation>
#include <QUrl>
#include <QMap>
#include <QTimer>

Q_MOC_INCLUDE("SampleListModel.h")
Q_MOC_INCLUDE("CategoryListModel.h")
Q_MOC_INCLUDE("Sample.h")
Q_MOC_INCLUDE("SampleCategory.h")
Q_MOC_INCLUDE("OfflineDataProjectsModel.h")

class SampleManager : public QObject
{
  Q_OBJECT

  Q_PROPERTY(SampleListModel* samples READ samples NOTIFY sampleInitComplete)
  Q_PROPERTY(SampleListModel* featuredSamples READ featuredSamples NOTIFY featuredSamplesChanged)
  Q_PROPERTY(CategoryListModel* categories READ categories NOTIFY sampleInitComplete)
  Q_PROPERTY(CurrentMode currentMode READ currentMode WRITE setCurrentMode NOTIFY currentModeChanged)
  Q_PROPERTY(Sample* currentSample READ currentSample WRITE setCurrentSample NOTIFY currentSampleChanged)
  Q_PROPERTY(SampleCategory* currentCategory READ currentCategory WRITE setCurrentCategory NOTIFY currentCategoryChanged)
  Q_PROPERTY(QString currentSourceCode READ currentSourceCode NOTIFY currentSourceCodeChanged)
  Q_PROPERTY(QUrl apiReferenceUrl READ apiReferenceUrl NOTIFY sampleInitComplete)
  Q_PROPERTY(QUrl qtSdkUrl READ qtSdkUrl NOTIFY sampleInitComplete)
  Q_PROPERTY(QUrl qtSamplesUrl READ qtSamplesUrl NOTIFY sampleInitComplete)
  Q_PROPERTY(bool downloadInProgress READ downloadInProgress WRITE setDownloadInProgress NOTIFY downloadInProgressChanged)
  Q_PROPERTY(QString downloadText READ downloadText WRITE setDownloadText NOTIFY downloadTextChanged)
  Q_PROPERTY(double downloadProgress READ downloadProgress WRITE setDownloadProgress NOTIFY downloadProgressChanged)
  Q_PROPERTY(bool cancelDownload READ cancelDownload WRITE setCancelDownload NOTIFY cancelDownloadChanged)
  Q_PROPERTY(bool downloadFailed READ downloadFailed WRITE setDownloadFailed NOTIFY downloadFailedChanged)
  Q_PROPERTY(QString api READ api CONSTANT)
  Q_PROPERTY(Reachability reachability READ reachability NOTIFY reachabilityChanged)
  Q_PROPERTY(OfflineDataProjectsModel* offlineDataProjects READ offlineDataProjects CONSTANT)
  Q_PROPERTY(bool showFullScreenDownload READ showFullScreenDownload WRITE setShowFullScreenDownload NOTIFY showFullScreenDownloadChanged)

public:
  explicit SampleManager(QObject* parent = nullptr);
  ~SampleManager() override;

  Q_INVOKABLE void init();

  Q_INVOKABLE bool dataItemsExists();
  Q_INVOKABLE void resetAuthenticationState();
  Q_INVOKABLE void downloadAllDataItems();
  Q_INVOKABLE void downloadDataItemsCurrentSample();
  Q_INVOKABLE bool deleteAllOfflineData();
  Q_INVOKABLE bool deleteProjectOfflineData(const QString& sampleName);
  Q_INVOKABLE void downloadProjectData(const QString& sampleName);
  Q_INVOKABLE bool hasOfflineData(const QString& sampleName);
  Q_INVOKABLE OfflineDataProjectsModel* getOfflineDataProjects();
  Q_INVOKABLE void setSourceCodeIndex(int i);
  Q_INVOKABLE void setupProxy(const QString& hostName, quint16 port, const QString& user, const QString& pw);
  Q_INVOKABLE void doneDownloading();
  Q_INVOKABLE void setApiKey(bool isSupportsApiKey = true);
  Q_INVOKABLE void cancelSampleDownload(const QString& sampleName);

  enum CurrentMode
  {
    LiveSampleView,
    SourceCodeView,
    DescriptionView,
    ManageOfflineDataView,
    NetworkRequiredView,
    DownloadDataView,
    HomepageView
  };
  Q_ENUM(CurrentMode)

  enum class Reachability
  {
    ReachabilityOnline = static_cast<int>(QNetworkInformation::Reachability::Online),
    ReachabilitySite = static_cast<int>(QNetworkInformation::Reachability::Site),
    ReachabilityLocal = static_cast<int>(QNetworkInformation::Reachability::Local),
    ReachabilityDisconnected = static_cast<int>(QNetworkInformation::Reachability::Disconnected),
    ReachabilityUnknown = static_cast<int>(QNetworkInformation::Reachability::Unknown)
  };
  Q_ENUM(Reachability)

signals:
  void sampleInitComplete();
  void featuredSamplesChanged();
  void cancelDownloadChanged();
  void currentModeChanged();
  void currentSampleChanged();
  void currentCategoryChanged();
  void currentSourceCodeChanged();
  void doneDownloadingChanged();
  void downloadFailedChanged();
  void downloadInProgressChanged();
  void downloadTextChanged();
  void downloadProgressChanged();
  void reachabilityChanged();
  void offlineDataProjectsChanged();
  void showFullScreenDownloadChanged();

protected:
  void setDownloadProgress(double progress);
  void buildCategoriesList();
  SampleCategory* createCategory(const QString& name, const QString& displayName, const QDir& dir);
  bool appendCategoryToManager(SampleCategory* category);

private:
  SampleListModel* buildSamplesList(const QDir& dir, const QString& prefix);
  QVariantMap toVariantMap(const QString& json);
  QVariant fileUrl(const QString& scheme, const QString& path);
  QString readTextFile(const QString& filePath);

  SampleListModel* samples() const;
  SampleListModel* featuredSamples() const;
  CategoryListModel* categories();
  CurrentMode currentMode();
  void setCurrentMode(const CurrentMode& mode);

  Sample* currentSample() const;
  void cacheToolkitChallengeHandler();
  void setCurrentSample(Sample* sample);
  void setCurrentSample(const QVariant& sample);

  SampleCategory* currentCategory() const;
  void setCurrentCategory(SampleCategory* category);
  QString currentSourceCode() const;
  QUrl apiReferenceUrl() const;
  QUrl qtSdkUrl() const;
  QUrl qtSamplesUrl() const;

  bool downloadInProgress() const;
  void downloadNextDataItem();
  void fetchPortalItemData(const QString& itemId, const QString& outputPath);
  void setDownloadInProgress(bool inProgress);
  void setDownloadText(const QString& downloadText);
  QString formattedPath(const QString& outputPath, const QString& folderName = QString());

  bool showFullScreenDownload() const;
  void setShowFullScreenDownload(bool show);
  void updateDownloadProgress();

private:
  QString downloadText() const;
  double downloadProgress() const;

  void createAndSetTempDirForLocalServer();

  bool cancelDownload() const;
  void setCancelDownload(bool cancel);

  bool downloadFailed() const;
  void setDownloadFailed(bool didFail);

  SampleManager::Reachability reachability() const;
  QString api() const;

  OfflineDataProjectsModel* offlineDataProjects() const;
  void updateOfflineDataProjects();

  double calculateSampleDownloadProgress(Sample* sample) const;
  bool m_showFullScreenDownload = false;
  int m_totalDownloadItems = 0;

private:
  QQueue<DataItem*> m_dataItems;
  Esri::ArcGISRuntime::Portal* m_portal = nullptr;
  CategoryListModel* m_categories = nullptr;
  SampleListModel* m_allSamples = nullptr;
  SampleListModel* m_featuredSamples = nullptr;
  CurrentMode m_currentMode = CurrentMode::LiveSampleView;
  Sample* m_currentSample = nullptr;
  SampleCategory* m_currentCategory = nullptr;
  QString m_currentSourceCode;
  QUrl m_apiReferenecUrl = QUrl("https://links.esri.com/qtApiRef");
  QUrl m_qtSdkUrl = QUrl("https://links.esri.com/qtDevelopersPage");
  QUrl m_qtSamplesUrl = QUrl("https://links.esri.com/qtSamples");
  bool m_downloadInProgress = false;
  QString m_downloadText = QString("Downloading");
  double m_downloadProgress = 0.0;
  std::unique_ptr<QTemporaryDir> m_tempDir;
  bool m_cancelDownload = false;
  bool m_downloadFailed = false;
  Esri::ArcGISRuntime::Authentication::ArcGISAuthenticationChallengeHandler* m_toolkitChallengeHandler = nullptr;
  OfflineDataProjectsModel* m_offlineDataProjects = nullptr;
  QMap<QString, double> m_dataItemProgress;
  QMap<QString, Esri::ArcGISRuntime::PortalItem*> m_activeDownloads;
  QTimer* m_progressUpdateTimer = nullptr;
};

#endif // SAMPLEMANAGER_H
