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

#include "pch.hpp"

#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QDomElement>
#include <QDomNodeList>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkProxy>
#include <QStandardPaths>
#include <QStringList>
#include <QTemporaryDir>
#include <QUrl>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>
#include <QQmlEngine>
#include <NetworkRequestProgress.h>

#include "ArcGISQt_global.h" // for LOCALSERVER_SUPPORTED
#include "ArcGISRuntimeEnvironment.h"
#include "Authentication/ArcGISCredentialStore.h"
#include "Authentication/AuthenticationManager.h"
#include "Authentication/NetworkCredentialStore.h"
#include "ErrorException.h"
#include "Portal.h"
#include "PortalItem.h"

// toolkit authentication support
#include "OAuthUserConfigurationManager.h"

#include "ZipHelper.h"

#include "CategoryListModel.h"
#include "DataItem.h"
#include "DataItemListModel.h"
#include "Error.h"
#include "MapTypes.h"
#include "PortalTypes.h"
#include "SampleCategory.h"
#include "Sample.h"
#include "SampleListModel.h"
#include "SampleManager.h"
#include "SampleManager_definitions.h"
#include "SourceCode.h"
#include "SourceCodeListModel.h"
#include "OfflineDataProjectsModel.h"

#include <cstdlib>

using namespace Esri::ArcGISRuntime;

#ifdef LOCALSERVER_SUPPORTED
#include "LocalServer.h"
#endif // LOCALSERVER_SUPPORTED

#define STRINGIZE(x) #x
#define QUOTE(x) STRINGIZE(x)

namespace
{
  QString apiKey = ""; // Provide your API key here
}

SampleManager::SampleManager(QObject* parent) :
  QObject(parent),
  m_categories(new CategoryListModel(this)),
  m_allSamples(new SampleListModel(this)),
  m_offlineDataProjects(new OfflineDataProjectsModel(this)),
  m_progressUpdateTimer(new QTimer(this))
{
#ifdef LOCALSERVER_SUPPORTED
  createAndSetTempDirForLocalServer();
#endif // LOCALSERVER_SUPPORTED

  if (QNetworkInformation::loadBackendByFeatures(QNetworkInformation::Feature::Reachability))
  {
    if (QNetworkInformation* networkInfo = QNetworkInformation::instance())
    {
      connect(networkInfo, &QNetworkInformation::reachabilityChanged, this, &SampleManager::reachabilityChanged);
    }
  }

  // UI update timer
  m_progressUpdateTimer->setInterval(100);
  m_progressUpdateTimer->setSingleShot(false);
  connect(m_progressUpdateTimer, &QTimer::timeout, this, [this]()
  {
    updateOfflineDataProjects();
    updateDownloadProgress();
  });
}

SampleManager::~SampleManager() = default;

SampleListModel* SampleManager::samples() const
{
  return m_allSamples;
}

SampleListModel* SampleManager::featuredSamples() const
{
  return m_featuredSamples;
}

CategoryListModel* SampleManager::categories()
{
  return m_categories;
}

SampleManager::CurrentMode SampleManager::currentMode()
{
  return m_currentMode;
}

Sample* SampleManager::currentSample() const
{
  return m_currentSample;
}

SampleCategory* SampleManager::currentCategory() const
{
  return m_currentCategory;
}

QString SampleManager::currentSourceCode() const
{
  return m_currentSourceCode;
}

QUrl SampleManager::apiReferenceUrl() const
{
  return m_apiReferenecUrl;
}

QUrl SampleManager::qtSdkUrl() const
{
  return m_qtSdkUrl;
}

QUrl SampleManager::qtSamplesUrl() const
{
  return m_qtSamplesUrl;
}

bool SampleManager::downloadInProgress() const
{
  return m_downloadInProgress;
}

QString SampleManager::downloadText() const
{
  return m_downloadText;
}

double SampleManager::downloadProgress() const
{
  return m_downloadProgress;
}

bool SampleManager::cancelDownload() const
{
  return m_cancelDownload;
}

bool SampleManager::downloadFailed() const
{
  return m_downloadFailed;
}

