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

using namespace Esri::ArcGISRuntime;

IntegratedWindowsAuthentication::IntegratedWindowsAuthentication(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::imagery(this), this))
{
  AuthenticationManager::instance()->setCredentialCacheEnabled(false);
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

void IntegratedWindowsAuthentication::searchPortal(QString url)
{
  m_portal = new Portal(url, this);
  connect(m_portal, &Portal::doneLoading, this, [this](Esri::ArcGISRuntime::Error loadError)
  {
    if(!loadError.isEmpty())
    {
      qDebug() << loadError.message();
    }
    PortalQueryParametersForItems query;
    query.setTypes(QList<PortalItemType>() << PortalItemType::WebMap);
    m_portal->findItems(query);

  });

  connect(m_portal, &Portal::findItemsCompleted, this, [this](PortalQueryResultSetForItems* results)
  {
    PortalItemListModel* temp = results->itemResults();
    m_webmapList.clear();
    for(PortalItem * pI : *temp) {
      m_webmapList.append(pI->title());
    }
    emit webmapListModelChanged();
  });

  connect( m_portal, &Portal::loadStatusChanged, this, [this]()
  {
    if(m_portal->loadStatus() == LoadStatus::FailedToLoad)
      qDebug() << "failed to load";
  });
  m_portal->load();
}

AuthenticationManager *IntegratedWindowsAuthentication::authManager() const
{
  return AuthenticationManager::instance();
}
