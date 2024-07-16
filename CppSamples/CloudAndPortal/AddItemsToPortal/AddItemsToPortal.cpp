// [WriteFile Name=AddItemsToPortal, Category=CloudAndPortal]
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
#include "Portal.h"
#include "PortalItem.h"
#include "PortalItemListModel.h"
#include "AddItemsToPortal.h"
#include "MapTypes.h"
#include "PortalTypes.h"
#include "CoreTypes.h"
#include "Credential.h"
#include "OAuthClientInfo.h"
#include "PortalUser.h"
#include "Error.h"
#include "ErrorException.h"
#include "ErrorInformationKeys.h"

#include <QFuture>
#include <QVariantMap>

using namespace Esri::ArcGISRuntime;

AddItemsToPortal::AddItemsToPortal(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_portal(new Portal(new Credential(OAuthClientInfo("iLkGIj0nX8A4EJda", OAuthMode::User), this), this))
{
  AuthenticationManager::instance()->setCredentialCacheEnabled(false);
  m_item = new PortalItem(m_portal, PortalItemType::CSV, this);
  m_item->setTitle("Add Items Sample");
}

AddItemsToPortal::~AddItemsToPortal() = default;

void AddItemsToPortal::init()
{
  // Register the types for QML
  qmlRegisterType<AddItemsToPortal>("Esri.Samples", 1, 0, "AddItemsToPortalSample");
}

void AddItemsToPortal::componentComplete()
{
  QQuickItem::componentComplete();

  if (m_portal)
  {
    connect(m_portal, &Portal::loadStatusChanged, this, [this]()
    {
      emit portalLoadedChanged();

      if (m_portal->loadStatus() != LoadStatus::Loaded)
        return;

      m_user = m_portal->portalUser();
      m_busy = false;
      connectUserSignals();
    });
  }

  if (m_item)
  {
    connect(m_item, &PortalItem::loadStatusChanged, this, [this]()
    {
      if (m_item->loadStatus() != LoadStatus::Loaded)
        return;

      m_busy = false;
      emit portalItemIdChanged();
      emit portalItemTitleChanged();
      emit portalItemLoadedChanged();
      if (m_alreadyExisted)
        setStatusText("Item already exists; using existing item instead. " + m_item->itemId());
      else
        setStatusText("Succesfully loaded item from portal. " + m_item->itemId());
    });
  }
}

bool AddItemsToPortal::portalLoaded() const
{
  return m_portal && (m_portal->loadStatus() == LoadStatus::Loaded);
}

bool AddItemsToPortal::portalItemLoaded() const
{
  return m_item && (m_item->loadStatus() == LoadStatus::Loaded);
}

QString AddItemsToPortal::portalItemId() const
{
  if (m_itemDeleted || !m_item)
    return QString();

  return m_item->itemId();
}

QString AddItemsToPortal::portalItemTitle() const
{
  if (m_itemDeleted || !m_item)
    return QString();

  return m_item->title();
}

QString AddItemsToPortal::portalItemTypeName() const
{
  if (m_itemDeleted || !m_item)
    return QString();

  return m_item->typeName();
}

bool AddItemsToPortal::itemDeleted() const
{
  return m_itemDeleted;
}

QString AddItemsToPortal::statusText() const
{
  return m_statusText;
}

bool AddItemsToPortal::busy() const
{
  return m_busy;
}

void AddItemsToPortal::authenticatePortal()
{
  if (!m_portal)
    return;
  if (m_portal->loadStatus() == LoadStatus::FailedToLoad)
    m_portal->retryLoad();
  else
    m_portal->load();
}

void AddItemsToPortal::addItem()
{
  if (!m_user || !m_item)
    return;

  m_busy = true;

  //! [PortalUser addItemWithUrlAsync]
  QUrl localCSV("qrc:/Samples/CloudAndPortal/AddItemsToPortal/add_item_sample.csv");
  m_user->addPortalItemWithUrlAsync(m_item, localCSV, "add_item_sample.csv" ).then(
  [this]()
  {
    m_busy = false;

    setStatusText("Successfully added item. " + m_item->itemId());
    m_item->load();
  })
  //! [PortalUser addItemWithUrlAsync]
  .onFailed(
  [this](const ErrorException& e)
  {
    m_busy = false;

    // Check for service error 409 "Conflict" - item already exists
    const QVariantMap additionalInfo = e.error().additionalInformation();
    if (additionalInfo.contains(ErrorInformationKeys::serviceError()) &&
        additionalInfo.value(ErrorInformationKeys::serviceError()).toInt() == 409)
    {
      m_alreadyExisted = true;
      setStatusText("Item already exists; fetching existing item instead. " + m_item->itemId());
      fetchItem();
    }
    else
    {
      setStatusText(e.error().message());
    }
  });
}

void AddItemsToPortal::deleteItem()
{
  if (!m_user || !m_item)
    return;

  m_busy = true;

  m_user->deletePortalItemAsync(m_item).then(
  [this]()
  {
    m_busy = false;
    m_itemDeleted = true;

    emit itemDeletedChanged();
    emit portalItemIdChanged();
    emit portalItemTitleChanged();
    emit portalItemTypeNameChanged();

    setStatusText("Successfully deleted item. " + m_item->itemId());
  }).onFailed(
  [this](const ErrorException& e)
  {
    m_busy = false;

    setStatusText(e.error().message());
  });
}

void AddItemsToPortal::fetchItem()
{
  if (!m_user || !m_item)
    return;

  m_busy = true;

  m_user->fetchContentAsync().then(
  [this]()
  {
    m_busy = false;

    for (PortalItem* item : *m_user->items())
    {
      if (item->title() == "Add Items Sample")
      {
        m_item->setItemId(item->itemId());
        m_item->load();
        return;
      }
    }
  });
}

void AddItemsToPortal::connectUserSignals()
{
  if (!m_user)
    return;

  connect(m_user, &PortalUser::errorOccurred, this, [this](const Error& error)
  {
    m_busy = false;
    setStatusText(QString(error.message() + ": " + error.additionalMessage()));
  });
}

void AddItemsToPortal::setStatusText(const QString &statusText)
{
  m_statusText = statusText;
  emit statusTextChanged();
}
