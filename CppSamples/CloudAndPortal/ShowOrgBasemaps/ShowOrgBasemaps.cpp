// [WriteFile Name=ShowOrgBasemaps, Category=CloudAndPortal]
// [Legal]
// Copyright 2016 Esri.
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "ShowOrgBasemaps.h"

// ArcGIS Maps SDK headers
#include "Authentication/OAuthUserConfiguration.h"
#include "Basemap.h"
#include "BasemapListModel.h"
#include "Error.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Portal.h"
#include "PortalInfo.h"

// Qt headers
#include <QFuture>

// Other headers
#include "OAuthUserConfigurationManager.h"

using namespace Esri::ArcGISRuntime;
using namespace Esri::ArcGISRuntime::Authentication;
using namespace Esri::ArcGISRuntime::Toolkit;

ShowOrgBasemaps::ShowOrgBasemaps(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

ShowOrgBasemaps::~ShowOrgBasemaps() = default;

void ShowOrgBasemaps::init()
{
  qmlRegisterUncreatableType<QAbstractListModel>("Esri.Samples", 1, 0, "AbstractListModel", "AbstractListModel is uncreateable");
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ShowOrgBasemaps>("Esri.Samples", 1, 0, "ShowOrgBasemapsSample");
}

void ShowOrgBasemaps::connectLoadStatusSignal()
{
  if (m_portal)
  {
    connect(m_portal, &Portal::loadStatusChanged, this, [this]()
    {
      m_portalLoaded = m_portal->loadStatus() == LoadStatus::Loaded;
      m_portalLoading = m_portal->loadStatus() == LoadStatus::Loading;

      emit portalLoadedChanged();
      emit portalLoadingChanged();
      emit orgNameChanged();

      if (m_portalLoaded)
      {
        m_portal->fetchBasemapsAsync().then(
        [this]()
        {
          emit basemapsChanged();
        });
      }
    });
  }

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
}

bool ShowOrgBasemaps::portalLoaded() const
{
  return m_portalLoaded;
}

bool ShowOrgBasemaps::portalLoading() const
{
  return m_portalLoading;
}

QString ShowOrgBasemaps::orgName() const
{
  if (!m_portalLoaded || !m_portal || !m_portal->portalInfo())
    return QString();

  return m_portal->portalInfo()->organizationName();
}

QAbstractListModel* ShowOrgBasemaps::basemaps() const
{
  return m_portal ? m_portal->basemaps() : nullptr;
}

QString ShowOrgBasemaps::mapLoadError() const
{
  return m_mapLoadError;
}

void ShowOrgBasemaps::load(bool anonymous)
{
  if (m_portal)
  {
    delete m_portal;
    m_portal = nullptr;
  }

  const bool loginRequired = !anonymous;
  m_portal = new Portal(loginRequired, this);
  connectLoadStatusSignal();

  if (loginRequired)
  {
    const QString redirectUrl{"urn:ietf:wg:oauth:2.0:oob"};
    OAuthUserConfiguration* config = new OAuthUserConfiguration(m_portal->url(), "iLkGIj0nX8A4EJda", redirectUrl, this);
    OAuthUserConfigurationManager::addConfiguration(config);
  }
  else
  {
    OAuthUserConfigurationManager::clearConfigurations();
  }

  load();
}

void ShowOrgBasemaps::load()
{
  if (!m_portal)
    return;

  if (m_portal->loadStatus() == LoadStatus::FailedToLoad)
    m_portal->retryLoad();
  else
    m_portal->load();
}

void ShowOrgBasemaps::loadSelectedBasemap(int index)
{
  if (!m_portal || !m_portal->basemaps())
    return;

  Basemap* selectedBasemap = m_portal->basemaps()->at(index);
  if (!selectedBasemap)
    return;

  if (m_map)
  {
    delete m_map;
    m_map = nullptr;
  }

  m_mapLoadError.clear();
  emit mapLoadErrorChanged();

  m_map = new Map(selectedBasemap->item(), this);

  connect(m_map, &Map::errorOccurred, this, [this]()
  {
    m_mapLoadError = m_map->loadError().message();
    emit mapLoadErrorChanged();
  });

  connect(m_map, &Map::loadStatusChanged, this, [this]()
  {
    if (!m_map || m_map->loadStatus() != LoadStatus::Loaded)
      return;

    m_mapView->setMap(m_map);
    m_mapView->setVisible(true);
  });

  m_map->load();
}

void ShowOrgBasemaps::errorAccepted()
{
  m_mapLoadError.clear();
  emit mapLoadErrorChanged();
}