OfflineDataProjectsModel* SampleManager::offlineDataProjects() const
{
  return m_offlineDataProjects;
}

void SampleManager::init()
{
  m_featuredSamples = new SampleListModel(this);
  buildCategoriesList();
  emit sampleInitComplete();
}

void SampleManager::createAndSetTempDirForLocalServer()
{
#ifdef LOCALSERVER_SUPPORTED
  // create temp/data path
  QString localServerTempDir;
  // get tmp and home paths
  const QString tmpPath = QDir::tempPath();
  const QString homePath = QDir::homePath();

  // use whichever is shorter, temp or home path
  if (homePath.length() > tmpPath.length())
  {
    localServerTempDir = tmpPath + "/EsriQtTemp";
  }
  else
  {
    localServerTempDir = homePath + "/EsriQtTemp";
  }

  // create the directory
  m_tempDir = std::make_unique<QTemporaryDir>(localServerTempDir);

  // set the temp & app data path for the local server
  LocalServer::setTempDataPath(m_tempDir->path());
  LocalServer::setAppDataPath(m_tempDir->path());
#endif // LOCALSERVER_SUPPORTED
}

void SampleManager::setCancelDownload(bool cancel)
{
  m_cancelDownload = cancel;
  emit cancelDownloadChanged();
}

void SampleManager::setDownloadFailed(bool didFail)
{
  m_downloadFailed = didFail;
  emit downloadFailedChanged();
}

// Build the Categories List
void SampleManager::buildCategoriesList()
{
  const QDir dir(DIRNAMESAMPLES);
  QFile xmlFile(":/Samples/Categories.xml");

  // Go through the XML to find the sample categories
  if (xmlFile.exists())
  {
    xmlFile.open(QIODevice::ReadOnly);
    QDomDocument doc;
    doc.setContent(&xmlFile);
    const QDomNodeList fileList = doc.documentElement().elementsByTagName("category");

    for (int i = 0; i < fileList.count(); i++)
    {
      // The first node contains the name
      const QDomNode node = fileList.at(i).childNodes().at(0);
      const QString name = node.toElement().text();
      // The second node contains the display name
      const QDomNode nameNode = fileList.at(i).childNodes().at(1);
      const QString displayName = nameNode.toElement().text();
      appendCategoryToManager(createCategory(name, displayName, dir));
    }
  }
  else
  {
    qWarning() << "could not find Categories.xml";
  }

#ifdef LOCALSERVER_SUPPORTED
  if (LocalServer::isInstallValid())
  {
    appendCategoryToManager(createCategory("LocalServer", "Local Server", dir));
  }
#endif // LOCALSERVER_SUPPORTED
}

SampleCategory* SampleManager::createCategory(const QString& name, const QString& displayName, const QDir& dir)
{
  // get path
  QDir categoryDir(dir.filePath(name));
  QString path = "qrc" + categoryDir.path();
  // get thumbnail URL
  QString thumbnailUrl = "qrc" + dir.filePath(name + ".png");
  // get background thumbnail URL
  QString backgroundThumbnailUrl = "qrc" + dir.filePath("BG_" + name + ".png");
  // get the samples in the category
  auto samples = buildSamplesList(categoryDir, "qrc");
  // make sure the category has samples in it
  if (samples->rowCount() == 0)
  {
    return nullptr;
  }
  // add to the list model
  return new SampleCategory(name, displayName, path, thumbnailUrl, backgroundThumbnailUrl, samples, this);
}

bool SampleManager::appendCategoryToManager(SampleCategory* category)
{
  if (nullptr != category)
  {
    m_categories->addCategory(category);
    const auto totalRows = category->samples()->rowCount();
    for (int i = 0; i < totalRows; ++i)
    {
      m_allSamples->addSample(category->samples()->at(i));
    }
    return true;
  }

  return false;
}

