// [WriteFile Name=BrowseBuildingFloors, Category=Maps]
// [Legal]
// Copyright 2022 Esri.
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "BrowseBuildingFloors.h"

// ArcGIS Maps SDK headers
#include "Map.h"
#include "MapQuickView.h"
#include "Portal.h"
#include "PortalItem.h"

using namespace Esri::ArcGISRuntime;

BrowseBuildingFloors::BrowseBuildingFloors(QObject* parent /* = nullptr */) :
  QObject(parent)
{
  Portal* portal = new Portal(QUrl("https://www.arcgis.com/"), this);
  PortalItem* portalItem = new PortalItem(portal, "f133a698536f44c8884ad81f80b6cfc7", this);

  // Create a map object using the portalItem
  m_map = new Map(portalItem, this);
}

BrowseBuildingFloors::~BrowseBuildingFloors() = default;

void BrowseBuildingFloors::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<BrowseBuildingFloors>("Esri.Samples", 1, 0, "BrowseBuildingFloorsSample");
}

MapQuickView* BrowseBuildingFloors::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void BrowseBuildingFloors::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}
