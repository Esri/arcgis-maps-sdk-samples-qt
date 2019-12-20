// [WriteFile Name=SetInitialMapLocation, Category=Maps]
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

#include "SetInitialMapLocation.h"

#include "Map.h"
#include "MapQuickView.h"

using namespace Esri::ArcGISRuntime;

SetInitialMapLocation::SetInitialMapLocation(QQuickItem* parent) :
  QQuickItem(parent)
{
}

SetInitialMapLocation::~SetInitialMapLocation() = default;

void SetInitialMapLocation::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<SetInitialMapLocation>("Esri.Samples", 1, 0, "SetInitialMapLocationSample");
}

void SetInitialMapLocation::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a new map with the basemap type enum and pass in initial lat, lon, and scale
  m_map = new Map(BasemapType::ImageryWithLabels, -33.867886, -63.985, 16, this);
  // set map on the map view
  m_mapView->setMap(m_map);
}