// Build the Samples List
SampleListModel* SampleManager::buildSamplesList(const QDir& dir, const QString& prefix)
{
  const auto sampleList = dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs, QDir::Name);

  auto samples = new SampleListModel(this);

  for (const auto& sample : sampleList)
  {
    QDir sampleDir(dir.filePath(sample));
    // Create a SourceCodeListModel
    auto codeFiles = new SourceCodeListModel(this);

    // Create a DataItemListModel
    auto dataItems = new DataItemListModel(this);

    // Obtain sample info from JSON
    auto sampleConfig = toVariantMap(readTextFile(sampleDir.filePath("sample.json")));
    auto name = sampleConfig.contains(PROPERTYNAME) ? sampleConfig.value(PROPERTYNAME).toString() : sample;

    // Get the name of the sample's QML source file from the README.metadata.json snippets section - often titled "SampleName.qml"
    // Sample snippets will list the primary C++ file name first (often SampleName.cpp) so it initially displays upon loading the sample in the ArcGIS Developer website
    // Sample snippets will list the QML source file name *last* to ensure consistent retrieval (rather than by a specific index)
    auto sourceFileName = sampleConfig.value(PROPERTYSOURCE).isValid() ? sampleConfig.value(PROPERTYSOURCE).toStringList().last() : "sample.qml";

    auto descriptionFileName = "README.md";
    auto thumbnailFileName = "screenshot.png";
    auto sourceFiles = sampleConfig.value(PROPERTYSOURCE);
    auto offlineDataItems = sampleConfig.value(PROPERTYDATAITEMS);

    // Set up sample info variables
    auto sampleName = name;
    auto samplePath = fileUrl(prefix, sampleDir.path());
    auto sampleSource = fileUrl(prefix, sampleDir.filePath(sourceFileName));
    auto sampleDescription = readTextFile(sampleDir.filePath(descriptionFileName));
    auto sampleThumbnailUrl = fileUrl(prefix, sampleDir.filePath(thumbnailFileName));

    // Construct path to sample screenshot and replace existing
    // * Note: This is done for the sampleviewer since it defaults to qrc:/qml/
    // * which is not where the sample screenshot is located
    auto sampleScreenshotReplacement = QString("![Image](%1/screenshot.png)").arg(samplePath.toString());
    sampleDescription.replace(QString("![](screenshot.png)"), sampleScreenshotReplacement);

    // Add source files to list model
    if (sourceFiles.isValid() && sourceFiles.toStringList().length() > 0)
    {
      const auto files = sourceFiles.toStringList();
      for (const auto& sourceFile : files)
      {
        codeFiles->addCodeFile(new SourceCode(sourceFile, readTextFile(sampleDir.filePath(sourceFile)), samplePath.toString(), this));
      }
    }

    // Add data Items to list model
    if (offlineDataItems.isValid() && offlineDataItems.toStringList().length() > 0)
    {
      const auto items = offlineDataItems.toList();
      for (const auto& dataItem : items)
      {
        auto itemId = dataItem.toMap()[PROPERTYITEMID].toString();
        auto path = dataItem.toMap()[PROPERTYPATH].toString();
        dataItems->addDataItem(new DataItem(itemId, path, this));
      }
    }

    // Create the sample
    Sample* newSample = new Sample(sampleName, samplePath, sampleSource, sampleDescription, codeFiles, sampleThumbnailUrl, dataItems, this);
    samples->addSample(newSample);

    // Add to featured samples list if necessary
    if (sampleConfig.contains(PROPERTYFEATURED) && sampleConfig.value(PROPERTYFEATURED).toBool())
    {
      m_featuredSamples->addSample(newSample);
      emit featuredSamplesChanged();
    }
  }

  return samples;
}

QVariantMap SampleManager::toVariantMap(const QString& json)
{
  auto jsonDocument = QJsonDocument::fromJson(json.toUtf8());

  if (jsonDocument.isObject())
  {
    return jsonDocument.object().toVariantMap();
  }
  else
  {
    return QVariantMap();
  }
}

QString SampleManager::readTextFile(const QString& filePath)
{
  QFile file(filePath);
  if (!file.exists())
  {
    return QString();
  }

  file.open(QIODevice::ReadOnly);
  auto data = file.readAll();
  file.close();

  return data;
}

QVariant SampleManager::fileUrl(const QString& scheme, const QString& path)
{
  return scheme.isEmpty() ? QUrl::fromLocalFile(path) : scheme + path;
}

