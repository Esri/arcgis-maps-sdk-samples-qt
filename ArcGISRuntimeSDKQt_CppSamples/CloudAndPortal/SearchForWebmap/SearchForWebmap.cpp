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

#include <QDate>

using namespace Esri::ArcGISRuntime;

SearchForWebmap::SearchForWebmap(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
  AuthenticationManager::instance()->setCredentialCacheEnabled(false);
}

SearchForWebmap::~SearchForWebmap()
{
}

void SearchForWebmap::init()
{
  qmlRegisterUncreatableType<AuthenticationManager>("Esri.Samples", 1, 0, "AuthenticationManager", "AuthenticationManager is uncreateable");
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<SearchForWebmap>("Esri.Samples", 1, 0, "SearchForWebmapSample");
}

void SearchForWebmap::componentComplete()
{
  QQuickItem::componentComplete();
  emit authManagerChanged();

  connect(m_portal, &Portal::loadStatusChanged, this, [this](){
    m_portalLoaded = m_portal->loadStatus() == LoadStatus::Loaded;
    emit portalLoadedChanged();
  });

  //! [SearchForWebmap CPP Portal find items completed]
  connect(m_portal, &Portal::findItemsCompleted, this, [this](PortalQueryResultSetForItems *webmapResults){
    m_webmapResults = webmapResults;
    m_webmaps = m_webmapResults->itemResults();
    emit webmapsChanged();
    emit hasMoreResultsChanged();
  });
  //! [SearchForWebmap CPP Portal find items completed]

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
  //! [SearchForWebmap CPP Portal find items]
  // webmaps authored prior to July 2nd, 2014 are not supported - so search only from that date to the current time
  QString fromDate = QString("000000%1").arg(QDateTime(QDate(2014, 7, 2)).toTime_t());
  QString toDate = QString("000000%1").arg(QDateTime::currentDateTime().toTime_t());

  PortalQueryParametersForItems query;
  query.setSearchString(QString("tags:\"%1\" AND +uploaded:[%2 TO %3]")
                        .arg(keyword)
                        .arg(fromDate)
                        .arg(toDate));
  query.setTypes(QList<PortalItemType>() << PortalItemType::WebMap);

  m_portal->findItems(query);
  //! [SearchForWebmap CPP Portal find items]

  m_mapView->setVisible(false);
}

void SearchForWebmap::searchNext()
{
  if (!m_webmapResults)
    return;

  //! [Portal find with nextQueryParameters]
  PortalQueryParametersForItems nextQuery = m_webmapResults->nextQueryParameters();
  // check whether the startIndex of the new query is valid
  if (nextQuery.startIndex() != -1)
    m_portal->findItems(nextQuery);
  //! [Portal find with nextQueryParameters]
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
