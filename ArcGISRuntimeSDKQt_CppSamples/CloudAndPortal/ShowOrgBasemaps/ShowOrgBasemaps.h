// [WriteFile Name=ShowOrgBasemaps, Category=CloudAndPortal]
// [Legal]
// Copyright 2016 Esri.

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

#ifndef SHOWORGBASEMAPS_H
#define SHOWORGBASEMAPS_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class AuthenticationManager;
    class BasemapListModel;
    class Map;
    class MapQuickView;
    class Portal;
  }
}

#include <QAbstractListModel>
#include <QQuickItem>

class ShowOrgBasemaps : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::AuthenticationManager* authManager READ authManager NOTIFY authManagerChanged)
  Q_PROPERTY(bool portalLoaded READ portalLoaded NOTIFY portalLoadedChanged)
  Q_PROPERTY(QString orgName READ orgName NOTIFY orgNameChanged)
  Q_PROPERTY(QAbstractListModel* basemaps READ basemaps NOTIFY basemapsChanged)
  Q_PROPERTY(QString mapLoadError READ mapLoadError NOTIFY mapLoadErrorChanged)

public:
  explicit ShowOrgBasemaps(QQuickItem* parent = nullptr);
  ~ShowOrgBasemaps() override;

  void componentComplete() override;
  static void init();

  Esri::ArcGISRuntime::AuthenticationManager* authManager() const;
  bool portalLoaded() const;
  QString orgName() const;
  QAbstractListModel* basemaps() const;
  QString mapLoadError() const;

  Q_INVOKABLE void load(bool anonymous);
  Q_INVOKABLE void loadSelectedBasemap(int index);
  Q_INVOKABLE void errorAccepted();

signals:
  void authManagerChanged();
  void portalLoadedChanged();
  void orgNameChanged();
  void basemapsChanged();
  void mapLoadErrorChanged();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Portal* m_portal = nullptr;
  bool m_portalLoaded = false;
  QString m_mapLoadError;
};

#endif // SHOWORGBASEMAPS_H
