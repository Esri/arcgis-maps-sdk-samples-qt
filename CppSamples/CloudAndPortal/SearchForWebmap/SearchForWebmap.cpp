// [WriteFile Name=SearchForWebmap, Category=CloudAndPortal]
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

#include "AuthenticationManager.h"
#include "Map.h"
#include "MapQuickView.h"
#include "Portal.h"
#include "PortalItem.h"
#include "PortalItemListModel.h"
#include "PortalQueryParametersForItems.h"
#include "SearchForWebmap.h"
#include "MapTypes.h"
#include "PortalTypes.h"
#include "Error.h"
#include "PortalQueryResultSetForItems.h"
#include "MapViewTypes.h"

#include <QDate>
#include <QDateTime>
#include <QFuture>

using namespace Esri::ArcGISRuntime;

SearchForWebmap::SearchForWebmap(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_portal(new Portal(this))
{
  AuthenticationManager::instance()->setCredentialCacheEnabled(false);
}

SearchForWebmap::~SearchForWebmap() = default;

void SearchForWebmap::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<SearchForWebmap>("Esri.Samples", 1, 0, "SearchForWebmapSample");
}

void SearchForWebmap::componentComplete()
{
  QQuickItem::componentComplete();

  if (m_portal)
  {
    connect(m_portal, &Portal::loadStatusChanged, this, [this]()
    {
      m_portalLoaded = m_portal->loadStatus() == LoadStatus::Loaded;
      emit portalLoadedChanged();
    });

    m_portal->load();
  }

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  if(m_mapView)
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

void SearchForWebmap::search(const QString& keyword)
{
  if (!m_portal)
    return;

  //! [SearchForWebmap CPP Portal find items async]
  // webmaps authored prior to July 2nd, 2014 are not supported
  // so search only from that date to the current time (in milliseconds)
  QString fromDate = QString("000000%1").arg(QDateTime::fromString(QDate(2014, 7, 2).toString()).toMSecsSinceEpoch());
  QString toDate = QString("000000%1").arg(QDateTime::currentDateTime().toMSecsSinceEpoch());

  PortalQueryParametersForItems query;
  query.setSearchString(QString("tags:\"%1\" AND +uploaded:[%2 TO %3]")
                        .arg(keyword, fromDate, toDate));
  query.setTypes(QList<PortalItemType>() << PortalItemType::WebMap);

  m_portal->findItemsAsync(query).then(
  [this](PortalQueryResultSetForItems* webmapResults)
  {
    onFindItemsCompleted(webmapResults);
  });
  //! [SearchForWebmap CPP Portal find items async]

  if(m_mapView)
    m_mapView->setVisible(false);
}

void SearchForWebmap::searchNext()
{
  if (!m_webmapResults || !m_portal)
    return;

  //! [Portal find with nextQueryParameters]
  PortalQueryParametersForItems nextQuery = m_webmapResults->nextQueryParameters();
  // check whether the startIndex of the new query is valid
  if (!nextQuery.isEmpty())
  {
    m_portal->findItemsAsync(nextQuery).then(
    [this](PortalQueryResultSetForItems* webmapResults)
    {
      onFindItemsCompleted(webmapResults);
    });
  }
  //! [Portal find with nextQueryParameters]
}

void SearchForWebmap::loadSelectedWebmap(int index)
{
  if (!m_webmaps)
     return;

   if (m_map)
     delete m_map;

   // create map from portal item
   m_map = new Map(m_webmaps->at(index), this);

   connect(m_map,&Map::errorOccurred, this, [this]()
   {
     m_mapLoadeError = m_map->loadError().message();
     emit mapLoadErrorChanged();
   });

   // set map on mapview
   m_mapView->setMap(m_map);
   m_mapView->setVisible(true);
}

void SearchForWebmap::errorAccepted()
{
  m_mapLoadeError.clear();
  emit mapLoadErrorChanged();
}

void SearchForWebmap::onFindItemsCompleted(PortalQueryResultSetForItems* webmapResults)
{
  m_webmapResults = webmapResults;
  m_webmaps = m_webmapResults->itemResults();
  emit webmapsChanged();
  emit hasMoreResultsChanged();
}
