// [WriteFile Name=SearchForWebmap, Category=CloudAndPortal]
// [Legal]
// Copyright 2016 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#include "AuthenticationManager.h"
#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "Portal.h"
#include "PortalItem.h"
#include "PortalItemListModel.h"
#include "PortalQueryParametersForItems.h"
#include "SearchForWebmap.h"

using namespace Esri::ArcGISRuntime;

SearchForWebmap::SearchForWebmap(QQuickItem* parent /* = nullptr */):
    QQuickItem(parent),
    m_map(nullptr),
    m_mapView(nullptr),
    m_portal(new Portal(new Credential(OAuthClientInfo("W3hPKzPbeJ0tr8aj", OAuthMode::User), this), this)),
    m_webmapResults(nullptr),
    m_webmaps(nullptr),
    m_selectedItem(nullptr),
    m_portalLoaded(false)
{
    AuthenticationManager::instance()->setCredentialCacheEnabled(false);
}

SearchForWebmap::~SearchForWebmap()
{
}

void SearchForWebmap::componentComplete()
{
    QQuickItem::componentComplete();
    emit authManagerChanged();

    connect(m_portal, &Portal::loadStatusChanged, this, [this](){
        m_portalLoaded = m_portal->loadStatus() == LoadStatus::Loaded;
        emit portalLoadedChanged();
    });

    connect(m_portal, &Portal::findItemsCompleted, this, [this](PortalQueryResultSetForItems *webmapResults){
        m_webmapResults = webmapResults;
        m_webmaps = m_webmapResults->itemResults();
        emit webmapsChanged();
        emit hasMoreResultsChanged();
    });

    m_portal->load();

    // find QML MapView component
    m_mapView = findChild<MapQuickView*>("mapView");
    m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);
}

bool SearchForWebmap::portalLoaded() const
{
    return m_portalLoaded;
}

QAbstractListModel* SearchForWebmap::webmaps() const
{
    return m_webmaps;
}

bool SearchForWebmap::hasMoreResults() const
{
    return m_webmapResults && m_webmapResults->nextQueryParameters().startIndex() > -1;
}

QString SearchForWebmap::mapLoadError() const
{
    return m_mapLoadeError;
}

void SearchForWebmap::search(const QString keyword)
{
    PortalQueryParametersForItems query;
    query.setSearchString(QString("tags:\"%1\" AND +type:\"Web Map\" AND -type:\"Web Mapping Application\"").arg(keyword));
    m_portal->findItems(query);

    m_mapView->setVisible(false);
}

void SearchForWebmap::searchNext()
{
    if (!m_webmapResults || m_webmapResults->nextQueryParameters().startIndex() == -1)
        return;

    m_portal->findItems(m_webmapResults->nextQueryParameters());
}

void SearchForWebmap::loadSelectedWebmap(int index)
{
    if (!m_webmaps)
        return;

    PortalItem* selectedItem = m_webmaps->at(index);
    if (!selectedItem)
        return;

    if (m_selectedItem)
        m_selectedItem->disconnect();

    m_selectedItem = selectedItem;

    connect(m_selectedItem, &PortalItem::loadStatusChanged, this, [this]{
        if (!m_selectedItem || m_selectedItem->loadStatus() != LoadStatus::Loaded)
            return;

        if (m_map)
            m_map->disconnect();

        m_mapLoadeError.clear();
        emit mapLoadErrorChanged();

        m_map = new Map(m_selectedItem, this);

        connect(m_map, &Map::errorOccurred, this, [this](){
            m_mapLoadeError = m_map->loadError().message();
            emit mapLoadErrorChanged();
        });

        connect(m_map, &Map::loadStatusChanged, this, [this](){
            if (!m_map || m_map->loadStatus() != LoadStatus::Loaded)
                return;

            m_mapView->setMap(m_map);
            m_mapView->setVisible(true);
        });

        m_map->load();
    });
    m_selectedItem->load();
}

void SearchForWebmap::errorAccepted()
{
    m_mapLoadeError.clear();
    emit mapLoadErrorChanged();
}

AuthenticationManager *SearchForWebmap::authManager() const
{
    return AuthenticationManager::instance();
}