void SampleManager::setCurrentMode(const CurrentMode& mode)
{
  m_currentMode = mode;
  emit currentModeChanged();
}

void SampleManager::cacheToolkitChallengeHandler()
{
  // Only cache the first instance of the challenge handler from the toolkit
  if (m_toolkitChallengeHandler == nullptr)
  {
    m_toolkitChallengeHandler = ArcGISRuntimeEnvironment::authenticationManager()->arcGISAuthenticationChallengeHandler();
  }
}

void SampleManager::setCurrentSample(Sample* sample)
{
  cacheToolkitChallengeHandler();
  m_currentSample = sample;

  // NOTE - currently we know we cannot set an API Key for the
  // following samples since they use named user login instead.

  const QString sampleName = m_currentSample->name().toString();
  if (sampleName == "Create and save map" || sampleName == "Add items to portal" || sampleName == "Search for web map by keyword")
  {
    setApiKey(false);
  }
  else
  {
    setApiKey(true);
  }

  emit currentSampleChanged();
}

void SampleManager::setCurrentSample(const QVariant& sample)
{
  cacheToolkitChallengeHandler();
  if (sample.isValid())
  {
    auto samplePtr = qvariant_cast<Sample*>(sample);
    if (samplePtr != m_currentSample)
    {
      m_currentSample = samplePtr;
      emit currentSampleChanged();
    }
  }
}

void SampleManager::setCurrentCategory(SampleCategory* category)
{
  m_currentCategory = category;
  emit currentCategoryChanged();
}

void SampleManager::setDownloadInProgress(bool inProgress)
{
  m_downloadInProgress = inProgress;
  emit downloadInProgressChanged();
}

void SampleManager::setDownloadText(const QString& downloadText)
{
  m_downloadText = downloadText;
  emit downloadTextChanged();
}

void SampleManager::setApiKey(bool isSupportsApiKey)
{
#ifdef SAMPLE_VIEWER_API_KEY
  // If the API key identifier is defined in the respective .pro file it will be used here
  // Otherwise use the API key provided by the user at the top of this file
  apiKey = QUOTE(SAMPLE_VIEWER_API_KEY);
#endif

  if (isSupportsApiKey && apiKey == "")
  {
    qWarning() << "This sample expects an API key to be set, but none was provided. Please provide an API key in SampleViewer/SampleManager.cpp";
  }
  const QString sampleApiKey = isSupportsApiKey ? apiKey : ""; // empty string will "unset" the key
  // set apikey for the sample viewer
  ArcGISRuntimeEnvironment::setApiKey(sampleApiKey);
}

void SampleManager::setDownloadProgress(double progress)
{
  m_downloadProgress = progress;
  emit downloadProgressChanged();
}

void SampleManager::resetAuthenticationState()
{
  // clear all credentials
  ArcGISRuntimeEnvironment::authenticationManager()->arcGISCredentialStore()->removeAll();
  auto removeAllFuture = ArcGISRuntimeEnvironment::authenticationManager()->networkCredentialStore()->removeAllAsync();
  Q_UNUSED(removeAllFuture)

  // and remove any oauth configurations
  Toolkit::OAuthUserConfigurationManager::clearConfigurations();

  if (m_toolkitChallengeHandler != nullptr)
  {
    // when sample changes, restore the original toolkit challenge handler
    ArcGISRuntimeEnvironment::authenticationManager()->setArcGISAuthenticationChallengeHandler(m_toolkitChallengeHandler);
  }
}

bool SampleManager::dataItemsExists()
{
  // Sample has no data items, return true
  if (currentSample()->dataItems()->size() == 0)
  {
    return true;
  }

  for (auto dataItem : *currentSample()->dataItems())
  {
    if (!dataItem->exists())
    {
      return false;
    }
  }
  return true;
}

void SampleManager::downloadAllDataItems()
{
  setDownloadFailed(false);
  setShowFullScreenDownload(true);

  if (!m_dataItems.isEmpty())
  {
    m_dataItems.clear();
  }

  // populate list of data items needed to be downloaded.
  for (auto sample : *samples())
  {
    for (auto dataItem : *sample->dataItems())
    {
      if (!dataItem->exists())
      {
        m_dataItems.enqueue(dataItem);
      }
    }
  }

  m_totalDownloadItems = m_dataItems.size();
  downloadNextDataItem();
}

