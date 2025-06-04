// [WriteFile Name=PortalUserInfo, Category=CloudAndPortal]
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
#include "PortalUserInfo.h"

// ArcGIS Maps SDK headers
#include "Authentication/OAuthUserConfiguration.h"
#include "Error.h"
#include "MapTypes.h"
#include "Portal.h"
#include "PortalInfo.h"
#include "PortalTypes.h"
#include "PortalUser.h"

// Qt headers
#include <QUrl>

// Other headers
#include "AuthenticatorController.h"

using namespace Esri::ArcGISRuntime;
using namespace Esri::ArcGISRuntime::Authentication;
using namespace Esri::ArcGISRuntime::Toolkit;

const QString PortalUserInfo::UNKNOWN = "Unknown";

PortalUserInfo::PortalUserInfo(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_portal(new Portal(true, this))
{
  const QString redirectUrl{"urn:ietf:wg:oauth:2.0:oob"};
  OAuthUserConfiguration* config = new OAuthUserConfiguration(m_portal->url(), "iLkGIj0nX8A4EJda", redirectUrl, this);
  AuthenticatorController::instance()->addOAuthUserConfiguration(config);

  connect(m_portal, &Portal::loadStatusChanged, this, &PortalUserInfo::onPortalLoadStatusChanged);
  connect(m_portal, &Portal::doneLoading, this, &PortalUserInfo::loadErrorMessageChanged);
}

PortalUserInfo::~PortalUserInfo() = default;
void PortalUserInfo::init()
{
  qmlRegisterType<PortalUserInfo>("Esri.Samples", 1, 0, "PortalUserInfoSample");
}

void PortalUserInfo::componentComplete()
{
  QQuickItem::componentComplete();
  load();
}

void PortalUserInfo::load()
{
  if (!m_portal)
    return;

  if (m_portal->loadStatus() == LoadStatus::NotLoaded)
    m_portal->load();
  else if (m_portal->loadStatus() == LoadStatus::FailedToLoad)
    m_portal->retryLoad();
}

QString PortalUserInfo::username() const
{
  if (m_user)
    return m_user->username();

  return UNKNOWN;
}

bool PortalUserInfo::loaded()
{
  if (m_portal)
    return m_portal->loadStatus() == LoadStatus::Loaded;

  return false;
}

bool PortalUserInfo::loginDismissed()
{
  if (m_portal)
    return m_portal->loadError().message() == "Code unauthorized." || m_portal->loadError().message() == "User canceled error.";

  return false;
}

QString PortalUserInfo::fullName() const
{
  if (m_user)
    return m_user->fullName();

  return UNKNOWN;
}

QString PortalUserInfo::email() const
{
  if (m_user)
    return m_user->email();

  return UNKNOWN;
}

QString PortalUserInfo::bio() const
{
  if (m_user)
    return m_user->userDescription();

  return UNKNOWN;
}

QString PortalUserInfo::access() const
{
  if (!m_user)
    return UNKNOWN;

  switch (m_user->access())
  {
    return UNKNOWN;
  case PortalAccess::Organization:
    return "Organization";
  case PortalAccess::Private:
    return "Only you";
  case PortalAccess::Public:
    return "Everyone";
  case PortalAccess::Shared:
    return "Shared Groups";
  default:
    return UNKNOWN;
  }
}

QUrl PortalUserInfo::thumbnailUrl() const
{
  if (m_user && !m_user->thumbnailUrl().isEmpty())
    return m_user->thumbnailUrl();

  return QUrl("qrc:/Samples/CloudAndPortal/PortalUserInfo/placeholder_img.png");
}

QString PortalUserInfo::orgTitle() const
{
    if (m_portal && m_portal->portalInfo())
        return m_portal->portalInfo()->organizationName();

    return "";
}

QString PortalUserInfo::orgDescription() const
{
    if (m_portal && m_portal->portalInfo())
        return m_portal->portalInfo()->organizationDescription();

    return "";
}

QUrl PortalUserInfo::orgThumbnailUrl() const
{
    if (m_portal && m_portal->portalInfo())
        return m_portal->portalInfo()->thumbnailUrl();

    return QUrl();
}

QString PortalUserInfo::canSearchPublic() const
{
    if (m_portal && m_portal->portalInfo())
        return m_portal->portalInfo()->isCanSearchPublic() ? "True" : "False";

    return "";
}

QString PortalUserInfo::canSharePublic() const
{
    if (m_portal && m_portal->portalInfo())
        return m_portal->portalInfo()->isCanSharePublic() ? "True" : "False";

    return "";
}

QString PortalUserInfo::loadErrorMessage() const
{
  if (m_portal)
    return m_portal->loadError().message();

  return "";
}

void PortalUserInfo::onPortalLoadStatusChanged(LoadStatus loadStatus)
{
    switch (loadStatus) {
    case LoadStatus::Loaded:
        if (m_portal)
        {
            m_user = m_portal->portalUser();
            connect(m_user, &PortalUser::thumbnailUrlChanged, this, &PortalUserInfo::thumbnailUrlChanged);
        }
        emit fullNameChanged();
        emit usernameChanged();
        emit emailChanged();
        emit bioChanged();
        emit accessChanged();
        emit thumbnailUrlChanged();
        break;
    case LoadStatus::Loading:
        break;
    case LoadStatus::FailedToLoad:
        break;
    case LoadStatus::NotLoaded:
        break;
    case LoadStatus::Unknown:
        break;
    default:
        break;
  }

  emit loadedChanged();
  emit isLoginDismissed();
}
