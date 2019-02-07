// [WriteFile Name=AddItemsToPortal, Category=CloudAndPortal]
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

#ifndef ADDITEMSTOPORTAL_H
#define ADDITEMSTOPORTAL_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class AuthenticationManager;
    class Portal;
    class PortalItem;
    class PortalUser;
  }
}

#include <QQuickItem>

class AddItemsToPortal : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::AuthenticationManager* authManager READ authManager NOTIFY authManagerChanged)
  Q_PROPERTY(bool portalLoaded READ portalLoaded NOTIFY portalLoadedChanged)
  Q_PROPERTY(bool portalItemLoaded READ portalItemLoaded NOTIFY portalItemLoadedChanged)
  Q_PROPERTY(QString portalItemId READ portalItemId NOTIFY portalItemIdChanged)
  Q_PROPERTY(QString portalItemTitle READ portalItemTitle NOTIFY portalItemTitleChanged)
  Q_PROPERTY(QString portalItemTypeName READ portalItemTypeName NOTIFY portalItemTypeNameChanged)
  Q_PROPERTY(bool itemDeleted READ itemDeleted NOTIFY itemDeletedChanged)
  Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
  Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)

public:
  explicit AddItemsToPortal(QQuickItem* parent = nullptr);
  ~AddItemsToPortal() override;

  void componentComplete() override;
  static void init();

  Q_INVOKABLE void authenticatePortal();
  Q_INVOKABLE void addItem();
  Q_INVOKABLE void deleteItem();

signals:
  void authManagerChanged();
  void portalLoadedChanged();
  void portalItemLoadedChanged();
  void portalItemTitleChanged();
  void portalItemIdChanged();
  void portalItemTypeNameChanged();
  void itemDeletedChanged();
  void statusTextChanged();
  void busyChanged();

private:
  Esri::ArcGISRuntime::AuthenticationManager* authManager() const;
  bool portalLoaded() const;
  bool portalItemLoaded() const;
  QString portalItemId() const;
  QString portalItemTitle() const;
  QString portalItemTypeName() const;
  bool itemDeleted() const;
  QString statusText() const;
  bool busy() const;

  void setStatusText(const QString& msg);
  void connectUserSignals();

  Esri::ArcGISRuntime::Portal* m_portal = nullptr;
  Esri::ArcGISRuntime::PortalItem* m_item = nullptr;
  Esri::ArcGISRuntime::PortalUser* m_user = nullptr;
  bool m_itemDeleted = false;
  QString m_statusText;
  bool m_busy = false;
};

#endif // ADDITEMSTOPORTAL_H