void SampleManager::downloadDataItemsCurrentSample()
{
  setDownloadFailed(false);
  setShowFullScreenDownload(true);

  if (!m_dataItems.isEmpty())
  {
    m_dataItems.clear();
  }

  for (auto dataItem : *currentSample()->dataItems())
  {
    if (!dataItem->exists())
    {
      m_dataItems.enqueue(dataItem);
    }
  }

  m_totalDownloadItems = m_dataItems.size();
  downloadNextDataItem();
}

static QString homePath()
{
  QString homePath;

#ifdef Q_OS_IOS
  homePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
  homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif

  return homePath;
}

void SampleManager::downloadNextDataItem()
{
  if (cancelDownload())
  {
    m_dataItems.clear();
  }

  if (!m_dataItems.isEmpty())
  {
    if (!m_portal)
    {
      m_portal = new Portal(QUrl("https://www.arcgis.com"), this);
      connect(m_portal, &Portal::doneLoading, this, [this](const Error& error)
      {
        if (!error.isEmpty())
        {
          setDownloadFailed(true);
          setDownloadText(QString("Download failed: %1").arg(error.message()));
          setDownloadInProgress(false);
          return;
        }

        setDownloadText(QString("Remaining items in queue: %1").arg(m_dataItems.size()));
        auto item = m_dataItems.dequeue();
        fetchPortalItemData(item->itemId(), homePath() + item->path().mid(1));
      });
      m_portal->load();
    }
    else
    {
      setDownloadText(QString("Remaining items in queue: %1").arg(m_dataItems.size()));
      auto item = m_dataItems.dequeue();
      fetchPortalItemData(item->itemId(), homePath() + item->path().mid(1));
    }
  }
  else
  {
    setDownloadText("Downloads complete");
    setDownloadProgress(0.0);
    setDownloadInProgress(false);

    setShowFullScreenDownload(false);
    m_totalDownloadItems = 0;

    // Stop progress timer
    if (m_progressUpdateTimer->isActive())
    {
      m_progressUpdateTimer->stop();
    }

    // Clear progress tracking
    m_dataItemProgress.clear();
    m_activeDownloads.clear();

    if (cancelDownload())
    {
      setCancelDownload(false);
    }
    else
    {
      emit doneDownloadingChanged();
    }

    // Final update to reflect completion
    updateOfflineDataProjects();
  }
}

void SampleManager::fetchPortalItemData(const QString& itemId, const QString& outputPath)
{
  auto portalItem = new PortalItem(m_portal, itemId, this);
  if (!portalItem)
  {
    return;
  }

  // Key for tracking
  QString itemKey = QString("%1|%2").arg(itemId, outputPath);
  m_activeDownloads[itemKey] = portalItem;
  m_dataItemProgress[itemKey] = 0.0;

  connect(portalItem, &PortalItem::doneLoading, this, [this, portalItem, outputPath, itemKey]()
  {
    if (portalItem->loadStatus() == LoadStatus::Loaded)
    {
      auto folder = portalItem->type() == PortalItemType::CodeSample ? portalItem->name() : QString();
      QString formattedPath = SampleManager::formattedPath(outputPath, folder);

      portalItem->fetchDataAsync(formattedPath)
        .then(this,
              [this, portalItem, formattedPath, itemKey]()
      {
        // Download complete
        m_dataItemProgress[itemKey] = 1.0;
        m_activeDownloads.remove(itemKey);

        if (QFileInfo(formattedPath).suffix() == "zip")
        {
          auto zipHelper = new ZipHelper(formattedPath, this);
          connect(zipHelper, &ZipHelper::extractCompleted, this, [this, formattedPath, portalItem, zipHelper, itemKey]()
          {
            m_dataItemProgress.remove(itemKey);
            downloadNextDataItem();
            portalItem->deleteLater();
            zipHelper->deleteLater();
          });
          zipHelper->extractAll(QFileInfo(formattedPath).dir().absolutePath());
        }
        else
        {
          m_dataItemProgress.remove(itemKey);
          downloadNextDataItem();
          portalItem->deleteLater();
        }
      })
        .onFailed([this, portalItem, itemKey](const ErrorException&)
      {
        m_dataItemProgress.remove(itemKey);
        m_activeDownloads.remove(itemKey);
        setDownloadFailed(true);
        setDownloadText(QString("Download failed for item %1").arg(portalItem->itemId()));
        setDownloadInProgress(false);

        portalItem->deleteLater();
        downloadNextDataItem();
      });
    }
    else
    {
      // Load failed
      m_dataItemProgress.remove(itemKey);
      m_activeDownloads.remove(itemKey);
      setDownloadFailed(true);
      setDownloadText(QString("Failed to load item %1").arg(itemKey));
      setDownloadInProgress(false);
      portalItem->deleteLater();

      // Continue with next data item
      downloadNextDataItem();
    }
  });

  connect(portalItem, &PortalItem::fetchDataProgressChanged, this, [this, itemKey](const NetworkRequestProgress& progress)
  {
    if (!downloadInProgress())
    {
      setDownloadInProgress(true);
    }
    // Update progress for this specific item (0.0 to 1.0)
    m_dataItemProgress[itemKey] = progress.progressPercentage() / 100.0;
  });

  portalItem->load();
}

