// [WriteFile Name=OSM_Layer, Category=Layers]
// [Legal]
// Copyright 2017 Esri.
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
#include "OSM_Layer.h"

// ArcGIS Maps SDK headers
#include "Basemap.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "OpenStreetMapLayer.h"

using namespace Esri::ArcGISRuntime;

OSM_Layer::OSM_Layer(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void OSM_Layer::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<OSM_Layer>("Esri.Samples", 1, 0, "OSM_LayerSample");
}

void OSM_Layer::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map
  m_map = new Map(this);

  // Create a new OpenStreetMapLayer and add it to the list of basemap layers
  OpenStreetMapLayer* osm = new OpenStreetMapLayer(this);
  m_map->basemap()->baseLayers()->append(osm);

  // Set map to map view
  m_mapView->setMap(m_map);
}
