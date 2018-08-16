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
#include "AuthenticationChallenge.h"
#include "Credential.h"

#include "OAuthRedirectHandler.h"

#include <QUrl>
#include <QDesktopServices>

#include "MyApplication.h"

using namespace Esri::ArcGISRuntime;

OAuthRedirectHandler::OAuthRedirectHandler(const QString& urlScheme, QObject* parent /* = nullptr */):
  QObject(parent),
  m_challenge(nullptr),
  m_status(OAuthRedirectHandlerStatus::Idle)
{
  // for iOS
  QDesktopServices::setUrlHandler(urlScheme, this, "handleAppUrl");

#if defined(Q_OS_WIN) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
  QCoreApplication* coreApp = MyApplication::instance();
  if (coreApp)
  {
    MyApplication* singleApp = qobject_cast< MyApplication* >(coreApp);
    if (singleApp)
      connect(singleApp, &MyApplication::messageReceived, this, &OAuthRedirectHandler::handleAppMessage);
  }
#endif

  connect(AuthenticationManager::instance(), &AuthenticationManager::authenticationChallenge, this, [this](AuthenticationChallenge* challenge)
  {
    m_status = OAuthRedirectHandlerStatus::ChallengeReceived;
    emit statusChanged();

    if (!challenge || !challenge->proposedCredential())
      return;

    if (challenge->proposedCredential()->oAuthClientInfo().redirectUri().isEmpty())
      return;

    m_challenge = challenge;

    QDesktopServices::openUrl(m_challenge->authorizationUrl());
    m_status = OAuthRedirectHandlerStatus::AuthorizeUrlOpened;
    emit statusChanged();
  });
}

OAuthRedirectHandler::~OAuthRedirectHandler()
{
}

void OAuthRedirectHandler::handleAppMessage(const QString& msg)
{
  m_status = OAuthRedirectHandlerStatus::MessageReceived;
  emit statusChanged();

  if (!m_challenge)
    return;

  QStringList parts = msg.split("/?code=");
  if (parts.size() < 2)
    return;

  QString code = parts[1];

  m_challenge->continueWithOAuthAuthorizationCode(code);
  m_status = OAuthRedirectHandlerStatus::AuthorizationCodeSent;
  emit statusChanged();
}

void OAuthRedirectHandler::handleAppUrl(const QUrl& url)
{
  handleAppMessage(url.toString());
}

OAuthRedirectHandlerStatus OAuthRedirectHandler::status() const
{
  return m_status;
}
