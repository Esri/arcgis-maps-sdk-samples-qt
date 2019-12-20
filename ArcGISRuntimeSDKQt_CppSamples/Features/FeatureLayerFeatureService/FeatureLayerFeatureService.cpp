// [WriteFile Name=FeatureLayerFeatureService, Category=Features]
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

#include "FeatureLayerFeatureService.h"

#include "Map.h"
#include "MapQuickView.h"
#include "FeatureLayer.h"
#include "Basemap.h"
#include "SpatialReference.h"
#include "ServiceFeatureTable.h"
#include "Viewpoint.h"
#include "Point.h"
#include <QUrl>

using namespace Esri::ArcGISRuntime;

FeatureLayerFeatureService::FeatureLayerFeatureService(QQuickItem* parent) :
  QQuickItem(parent)
{
}

FeatureLayerFeatureService::~FeatureLayerFeatureService() = default;
void FeatureLayerFeatureService::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<FeatureLayerFeatureService>("Esri.Samples", 1, 0, "FeatureLayerFeatureServiceSample");
}

void FeatureLayerFeatureService::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create a map using the terrain with labels basemap
  m_map = new Map(Basemap::terrainWithLabels(this), this);
  m_map->setInitialViewpoint(Viewpoint(Point(-13176752, 4090404, SpatialReference(102100)), 300000));

  // Set map to map view
  m_mapView->setMap(m_map);

  //! [Display Feature Service]
  // create the feature table
  m_featureTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/Energy/Geology/FeatureServer/9"), this);

  // create the feature layer using the feature table
  m_featureLayer = new FeatureLayer(m_featureTable, this);

  // add the feature layer to the map
  m_map->operationalLayers()->append(m_featureLayer);
  //! [Display Feature Service]
}