QString SampleManager::formattedPath(const QString& outputPath, const QString& folderName)
{
  // first make sure output path exists
  if (!QFile::exists(outputPath))
  {
    QFileInfo fileInfo = QFileInfo(outputPath);
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
    QFileInfo fileInfo = QFileInfo(outputPath);
    QString dir = fileInfo.dir().absolutePath();
    formattedFilePath = dir + "/" + folderName;
  }

  return formattedFilePath;
}

SampleManager::Reachability SampleManager::reachability() const
{
  auto* instance = QNetworkInformation::instance();

  if (!instance)
  {
    return SampleManager::Reachability::ReachabilityUnknown;
  }
  switch (instance->reachability())
  {
    case QNetworkInformation::Reachability::Online:
      return SampleManager::Reachability::ReachabilityOnline;
    case QNetworkInformation::Reachability::Site:
      return SampleManager::Reachability::ReachabilitySite;
    case QNetworkInformation::Reachability::Local:
      return SampleManager::Reachability::ReachabilityLocal;
    case QNetworkInformation::Reachability::Disconnected:
      return SampleManager::Reachability::ReachabilityDisconnected;
    case QNetworkInformation::Reachability::Unknown:
    default:
      return SampleManager::Reachability::ReachabilityUnknown;
  };
}

bool SampleManager::deleteAllOfflineData()
{
  QDir dir(homePath() + "/ArcGIS/Runtime/Data");
  bool success = dir.removeRecursively();

  // Update the offline data projects list to reflect deletion status
  if (success)
  {
    updateOfflineDataProjects();
  }

  return success;
}

QString SampleManager::api() const
{
  return QStringLiteral("C++");
}

void SampleManager::setSourceCodeIndex(int i)
{
  if (m_currentSample)
  {
    m_currentSourceCode = m_currentSample->codeFiles()->get(i)->code();
  }
  else
  {
    m_currentSourceCode = "Sample Viewer Error.";
  }
  emit currentSourceCodeChanged();
}

void SampleManager::setupProxy(const QString& hostName, quint16 port, const QString& user, const QString& pw)
{
  QNetworkProxy proxy;
  proxy.setType(QNetworkProxy::HttpProxy);
  proxy.setHostName(hostName);
  proxy.setPort(port);
  proxy.setUser(user);
  proxy.setPassword(pw);
  QNetworkProxy::setApplicationProxy(proxy);
}

void SampleManager::doneDownloading()
{
  emit doneDownloadingChanged();
}

OfflineDataProjectsModel* SampleManager::getOfflineDataProjects()
{
  updateOfflineDataProjects();
  return m_offlineDataProjects;
}

