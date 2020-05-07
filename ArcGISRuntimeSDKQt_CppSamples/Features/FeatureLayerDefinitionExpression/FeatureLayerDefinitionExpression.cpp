// [WriteFile Name=FeatureLayerDefinitionExpression, Category=Features]
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

#include "FeatureLayerDefinitionExpression.h"

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

FeatureLayerDefinitionExpression::FeatureLayerDefinitionExpression(QQuickItem* parent) :
  QQuickItem(parent)
{
}

FeatureLayerDefinitionExpression::~FeatureLayerDefinitionExpression() = default;

void FeatureLayerDefinitionExpression::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<FeatureLayerDefinitionExpression>("Esri.Samples", 1, 0, "FeatureLayerDefinitionExpressionSample");
}

void FeatureLayerDefinitionExpression::componentComplete()
{
  QQuickItem::componentComplete();

  //! [Obtain the instantiated map view in Cpp]
  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create a map using the topographic basemap
  m_map = new Map(Basemap::topographic(this), this);
  const Point center = Point(-13630484, 4545415, SpatialReference(102100));
  constexpr double scale = 300000.0;
  m_map->setInitialViewpoint(Viewpoint(center, scale));

  // Set map to map view
  m_mapView->setMap(m_map);
  //! [Obtain the instantiated map view in Cpp]

  // create the feature table
  ServiceFeatureTable* featureTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/SF311/FeatureServer/0"), this);
  // create the feature layer using the feature table
  m_featureLayer = new FeatureLayer(featureTable, this);

  connect(m_featureLayer, &FeatureLayer::loadStatusChanged, this, [this](LoadStatus loadStatus)
  {
    loadStatus == LoadStatus::Loaded ? m_initialized = true : m_initialized = false;
    emit layerInitializedChanged();
  });

  // add the feature layer to the map
  m_map->operationalLayers()->append(m_featureLayer);
}

bool FeatureLayerDefinitionExpression::layerInitialized() const
{
  return m_initialized;
}

void FeatureLayerDefinitionExpression::setDefExpression(QString whereClause)
{
  // In QML, "req_Type = \'Tree Maintenance or Damage\'"
  m_featureLayer->setDefinitionExpression(whereClause);
}
