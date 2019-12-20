// [WriteFile Name=DisplayMap, Category=Maps]
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

#include "DisplayMap.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"

using namespace Esri::ArcGISRuntime;

DisplayMap::DisplayMap(QQuickItem* parent) :
  QQuickItem(parent)
{
}

DisplayMap::~DisplayMap() = default;

void DisplayMap::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DisplayMap>("Esri.Samples", 1, 0, "DisplayMapSample");
}

void DisplayMap::componentComplete()
{
  QQuickItem::componentComplete();

  //! [MapQuickView API Snippet]
  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // create a new basemap instance
  Basemap* basemap = Basemap::imageryWithLabels(this);
  // create a new map instance
  m_map = new Map(basemap, this);
  // set map on the map view
  m_mapView->setMap(m_map);
  //! [MapQuickView API Snippet]
}

