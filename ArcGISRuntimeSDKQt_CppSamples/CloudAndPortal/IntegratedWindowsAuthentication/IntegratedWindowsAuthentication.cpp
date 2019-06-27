// [WriteFile Name=IntegratedWindowsAuthentication, Category=CloudAndPortal]
// [Legal]
// Copyright 2019 Esri.

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

#include "IntegratedWindowsAuthentication.h"

#include "Map.h"
#include "MapQuickView.h"
#include "AuthenticationManager.h"
#include "Portal.h"
#include "PortalItem.h"
#include "PortalQueryResultSetForItems.h"
#include "PortalItemListModel.h"

using namespace Esri::ArcGISRuntime;

IntegratedWindowsAuthentication::IntegratedWindowsAuthentication(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::imagery(this), this))
{
}

IntegratedWindowsAuthentication::~IntegratedWindowsAuthentication() = default;

void IntegratedWindowsAuthentication::init()
{
  // Register the map view for QML
  qmlRegisterUncreatableType<AuthenticationManager>("Esri.Samples", 1, 0, "AuthenticationManager", "AuthenticationManager is uncreateable");
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<IntegratedWindowsAuthentication>("Esri.Samples", 1, 0, "IntegratedWindowsAuthenticationSample");
}

MapQuickView* IntegratedWindowsAuthentication::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void IntegratedWindowsAuthentication::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}

void IntegratedWindowsAuthentication::searchPortal(QString url, bool forceLogin)
{
  m_loadingIndicator = true;
  emit isLoadingChanged();

  if (forceLogin)
  {
    m_portal = new Portal(url, forceLogin, this);
  }
  else
  {
    m_portal = new Portal(url, this);
  }

  connect(m_portal, &Portal::doneLoading, this, [this](Esri::ArcGISRuntime::Error loadError)
  {
    if (!loadError.isEmpty())
    {
      m_mapLoadError = loadError.message();
      m_loadingIndicator = false;
      emit isLoadingChanged();
      emit mapLoadErrorChanged();
      return;
    }

    PortalQueryParametersForItems query;
    query.setTypes(QList<PortalItemType>() << PortalItemType::WebMap);
    m_portal->findItems(query);
  });

  connect(m_portal, &Portal::findItemsCompleted, this, [this](PortalQueryResultSetForItems* results)
  {
    m_webmaps = results->itemResults();
    m_webmapList.clear();
    for (PortalItem * pI : *m_webmaps)
    {
      m_webmapList.append(pI->title());
    }

    emit webmapListModelChanged();
    m_loadingIndicator = false;
    emit isLoadingChanged();
  });

  m_portal->load();
}

void IntegratedWindowsAuthentication::loadSelectedWebmap(int index)
{
  if (!m_webmaps)
    return;

  PortalItem* selectedItem = m_webmaps->at(index);
  if (!selectedItem)
    return;

  if (m_selectedItem)
    m_selectedItem->disconnect();

  m_selectedItem = selectedItem;
  m_loadingIndicator = true;
  emit isLoadingChanged();

  connect(m_selectedItem, &PortalItem::doneLoading, this, [this]
  {
    if (!m_selectedItem || m_selectedItem->loadStatus() != LoadStatus::Loaded)
      return;

    if (m_map)
    {
      delete m_map;
      m_map = nullptr;
    }

    m_mapLoadError.clear();
    emit mapLoadErrorChanged();

    m_map = new Map(m_selectedItem, this);

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
      m_loadingIndicator = false;
      emit isLoadingChanged();
    });

    m_map->load();
  });

  m_selectedItem->load();
}

void IntegratedWindowsAuthentication::errorAccepted()
{
  m_mapLoadError.clear();
  emit mapLoadErrorChanged();
}

AuthenticationManager *IntegratedWindowsAuthentication::authManager() const
{
  return AuthenticationManager::instance();
}
