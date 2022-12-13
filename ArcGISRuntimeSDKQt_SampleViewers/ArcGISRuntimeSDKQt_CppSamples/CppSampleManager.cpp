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

#include "CppSampleManager.h"

#include "ArcGISQt_global.h" // for LOCALSERVER_SUPPORTED
#include "AuthenticationManager.h"
#include "Portal.h"
#include "PortalItem.h"
#include "SampleManager_definitions.h"
#include "MapTypes.h"
#include "Error.h"
#include "CredentialCache.h"
#include "PortalTypes.h"
#include "NetworkRequestProgress.h"

using namespace Esri::ArcGISRuntime;

#ifdef LOCALSERVER_SUPPORTED
#include "LocalServer.h"
#endif // LOCALSERVER_SUPPORTED

#include <QQmlEngine>

CppSampleManager::CppSampleManager(QObject* parent):
  DownloadSampleManager(parent),
  m_portal(new Portal(QUrl("https://arcgis.com"), this))
{
  connect(m_portal, &Portal::doneLoading, this, [this](Error error)
  {
    emit portalDoneLoading(error.isEmpty());
  });
}

CppSampleManager::~CppSampleManager() = default;

void CppSampleManager::init()
{
  // Register the AuthenticationManager for QML
  qmlRegisterUncreatableType<AuthenticationManager>(
        "Esri.ArcGISRuntimeSamples", 1, 0,
        "AuthenticationManager", "AuthenticationManager is uncreateable");

  DownloadSampleManager::init();
}

void CppSampleManager::clearCredentialCache()
{
  AuthenticationManager::credentialCache()->removeAndRevokeAllCredentials();
}

void CppSampleManager::buildCategoriesList()
{
  DownloadSampleManager::buildCategoriesList();
  // If Local Server is supported and installed, add the category
#ifdef LOCALSERVER_SUPPORTED
  if (LocalServer::isInstallValid())
  {
    const QDir dir(DIRNAMESAMPLES);
    appendCategoryToManager(createCategory("LocalServer", "Local Server", dir));
  }
#endif // LOCALSERVER_SUPPORTED
}

AuthenticationManager* CppSampleManager::authenticationManager() const
{
  return AuthenticationManager::instance();
}

void CppSampleManager::loadPortal()
{
  m_portal->load();
}

bool CppSampleManager::isPortalLoaded() const
{
  return m_portal->loadStatus() == LoadStatus::Loaded;
}

void CppSampleManager::createPortalItem(const QString& itemId)
{
  m_portalItem = new PortalItem(m_portal, itemId, this);

  connect(m_portalItem, &PortalItem::doneLoading, this,
          [this](Error error)
  {
    bool success = error.isEmpty();

    // Copy important parts before potential delete.
    auto type = m_portalItem->type();
    auto name = m_portalItem->name();
    auto id = m_portalItem->itemId();

    if (!success)
    {
      delete m_portalItem;
      m_portalItem = nullptr;
    }

    emit portalItemDoneLoading(
          success,
          id,
          type == PortalItemType::CodeSample ? name : QString());

  });

  connect(m_portalItem, &PortalItem::fetchDataProgressChanged, this,
          [this](const NetworkRequestProgress& progress)
  {
    emit portalItemFetchDataProgress(m_portalItem->itemId(), progress.progressPercentage());
  });

  connect(m_portalItem, &PortalItem::fetchDataCompleted,
          this, [this](bool success)
  {
    auto id = m_portalItem->itemId();
    delete m_portalItem;
    m_portalItem = nullptr;
    emit portalItemFetchDataCompleted(id, success);
  });

  m_portalItem->load();
}

void CppSampleManager::fetchData(const QString& outputPath)
{
  m_portalItem->fetchData(outputPath);
}

QString CppSampleManager::api() const
{
  return QStringLiteral("C++");
}
