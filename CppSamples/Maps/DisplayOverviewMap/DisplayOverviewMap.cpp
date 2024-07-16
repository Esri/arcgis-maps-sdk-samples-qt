// [WriteFile Name=DisplayOverviewMap, Category=Maps]
// [Legal]
// Copyright 2021 Esri.
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
#include "DisplayOverviewMap.h"

// C++ API headers
#include "Basemap.h"
#include "FeatureLayer.h"
#include "GeodatabaseTypes.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "ServiceFeatureTable.h"
#include "Viewpoint.h"

using namespace Esri::ArcGISRuntime;

DisplayOverviewMap::DisplayOverviewMap(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(new Basemap(BasemapStyle::ArcGISTopographic, this), this))
{
  m_map->setInitialViewpoint(Viewpoint(49.28299, -123.12052, 70000));

  // Access the feature layer and add it to the maps operational layers.
  ServiceFeatureTable* serviceFeatureTable = new ServiceFeatureTable(QUrl("https://services6.arcgis.com/Do88DoK2xjTUCXd1/arcgis/rest/services/OSM_Tourism_NA/FeatureServer/0"), this);
  serviceFeatureTable->setFeatureRequestMode(FeatureRequestMode::OnInteractionCache);
  FeatureLayer* featureLayer = new FeatureLayer(serviceFeatureTable, this);
  m_map->operationalLayers()->append(featureLayer);
}

DisplayOverviewMap::~DisplayOverviewMap() = default;

void DisplayOverviewMap::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DisplayOverviewMap>("Esri.Samples", 1, 0, "DisplayOverviewMapSample");
}

MapQuickView* DisplayOverviewMap::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void DisplayOverviewMap::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}
