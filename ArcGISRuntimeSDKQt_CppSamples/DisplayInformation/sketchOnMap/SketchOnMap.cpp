// [WriteFile Name=SketchOnMap, Category=DisplayInformation]
// [Legal]
// Copyright 2020 Esri.

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

#include "SketchOnMap.h"

#include "Map.h"
#include "MapQuickView.h"

using namespace Esri::ArcGISRuntime;

SketchOnMap::SketchOnMap(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::imagery(this), this))
{

}

SketchOnMap::~SketchOnMap() = default;

void SketchOnMap::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<SketchOnMap>("Esri.Samples", 1, 0, "SketchOnMapSample");
}

MapQuickView* SketchOnMap::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void SketchOnMap::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}
