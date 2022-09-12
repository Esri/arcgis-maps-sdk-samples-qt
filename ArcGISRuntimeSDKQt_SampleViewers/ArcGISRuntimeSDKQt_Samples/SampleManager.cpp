// [Legal]
// Copyright 2022 Esri.

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
#include <QStringList>
#include <QTemporaryDir>
#include <QUrl>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>
#include <QQmlEngine>

#include "CategoryListModel.h"
#include "DataItem.h"
#include "DataItemListModel.h"
#include "PermissionsHelper.h"
#include "SampleCategory.h"
#include "Sample.h"
#include "SampleListModel.h"
#include "SampleManager.h"
#include "SampleManager_definitions.h"
#include "SampleSearchFilterModel.h"
#include "SourceCode.h"
#include "SourceCodeListModel.h"

#include <cstdlib>

#ifdef CPP_VIEWER
#include "ArcGISRuntimeEnvironment.h"
using namespace Esri::ArcGISRuntime;
#endif

#ifdef LOCALSERVER_SUPPORTED
#include "LocalServer.h"
#endif

#define STRINGIZE(x) #x
#define QUOTE(x) STRINGIZE(x)

namespace
{
QString apiKey = ""; // Provide your API key here
}

SampleManager::SampleManager(QObject *parent):
  QObject(parent),
  m_categories(new CategoryListModel(this)),
  m_allSamples(new SampleListModel(this))
{
#ifdef LOCALSERVER_SUPPORTED
  createAndSetTempDirForLocalServer();
#endif
}

SampleManager::~SampleManager() = default;

void SampleManager::init()
{
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
    localServerTempDir = tmpPath + "/EsriQtTemp";
  else
    localServerTempDir = homePath + "/EsriQtTemp";

  // create the directory
  m_tempDir = std::make_unique<QTemporaryDir>(localServerTempDir);

  // set the temp & app data path for the local server
  LocalServer::setTempDataPath(m_tempDir->path());
  LocalServer::setAppDataPath(m_tempDir->path());
#endif
}

// Build the Categories List
void SampleManager::buildCategoriesList()
{
  QDir dir(DIRNAMESAMPLES);
  QFile xmlFile(":/Samples/Categories.xml");

  // Go through the XML to find the sample categories
  if (xmlFile.exists())
  {
    xmlFile.open(QIODevice::ReadOnly);
    QDomDocument doc;
    doc.setContent(&xmlFile);
    QDomNodeList fileList = doc.documentElement().elementsByTagName("category");

    for (int i = 0; i < fileList.count(); i++)
    {
      // The first node contains the name
      QDomNode node = fileList.at(i).childNodes().at(0);
      QString name = node.toElement().text();
      // The second node contains the display name
      QDomNode nameNode = fileList.at(i).childNodes().at(1);
      QString displayName = nameNode.toElement().text();
      appendCategoryToManager(createCategory(name, displayName, dir));
    }
  }
  else
  {
    qWarning() << "could not find Categories.xml";
  }
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
    return nullptr;
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
      m_allSamples->addSample(category->samples()->get(i));
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
    // The ordering of the snippets is specific to the sample viewer
#ifdef CPP_VIEWER
    // C++ sample snippets will list the primary C++ file name first (often SampleName.cpp) so it initially displays upon loading the sample in the ArcGIS Developer website
    // C++ sample snippets will list the QML source file name *last* to ensure consistent retrieval (rather than by a specific index)
    auto sourceFileName = sampleConfig.value(PROPERTYSOURCE).isValid() ? sampleConfig.value(PROPERTYSOURCE).toStringList().last() : "sample.qml";
#else
    // QML sample snippets will list the QML source file *first* so they display upon loading the sample in the ArcGIS Developer website
    auto sourceFileName = sampleConfig.value(PROPERTYSOURCE).isValid() ? sampleConfig.value(PROPERTYSOURCE).toStringList().first() : "sample.qml";
#endif

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
  return scheme.isEmpty()
      ? QUrl::fromLocalFile(path)
      : scheme + path;
}

void SampleManager::setCurrentMode(const CurrentMode& mode)
{
  m_currentMode = mode;
  emit currentModeChanged();
}

void SampleManager::setCurrentSample(Sample* sample)
{
  m_currentSample = sample;

  // NOTE - currently we know we cannot set an API Key for the
  // following samples since they use named user login instead.

  const QString sampleName = m_currentSample->name().toString();
  if (sampleName == "Create and save map" ||
      sampleName == "Add items to portal" ||
      sampleName == "Search for web map by keyword")
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
    qWarning() << "This sample expects an API key to be set, but none was provided. Please provide an API key in ArcGISRuntimeSDKQt_Samples/SampleManager.cpp";
  }
  const QString sampleApiKey = isSupportsApiKey ? apiKey : ""; // empty string will "unset" the key
  // set apikey for CPP/QML sample viewer
#ifdef CPP_VIEWER
  ArcGISRuntimeEnvironment::setApiKey(sampleApiKey);
#else
  emit apiKeyRequired(apiKey);
#endif
}

void SampleManager::setDownloadProgress(double progress)
{
  m_downloadProgress = progress;
  emit downloadProgressChanged();
}

void SampleManager::setSourceCodeIndex(int i)
{
  if (m_currentSample)
    m_currentSourceCode = m_currentSample->codeFiles()->get(i)->code();
  else
    m_currentSourceCode = "Sample Viewer Error.";
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
