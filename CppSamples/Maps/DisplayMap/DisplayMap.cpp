// [WriteFile Name=DisplayMap, Category=Maps]
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
#include "DisplayMap.h"

// C++ API headers
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"

using namespace Esri::ArcGISRuntime;

DisplayMap::DisplayMap(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISImagery, this))
{
}

DisplayMap::~DisplayMap() = default;

void DisplayMap::init()
{
  // Register the map view for QML
  //! [Register the map view for QML]
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  //! [Register the map view for QML]
  qmlRegisterType<DisplayMap>("Esri.Samples", 1, 0, "DisplayMapSample");
}

MapQuickView* DisplayMap::mapView() const
{
  return m_mapView;
}

//! [MapQuickView API Snippet]
// Set the view (created in QML)
void DisplayMap::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}
 //! [MapQuickView API Snippet]
