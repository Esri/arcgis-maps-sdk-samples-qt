// [WriteFile Name=MapLoaded, Category=Maps]
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

#include "MapLoaded.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"

using namespace Esri::ArcGISRuntime;

MapLoaded::MapLoaded(QQuickItem* parent) :
  QQuickItem(parent)
{
}

MapLoaded::~MapLoaded() = default;

void MapLoaded::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<MapLoaded>("Esri.Samples", 1, 0, "MapLoadedSample");
}

void MapLoaded::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // create a new basemap instance
  Basemap* basemap = Basemap::streets(this);
  // create a new map instance
  m_map = new Map(basemap, this);
  // set map on the map view
  m_mapView->setMap(m_map);

  connect(m_map, &Esri::ArcGISRuntime::Map::loadStatusChanged, this, [this](Esri::ArcGISRuntime::LoadStatus loadStatus)
  {
    switch (loadStatus)
    {
    case LoadStatus::Loaded:
      m_loadStatus = "Map Load Status: Loaded";
      break;
    case LoadStatus::Loading:
      m_loadStatus = "Map Load Status: Loading...";
      break;
    case LoadStatus::FailedToLoad:
      m_loadStatus = "Map Load Status: Failed to Load";
      break;
    case LoadStatus::NotLoaded:
      m_loadStatus = "Map Load Status: Not Loaded";
      break;
    case LoadStatus::Unknown:
      m_loadStatus = "Map Load Status: Unknown";
      break;
    }
    emit mapLoadStatusChanged();
  });
}

QString MapLoaded::mapLoadStatus() const
{
  return m_loadStatus;
}

