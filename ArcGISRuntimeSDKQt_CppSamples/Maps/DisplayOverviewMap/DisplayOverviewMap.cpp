// [WriteFile Name=DisplayOverviewMap, Category=Maps]
// [Legal]
// Copyright 2021 Esri.

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

#include "DisplayOverviewMap.h"

#include "FeatureLayer.h"
#include "Map.h"
#include "MapQuickView.h"
#include "ServiceFeatureTable.h"
#include "Viewpoint.h"

namespace
{
  QUrl featureTableURL = QUrl("https://services6.arcgis.com/Do88DoK2xjTUCXd1/arcgis/rest/services/OSM_Tourism_NA/FeatureServer/0");
}

using namespace Esri::ArcGISRuntime;

DisplayOverviewMap::DisplayOverviewMap(QObject* parent /* = nullptr */):
  QObject(parent),
  m_basemap(new Basemap(BasemapStyle::ArcGISTopographic, this)),
  m_map(new Map(m_basemap, this))
{
  m_map->setInitialViewpoint(Viewpoint(49.28299, -123.12052, 70000));

  connect(m_map, &Map::doneLoading, this, &DisplayOverviewMap::addFeatureLayerToMap);
}

DisplayOverviewMap::~DisplayOverviewMap() = default;

void DisplayOverviewMap::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DisplayOverviewMap>("Esri.Samples", 1, 0, "DisplayOverviewMapSample");
}

void DisplayOverviewMap::addFeatureLayerToMap()
{
  // Access the feature layer and add it to the mapView
  ServiceFeatureTable* serviceFeatureTable = new ServiceFeatureTable(featureTableURL, this);
  serviceFeatureTable->setFeatureRequestMode(FeatureRequestMode::OnInteractionCache);

  FeatureLayer* featureLayer = new FeatureLayer(serviceFeatureTable, this);

  m_map->operationalLayers()->append(featureLayer);
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
