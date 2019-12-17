// [WriteFile Name=OpenMapUrl, Category=Maps]
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "OpenMapUrl.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"

using namespace Esri::ArcGISRuntime;

OpenMapUrl::OpenMapUrl(QQuickItem* parent) :
  QQuickItem(parent)
{
}

OpenMapUrl::~OpenMapUrl() = default;

void OpenMapUrl::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<OpenMapUrl>("Esri.Samples", 1, 0, "OpenMapUrlSample");
}

void OpenMapUrl::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // create a new basemap instance
  Basemap* basemap = Basemap::imagery(this);
  // create a new map instance
  Map* map = new Map(basemap, this);
  // set map on the map view
  m_mapView->setMap(map);
}

void OpenMapUrl::openMap(const QString& itemId)
{
  //! [Construct map from a webmap Url]
  // create a QUrl using the item id QString
  QString organizationPortalUrl(QStringLiteral("https://arcgis.com"));
  const QUrl webmapUrl(QString(organizationPortalUrl + "/sharing/rest/content/items/" + itemId));
  // create a new map from the webmap Url
  Map* map = new Map(webmapUrl, this);
  //! [Construct map from a webmap Url]

  // set the map to the map view
  m_mapView->setMap(map);
}