double SampleManager::calculateSampleDownloadProgress(Sample* sample) const
{
  if (!sample || sample->dataItems()->size() == 0)
  {
    return 0.0;
  }

  double totalProgress = 0.0;
  int itemCount = 0;

  for (int i = 0; i < sample->dataItems()->size(); ++i)
  {
    DataItem* dataItem = sample->dataItems()->at(i);
    if (dataItem->exists())
    {
      // Already downloaded - counts as 100%
      totalProgress += 1.0;
    }
    else
    {
      // Check if currently downloading
      QString key = QString("%1|%2").arg(dataItem->itemId(), dataItem->path());
      if (m_dataItemProgress.contains(key))
      {
        totalProgress += m_dataItemProgress[key];
      }
      // else: not started yet, contributes 0.0
    }
    itemCount++;
  }

  return itemCount > 0 ? (totalProgress / itemCount) : 0.0;
}

void SampleManager::updateOfflineDataProjects()
{
  if (!m_offlineDataProjects)
  {
    return;
  }

  // Clear the model
  m_offlineDataProjects->clear();

  const SampleListModel* sampleList = samples();
  const int totalSamples = sampleList->size();

  for (int i = 0; i < totalSamples; ++i)
  {
    Sample* sample = sampleList->at(i);
    if (sample->dataItems()->size() > 0)
    {
      int downloadedCount = 0;
      int totalCount = sample->dataItems()->size();
      bool isDownloading = false;

      // Count downloaded items and check if any are currently downloading
      for (int j = 0; j < totalCount; ++j)
      {
        DataItem* dataItem = sample->dataItems()->at(j);
        if (dataItem->exists())
        {
          downloadedCount++;
        }
        else
        {
          // Check if this item is currently downloading
          QString key = QString("%1|%2").arg(dataItem->itemId(), dataItem->path());
          if (m_dataItemProgress.contains(key) || m_activeDownloads.contains(key))
          {
            isDownloading = true;
          }
        }
      }

      bool allDownloaded = (downloadedCount == totalCount);
      double progress = calculateSampleDownloadProgress(sample);

      // Add project to model
      m_offlineDataProjects->addProject(sample, allDownloaded, isDownloading, progress, downloadedCount, totalCount);
    }
  }
}

bool SampleManager::hasOfflineData(const QString& sampleName)
{
  const SampleListModel* sampleList = samples();
  const int totalSamples = sampleList->size();

  for (int i = 0; i < totalSamples; ++i)
  {
    Sample* sample = sampleList->at(i);
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
      break;
    }
  }
  return false;
}

bool SampleManager::deleteProjectOfflineData(const QString& sampleName)
{
  bool success = true;

  const SampleListModel* sampleList = samples();
  const int totalSamples = sampleList->size();

  for (int i = 0; i < totalSamples; ++i)
  {
    Sample* sample = sampleList->at(i);
    if (sample->name().toString() == sampleName)
    {
      const int dataItemCount = sample->dataItems()->size();
      for (int j = 0; j < dataItemCount; ++j)
      {
        DataItem* dataItem = sample->dataItems()->at(j);
        if (dataItem->exists())
        {
          QString fullPath = homePath() + dataItem->path().mid(1);
          QFileInfo fileInfo(fullPath);

          if (fileInfo.isDir())
          {
            QDir dir(fullPath);
            if (!dir.removeRecursively())
            {
              success = false;
            }
          }
          else
          {
            QFile file(fullPath);
            if (!file.remove())
            {
              success = false;
            }
          }
        }
      }
      break;
    }
  }

  // Update the projects list after deletion
  updateOfflineDataProjects();

  return success;
}

