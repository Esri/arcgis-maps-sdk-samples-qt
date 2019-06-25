// [WriteFile Name=IntegratedWindowsAuthentication, Category=CloudAndPortal]
// [Legal]
// Copyright 2019 Esri.

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

#ifndef INTEGRATEDWINDOWSAUTHENTICATION_H
#define INTEGRATEDWINDOWSAUTHENTICATION_H

namespace Esri
{
namespace ArcGISRuntime
{
class AuthenticationManager;
class Map;
class MapQuickView;
class Portal;
class PortalItem;
class PortalQueryResultSetForItems;
}
}

#include <QObject>

class IntegratedWindowsAuthentication : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::AuthenticationManager* authManager READ authManager NOTIFY authManagerChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QStringList webmapListModel MEMBER m_webmapList NOTIFY webmapListModelChanged)

public:
  explicit IntegratedWindowsAuthentication(QObject* parent = nullptr);
  ~IntegratedWindowsAuthentication();

  static void init();

  Esri::ArcGISRuntime::AuthenticationManager* authManager() const;
  Q_INVOKABLE void searchPortal(QString url);

signals:
  void mapViewChanged();
  void authManagerChanged();
  void webmapListModelChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Portal* m_portal = nullptr;
  Esri::ArcGISRuntime::PortalQueryResultSetForItems* m_webmapResults = nullptr;
  QStringList m_webmapList;
  //const QString arcgis_url = "http://www.arcgis.com";
};

#endif // INTEGRATEDWINDOWSAUTHENTICATION_H
