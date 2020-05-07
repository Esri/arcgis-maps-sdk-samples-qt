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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

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
  m_map(new Map(Basemap::topographic(this), this)),
  query(new PortalQueryParametersForItems())
{
  query->setTypes(QList<PortalItemType>() << PortalItemType::WebMap);
}

IntegratedWindowsAuthentication::~IntegratedWindowsAuthentication() = default;

void IntegratedWindowsAuthentication::init()
{
  // Register C++ classes as QML types
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

void IntegratedWindowsAuthentication::searchPublicPortal()
{
  if (!m_publicPortal)
  {
    m_publicPortal = new Portal(arcgis_url, this);
  }
  else
  {
    m_loadingIndicator = true;
    emit isLoadingChanged();
    m_publicPortal->findItems(*query);
    return;
  }

  connect(m_publicPortal, &Portal::doneLoading, this, &IntegratedWindowsAuthentication::publicPortalDoneLoading);
  connect(m_publicPortal, &Portal::findItemsCompleted, this, &IntegratedWindowsAuthentication::searchItemsCompleted);

  m_loadingIndicator = true;
  emit isLoadingChanged();

  m_publicPortal->load();
}

void IntegratedWindowsAuthentication::searchIwaSecurePortal(const QString& url)
{
  if (!m_iwaSecurePortal)
  {
    m_iwaSecurePortal = new Portal(url, this);
  }
  else if (m_iwaSecurePortal && (m_iwaSecurePortal->loadStatus() == LoadStatus::FailedToLoad))
  {
    m_iwaSecurePortal = nullptr;
    m_iwaSecurePortal = new Portal(url, this);
  }
  else if (m_iwaSecurePortal && (m_iwaSecurePortal->loadStatus() == LoadStatus::Loaded))
  {
    if (m_iwaSecurePortal->url() != url)
    {
      m_iwaSecurePortal = nullptr;
      m_iwaSecurePortal = new Portal(url, this);
    }
    else
    {
      m_loadingIndicator = true;
      emit isLoadingChanged();
      m_iwaSecurePortal->findItems(*query);
      return;
    }
  }

  connect(m_iwaSecurePortal, &Portal::doneLoading, this, &IntegratedWindowsAuthentication::securePortalDoneLoading);
  connect(m_iwaSecurePortal, &Portal::findItemsCompleted, this, &IntegratedWindowsAuthentication::searchItemsCompleted);

  m_loadingIndicator = true;
  emit isLoadingChanged();

  m_iwaSecurePortal->load();
}

void IntegratedWindowsAuthentication::loadSelectedWebmap(int index)
{
  if (!m_webmaps)
    return;

  if (m_map)
    delete m_map;

  m_map = new Map(m_webmaps->at(index), this);

  m_mapView->setMap(m_map);
}

void IntegratedWindowsAuthentication::errorAccepted()
{
  m_mapLoadError.clear();
  emit mapLoadErrorChanged();
}

QAbstractListModel* IntegratedWindowsAuthentication::webmapListModel() const
{
  return m_webmaps;
}

AuthenticationManager* IntegratedWindowsAuthentication::authManager() const
{
  return AuthenticationManager::instance();
}

void IntegratedWindowsAuthentication::securePortalDoneLoading(const Error& loadError)
{
  if (!loadError.isEmpty())
  {
    m_mapLoadError = loadError.message();
    m_loadingIndicator = false;

    if(m_webmaps)
      m_webmaps = nullptr;

    emit webmapListModelChanged();
    emit isLoadingChanged();
    emit mapLoadErrorChanged();
    return;
  }

  m_iwaSecurePortal->findItems(*query);
}

void IntegratedWindowsAuthentication::publicPortalDoneLoading(const Error& loadError)
{
  if (!loadError.isEmpty())
  {
    m_mapLoadError = loadError.message();
    m_loadingIndicator = false;

    if(m_webmaps)
      m_webmaps = nullptr;

    emit webmapListModelChanged();
    emit isLoadingChanged();
    emit mapLoadErrorChanged();
    return;
  }

  m_publicPortal->findItems(*query);
}

void IntegratedWindowsAuthentication::searchItemsCompleted(const PortalQueryResultSetForItems* result)
{
  if(!result)
    return;

  if(m_webmaps)
    m_webmaps = nullptr;

  m_webmaps = result->itemResults();

  emit webmapListModelChanged();
  m_loadingIndicator = false;
  emit isLoadingChanged();
}
