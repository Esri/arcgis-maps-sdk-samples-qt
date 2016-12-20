// [WriteFile Name=ShowOrgBasemaps, Category=CloudAndPortal]
// [Legal]
// Copyright 2016 Esri.

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

#include "AuthenticationManager.h"
#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "BasemapListModel.h"
#include "Portal.h"
#include "ShowOrgBasemaps.h"

using namespace Esri::ArcGISRuntime;

ShowOrgBasemaps::ShowOrgBasemaps(QQuickItem* parent /* = nullptr */):
    QQuickItem(parent),
    m_map(nullptr),
    m_mapView(nullptr),
    m_portal(new Portal(this))
{
    AuthenticationManager::instance()->setCredentialCacheEnabled(false);
}

ShowOrgBasemaps::~ShowOrgBasemaps()
{
}

void ShowOrgBasemaps::componentComplete()
{
    QQuickItem::componentComplete();

    connect(m_portal, &Portal::loadStatusChanged, this, [this](){
        m_portalLoaded = m_portal->loadStatus() == LoadStatus::Loaded;

        emit portalLoadedChanged();
        emit orgNameChanged();

        if (m_portalLoaded)
            m_portal->fetchBasemaps();
    });

    connect(m_portal, &Portal::basemapsChanged, this, [this](){
        emit basemapsChanged();
    });

    // find QML MapView component
    m_mapView = findChild<MapQuickView*>("mapView");
    m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

    emit authManagerChanged();
}

bool ShowOrgBasemaps::portalLoaded() const
{
    return m_portalLoaded;
}

QString ShowOrgBasemaps::orgName() const
{
    if (!portalLoaded() || !m_portal->portalInfo())
        return "";

    return m_portal->portalInfo()->organizationName();
}

QAbstractListModel* ShowOrgBasemaps::basemaps() const
{
    return m_portal->basemaps();
}

QString ShowOrgBasemaps::mapLoadError() const
{
    return m_mapLoadError;
}

void ShowOrgBasemaps::load(bool anonymous)
{
    if (anonymous)
        m_portal->load();
    else {
        Credential* cred = new Credential(OAuthClientInfo("W3hPKzPbeJ0tr8aj", OAuthMode::User), this);
        m_portal->setCredential(cred);
        m_portal->load();
    }
}

void ShowOrgBasemaps::loadSelectedBasemap(int index)
{
    if (!m_portal->basemaps())
        return;

    Basemap* selectedBasemap = m_portal->basemaps()->at(index);
    if (!selectedBasemap)
        return;

    if (m_map)
        m_map->disconnect();

    m_mapLoadError.clear();
    emit mapLoadErrorChanged();

    m_map = new Map(selectedBasemap, this);

    connect(m_map, &Map::errorOccurred, this, [this](){
        m_mapLoadError = m_map->loadError().message();
        emit mapLoadErrorChanged();
    });

    connect(m_map, &Map::loadStatusChanged, this, [this](){
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

AuthenticationManager *ShowOrgBasemaps::authManager() const
{
    return AuthenticationManager::instance();
}
