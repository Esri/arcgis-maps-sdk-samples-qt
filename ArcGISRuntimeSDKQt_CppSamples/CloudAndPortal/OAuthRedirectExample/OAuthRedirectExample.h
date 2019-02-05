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
#ifndef OAUTHREDIRECTEXAMPLE_H
#define OAUTHREDIRECTEXAMPLE_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Portal;
  }
}

class OAuthRedirectHandler;

#include <QQuickItem>
#include <QUrl>

class OAuthRedirectExample : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(QString  clientId READ clientId WRITE setClientId NOTIFY clientIdChanged)
  Q_PROPERTY(QString portalLoadStatus READ portalLoadStatus NOTIFY portalLoadStatusChanged)
  Q_PROPERTY(QString status READ status NOTIFY statusChanged)
  Q_PROPERTY(QString portalName READ portalName NOTIFY portalLoadStatusChanged)

public:
  OAuthRedirectExample(QQuickItem* parent = nullptr);
  ~OAuthRedirectExample() override;

  void componentComplete() override;

  Q_INVOKABLE void loadPortal();
  Q_INVOKABLE void setClientId(const QString& clientId);

  static QString customUrlProtocol();

signals:
  void clientIdChanged();
  void portalLoadStatusChanged();
  void statusChanged();
  void portalNameChanged();

private:
  QString clientId() const;
  QString portalLoadStatus() const;
  QString status() const;
  QString portalName() const;

  Esri::ArcGISRuntime::Portal* m_portal = nullptr;
  OAuthRedirectHandler* m_handler = nullptr;
};

#endif // OAUTHREDIRECTEXAMPLE_H
