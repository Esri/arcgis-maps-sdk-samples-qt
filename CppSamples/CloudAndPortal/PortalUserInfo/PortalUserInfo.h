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

#ifndef PORTALUSERINFO_H
#define PORTALUSERINFO_H

// Qt headers
#include <QQuickItem>
#include <QUrl>

namespace Esri::ArcGISRuntime
{
    class Portal;
    class PortalUser;
    enum class LoadStatus;
}

class PortalUserInfo : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(QString username READ username NOTIFY usernameChanged)
  Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)
  Q_PROPERTY(QString fullName READ fullName NOTIFY fullNameChanged)
  Q_PROPERTY(QString email READ email NOTIFY emailChanged)
  Q_PROPERTY(QString bio READ bio NOTIFY bioChanged)
  Q_PROPERTY(QString access READ access NOTIFY accessChanged)
  Q_PROPERTY(QUrl thumbnailUrl READ thumbnailUrl NOTIFY thumbnailUrlChanged)
  Q_PROPERTY(QString orgTitle READ orgTitle NOTIFY loadedChanged)
  Q_PROPERTY(QString orgDescription READ orgDescription NOTIFY loadedChanged)
  Q_PROPERTY(QUrl orgThumbnailUrl READ orgThumbnailUrl NOTIFY loadedChanged)
  Q_PROPERTY(QUrl canSearchPublic READ canSearchPublic NOTIFY loadedChanged)
  Q_PROPERTY(QUrl canSharePublic READ canSharePublic NOTIFY loadedChanged)
  Q_PROPERTY(QString loadErrorMessage READ loadErrorMessage NOTIFY loadedChanged)
  Q_PROPERTY(bool loginDismissed READ loginDismissed NOTIFY isLoginDismissed)

public:
  explicit PortalUserInfo(QQuickItem* parent = nullptr);
  ~PortalUserInfo() override;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void load();

signals:
  void loadedChanged();
  void usernameChanged();
  void fullNameChanged();
  void emailChanged();
  void bioChanged();
  void accessChanged();
  void thumbnailUrlChanged();
  void loadErrorMessageChanged();
  void isLoginDismissed();

private slots:
  void onPortalLoadStatusChanged(Esri::ArcGISRuntime::LoadStatus loadStatus);

private:
  QString username() const;
  bool loaded();
  QString fullName() const;
  QString email() const;
  QString bio() const;
  QString access() const;
  QUrl thumbnailUrl() const;
  QString orgTitle() const;
  QString orgDescription() const;
  QUrl orgThumbnailUrl() const;
  QString canSearchPublic() const;
  QString canSharePublic() const;
  QString loadErrorMessage() const;
  bool loginDismissed();

  static const QString UNKNOWN;

  Esri::ArcGISRuntime::Portal* m_portal = nullptr;
  Esri::ArcGISRuntime::PortalUser* m_user = nullptr;
};

#endif // PORTALUSERINFO_H
