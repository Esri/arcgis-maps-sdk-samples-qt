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

#ifndef SAMPLEMANAGER_H
#define SAMPLEMANAGER_H

class QAbstractItemModel;
class CategoryListModel;
class DataItem;
class DownloadsManager;
class OfflineDataProjectsModel;
class QTemporaryDir;
class SampleListModel;
class SourceCode;
class SampleCategory;
class Sample;

namespace Esri::ArcGISRuntime
{
  class Portal;
  class PortalItem;
} // namespace Esri::ArcGISRuntime

namespace Esri::ArcGISRuntime::Authentication
{
  class ArcGISAuthenticationChallengeHandler;
}

#include <QDir>
#include <QJsonDocument>
#include <QMap>
#include <QObject>
#include <QQueue>
#include <QString>
#include <QTimer>
#include <QNetworkInformation>
#include <QUrl>
#include <QVariantList>

Q_MOC_INCLUDE("SampleListModel.h")
Q_MOC_INCLUDE("CategoryListModel.h")
Q_MOC_INCLUDE("Sample.h")
Q_MOC_INCLUDE("SampleCategory.h")
Q_MOC_INCLUDE("OfflineDataProjectsModel.h")
Q_MOC_INCLUDE("DownloadsManager.h")

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
  Q_PROPERTY(QString api READ api CONSTANT)
  Q_PROPERTY(bool showApiKeyOption READ showApiKeyOption CONSTANT)
  Q_PROPERTY(Reachability reachability READ reachability NOTIFY reachabilityChanged)
  Q_PROPERTY(DownloadsManager* downloadsManager READ downloadsManager CONSTANT)
  Q_PROPERTY(SampleListModel* favoriteSamples READ favoriteSamples NOTIFY favoriteSamplesChanged)

public:
  explicit SampleManager(QObject* parent = nullptr);
  ~SampleManager() override;

  Q_INVOKABLE void init();

  // Data management
  Q_INVOKABLE bool dataItemsExists();

  // Download operations (bulk and individual)
  Q_INVOKABLE void downloadAllDataItems();
  Q_INVOKABLE void downloadDataItemsCurrentSample();
  Q_INVOKABLE void downloadProjectData(const QString& sampleName);
  Q_INVOKABLE void cancelAllDownloads();

  // Delete operations
  Q_INVOKABLE bool deleteAllOfflineData();
  Q_INVOKABLE bool deleteProjectOfflineData(const QString& sampleName);

  // Query methods
  Q_INVOKABLE bool hasOfflineData(const QString& sampleName);
  Q_INVOKABLE OfflineDataProjectsModel* getOfflineDataProjects();
  Q_INVOKABLE bool hasAnyDataToDownload() const;
  Q_INVOKABLE bool hasAnyDataToDelete() const;

  // Utility methods
  Q_INVOKABLE void setSourceCodeIndex(int i);
  Q_INVOKABLE void setupProxy(const QString& hostName, quint16 port, const QString& user, const QString& pw);
  Q_INVOKABLE void setCurrentApiKeyToMapsSDK();
  Q_INVOKABLE void unsetApiKeyFromMapsSDK();
  Q_INVOKABLE void setApiKey(const QString& enteredApiKey);
  Q_INVOKABLE bool showApiKeyOption() const;
  Q_INVOKABLE void resetAuthenticationState();

  // Favorites
  Q_INVOKABLE void addSampleToFavorites(Sample* sample);
  Q_INVOKABLE void removeSampleFromFavorites(Sample* sample);
  Q_INVOKABLE bool isFavorite(Sample* sample) const;

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
  void currentModeChanged();
  void currentSampleChanged();
  void currentCategoryChanged();
  void currentSourceCodeChanged();
  void reachabilityChanged();
  void favoriteSamplesChanged();

protected:
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
  bool currentSampleSupportsApiKey() const;
  void cacheToolkitChallengeHandler();
  void setCurrentSample(Sample* sample);
  void setCurrentSample(const QVariant& sample);

  SampleCategory* currentCategory() const;
  void setCurrentCategory(SampleCategory* category);
  QString currentSourceCode() const;
  QUrl apiReferenceUrl() const;
  QUrl qtSdkUrl() const;
  QUrl qtSamplesUrl() const;

  // Downloads manager accessor
  DownloadsManager* downloadsManager() const;

  // Favorites
  SampleListModel* favoriteSamples() const;
  void initFavorites();
  void saveSampleListToFavorites(const SampleListModel* samples);

  // Other helpers
  SampleManager::Reachability reachability() const;
  QString api() const;
  void createAndSetTempDirForLocalServer();

private:
  // Downloads and offline data management
  DownloadsManager* m_downloadsManager = nullptr;
  Esri::ArcGISRuntime::Portal* m_portal = nullptr;

  // Sample/Category data
  CategoryListModel* m_categories = nullptr;
  SampleListModel* m_allSamples = nullptr;
  SampleListModel* m_featuredSamples = nullptr;
  SampleListModel* m_offlineDataSamples = nullptr;
  Sample* m_currentSample = nullptr;
  SampleCategory* m_currentCategory = nullptr;
  QString m_currentSourceCode;

  // UI state
  CurrentMode m_currentMode = CurrentMode::LiveSampleView;

  // Links
  QUrl m_apiReferenecUrl = QUrl("https://links.esri.com/qtApiRef");
  QUrl m_qtSdkUrl = QUrl("https://links.esri.com/qtDevelopersPage");
  QUrl m_qtSamplesUrl = QUrl("https://links.esri.com/qtSamples");

  // Other
  std::unique_ptr<QTemporaryDir> m_tempDir;
  Esri::ArcGISRuntime::Authentication::ArcGISAuthenticationChallengeHandler* m_toolkitChallengeHandler = nullptr;

  // Favorites
  SampleListModel* m_favoriteSamples = nullptr;
};

#endif // SAMPLEMANAGER_H