void SampleManager::downloadProjectData(const QString& sampleName)
{
  setDownloadFailed(false);
  setShowFullScreenDownload(false);

  if (!m_dataItems.isEmpty())
  {
    m_dataItems.clear();
  }

  // Find the sample and mark it as downloading
  Sample* targetSample = nullptr;
  int projectIndex = -1;

  for (int i = 0; i < m_allSamples->size(); ++i)
  {
    Sample* sample = m_allSamples->at(i);
    if (sample->name().toString() == sampleName)
    {
      targetSample = sample;
      projectIndex = m_offlineDataProjects->findProjectBySample(sample);
      break;
    }
  }

  if (!targetSample)
  {
    return;
  }

  // Clear any existing progress for this sample's items
  for (int i = 0; i < targetSample->dataItems()->size(); ++i)
  {
    DataItem* dataItem = targetSample->dataItems()->at(i);
    QString key = QString("%1|%2").arg(dataItem->itemId(), dataItem->path());
    m_dataItemProgress.remove(key);
  }

  // Mark as downloading and update model
  if (projectIndex >= 0)
  {
    int downloadedCount = 0;
    int totalCount = targetSample->dataItems()->size();

    for (int i = 0; i < totalCount; ++i)
    {
      if (targetSample->dataItems()->at(i)->exists())
      {
        downloadedCount++;
      }
    }

    m_offlineDataProjects->updateProject(projectIndex, false, true, 0.0, downloadedCount, totalCount);
  }

  // Add items to download queue
  for (int i = 0; i < targetSample->dataItems()->size(); ++i)
  {
    DataItem* dataItem = targetSample->dataItems()->at(i);
    if (!dataItem->exists())
    {
      m_dataItems.enqueue(dataItem);
    }
  }

  // Start progress timer
  if (!m_progressUpdateTimer->isActive())
  {
    m_progressUpdateTimer->start();
  }

  downloadNextDataItem();
}

void SampleManager::cancelSampleDownload(const QString& sampleName)
{
  if (m_dataItems.isEmpty())
  {
    setShowFullScreenDownload(false);
    setDownloadInProgress(false);

    // Stop progress timer
    if (m_progressUpdateTimer->isActive())
    {
      m_progressUpdateTimer->stop();
    }
  }

  // Find the sample
  Sample* targetSample = nullptr;
  for (int i = 0; i < m_allSamples->size(); ++i)
  {
    Sample* sample = m_allSamples->at(i);
    if (sample->name().toString() == sampleName)
    {
      targetSample = sample;
      break;
    }
  }

  if (!targetSample)
  {
    return;
  }

  // Cancel active downloads for this sample's data items
  for (int i = 0; i < targetSample->dataItems()->size(); ++i)
  {
    DataItem* dataItem = targetSample->dataItems()->at(i);
    QString key = QString("%1|%2").arg(dataItem->itemId(), dataItem->path());

    // Remove from active downloads and clean up
    if (m_activeDownloads.contains(key))
    {
      PortalItem* portalItem = m_activeDownloads.take(key);
      if (portalItem)
      {
        portalItem->cancelLoad();
        portalItem->deleteLater();
      }
    }

    // Clear progress
    m_dataItemProgress.remove(key);
  }

  // Remove this sample's items from download queue
  QQueue<DataItem*> newQueue;
  while (!m_dataItems.isEmpty())
  {
    DataItem* item = m_dataItems.dequeue();
    bool belongsToSample = false;

    for (int i = 0; i < targetSample->dataItems()->size(); ++i)
    {
      if (targetSample->dataItems()->at(i) == item)
      {
        belongsToSample = true;
        break;
      }
    }

    if (!belongsToSample)
    {
      newQueue.enqueue(item);
    }
  }
  m_dataItems = newQueue;

  // Update the model to reflect cancellation
  updateOfflineDataProjects();
}

bool SampleManager::showFullScreenDownload() const
{
  return m_showFullScreenDownload;
}

void SampleManager::setShowFullScreenDownload(bool show)
{
  m_showFullScreenDownload = show;
  emit showFullScreenDownloadChanged();
}

void SampleManager::updateDownloadProgress()
{
  if (m_totalDownloadItems == 0)
  {
    setDownloadProgress(0.0);
    return;
  }

  // Calculate completed items
  int completedItems = m_totalDownloadItems - m_dataItems.size() - m_activeDownloads.size();

  // Add progress from currently downloading items
  double currentProgress = 0.0;
  for (const auto& progress : m_dataItemProgress)
  {
    currentProgress += progress;
  }

  // Overall progress: (completed + in-progress) / total
  double overallProgress = ((completedItems + currentProgress) / m_totalDownloadItems) * 100.0;
  setDownloadProgress(qBound(0.0, overallProgress, 100.0));
}
