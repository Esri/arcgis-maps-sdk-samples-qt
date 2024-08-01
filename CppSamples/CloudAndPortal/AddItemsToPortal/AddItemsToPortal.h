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

#ifndef ADDITEMSTOPORTAL_H
#define ADDITEMSTOPORTAL_H

// Qt headers
#include <QQuickItem>

namespace Esri::ArcGISRuntime
{
    class Portal;
    class PortalItem;
    class PortalUser;
}

class AddItemsToPortal : public QQuickItem
{
  Q_OBJECT

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
  void portalLoadedChanged();
  void portalItemLoadedChanged();
  void portalItemTitleChanged();
  void portalItemIdChanged();
  void portalItemTypeNameChanged();
  void itemDeletedChanged();
  void statusTextChanged();
  void busyChanged();

private:
  void fetchItem();
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
  bool m_alreadyExisted = false;
};

#endif // ADDITEMSTOPORTAL_H
