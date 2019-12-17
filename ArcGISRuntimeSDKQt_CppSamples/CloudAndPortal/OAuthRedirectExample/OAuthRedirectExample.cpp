// [WriteFile Name=OAuthRedirectExample, Category=CloudAndPortal]
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
// [Legal]

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "Credential.h"
#include "Portal.h"

#include "OAuthRedirectExample.h"
#include "OAuthRedirectHandler.h"

#define STRINGIZE(x) #x
#define QUOTE(x) STRINGIZE(x)

using namespace Esri::ArcGISRuntime;

OAuthRedirectExample::OAuthRedirectExample(QQuickItem* parent /* = nullptr */):
    QQuickItem(parent),
    m_portal(new Portal(new Credential(OAuthClientInfo(QUOTE(CLIENT_ID), "", QString("%1://").arg(customUrlProtocol()), OAuthMode::User ), this), this)),
    m_handler(new OAuthRedirectHandler(customUrlProtocol(), this))
{
    connect(m_handler, &OAuthRedirectHandler::statusChanged, this, &OAuthRedirectExample::statusChanged);
    connect(m_portal, &Portal::loadStatusChanged, this, &OAuthRedirectExample::portalLoadStatusChanged);
    connect(m_portal, &Portal::loadStatusChanged, this, &OAuthRedirectExample::statusChanged);
}

OAuthRedirectExample::~OAuthRedirectExample() = default;

//static
QString OAuthRedirectExample::customUrlProtocol()
{
    return QUOTE(URL_SCHEME);
}

//static
QString OAuthRedirectExample::clientId() const
{
    return m_portal->credential()->oAuthClientInfo().clientId();
}

void OAuthRedirectExample::componentComplete()
{
  QQuickItem::componentComplete();
}

void OAuthRedirectExample::loadPortal()
{
    m_portal->load();
    emit statusChanged();
}

void OAuthRedirectExample::setClientId(const QString &clientId)
{
    if (m_portal->credential()->oAuthClientInfo().clientId() == clientId)
        return;

    m_portal->credential()->setOAuthClientInfo(
        OAuthClientInfo(clientId, "", QString("%1://").arg(customUrlProtocol()), OAuthMode::User));
    qDebug() << "clientId" << m_portal->credential()->oAuthClientInfo().clientId();
    emit clientIdChanged();
}

QString OAuthRedirectExample::portalLoadStatus() const
{
    switch (m_portal->loadStatus())
    {
    case LoadStatus::FailedToLoad:
        return "Failed to Load";
    case LoadStatus::Loaded:
        return "Loaded";
    case LoadStatus::Loading:
        return "Loading...";
    case LoadStatus::NotLoaded:
        return "Not Loaded";
    case LoadStatus::Unknown:
    default:
        return "Unknown";
    }

    return "";
}

QString OAuthRedirectExample::status() const
{
    switch (m_portal->loadStatus())
    {
    case LoadStatus::FailedToLoad:
        return m_portal->loadError().message();
    case LoadStatus::Loaded:
        return "successfully authenticated!";
    default:
        break;
    }

    switch (m_handler->status())
    {
    case OAuthRedirectHandlerStatus::Idle:
        return "waiting";
    case OAuthRedirectHandlerStatus::ChallengeReceived:
        return "challenge received";
    case OAuthRedirectHandlerStatus::AuthorizeUrlOpened:
        return "authorizeUrl opened";
    case OAuthRedirectHandlerStatus::MessageReceived:
        return "app msg received";
    case OAuthRedirectHandlerStatus::AuthorizationCodeSent:
        return "authorization code sent";
    default:
        break;
    }

    return "";
}

QString OAuthRedirectExample::portalName() const
{
    if (m_portal->loadStatus() != LoadStatus::Loaded)
        return "";

    return m_portal->portalInfo()->organizationName();
}
