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

#ifndef OAUTHREDIRECTHANDLER_H
#define OAUTHREDIRECTHANDLER_H

namespace Esri
{
namespace ArcGISRuntime
{
class AuthenticationChallenge;
}
}

#include <QObject>

enum class OAuthRedirectHandlerStatus : unsigned int
{
  Idle,
  ChallengeReceived,
  AuthorizeUrlOpened,
  MessageReceived,
  AuthorizationCodeSent
};

class OAuthRedirectHandler : public QObject
{
  Q_OBJECT

public:

  OAuthRedirectHandler(const QString& urlScheme, QObject* parent = nullptr);
  ~OAuthRedirectHandler() override;

  OAuthRedirectHandlerStatus status() const;

private slots:
  void handleAppMessage(const QString& msg);
  void handleAppUrl(const QUrl& url);

signals:
  void statusChanged();

private:
  Esri::ArcGISRuntime::AuthenticationChallenge* m_challenge = nullptr;
  OAuthRedirectHandlerStatus m_status;
};

#endif // OAUTHREDIRECTHANDLER_H
