// [WriteFile Name=MinMaxScale, Category=Maps]
// [Legal]
// Copyright 2018 Esri.

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

#include "MinMaxScale.h"

#include "Map.h"
#include "MapQuickView.h"

using namespace Esri::ArcGISRuntime;

MinMaxScale::MinMaxScale(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void MinMaxScale::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<MinMaxScale>("Esri.Samples", 1, 0, "MinMaxScaleSample");
}

void MinMaxScale::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the streets basemap
  m_map = new Map(Basemap::streets(this), this);

  // Establish the minimum and maximum scales for the map
  m_map->setMaxScale(2000);
  m_map->setMinScale(8000);

  // Set map to map view
  m_mapView->setMap(m_map);

  Point edinburgh(-3.1927,55.9515, SpatialReference(4326));
  m_mapView->setViewpointCenter(edinburgh);
}
