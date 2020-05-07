// [WriteFile Name=ShowMagnifier, Category=Maps]
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

#include "ShowMagnifier.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "Viewpoint.h"

using namespace Esri::ArcGISRuntime;

ShowMagnifier::ShowMagnifier(QQuickItem* parent) :
  QQuickItem(parent)
{
}

ShowMagnifier::~ShowMagnifier() = default;

void ShowMagnifier::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ShowMagnifier>("Esri.Samples", 1, 0, "ShowMagnifierSample");
}

void ShowMagnifier::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // create a new basemap instance
  Basemap* basemap = Basemap::imagery(this);

  // create a new map instance
  m_map = new Map(basemap, this);
  m_map->setInitialViewpoint(Viewpoint(Point(-110.8258, 32.1545089, SpatialReference::wgs84()), 2e4));

  // set map on the map view
  m_mapView->setMap(m_map);

  // enable magnifier
  m_mapView->setMagnifierEnabled(true);
}

