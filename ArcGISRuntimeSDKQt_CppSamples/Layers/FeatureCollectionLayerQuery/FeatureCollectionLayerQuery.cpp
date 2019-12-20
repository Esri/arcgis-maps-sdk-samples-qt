// [WriteFile Name=FeatureCollectionLayerQuery, Category=Layers]
// [Legal]
// Copyright 2017 Esri.

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

#include "FeatureCollectionLayerQuery.h"

#include "FeatureCollection.h"
#include "FeatureCollectionLayer.h"
#include "FeatureCollectionTable.h"
#include "Map.h"
#include "MapQuickView.h"
#include "ServiceFeatureTable.h"

#include <QString>

using namespace Esri::ArcGISRuntime;

FeatureCollectionLayerQuery::FeatureCollectionLayerQuery(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void FeatureCollectionLayerQuery::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<FeatureCollectionLayerQuery>("Esri.Samples", 1, 0, "FeatureCollectionLayerQuerySample");
}

bool FeatureCollectionLayerQuery::busy() const
{
  return m_busy;
}

void FeatureCollectionLayerQuery::setBusy(bool val)
{
  if (val == m_busy)
    return;

  m_busy = val;
  emit busyChanged();
}

void FeatureCollectionLayerQuery::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the oceans basemap
  m_map = new Map(Basemap::oceans(this), this);

  // Set map to map view
  m_mapView->setMap(m_map);

  //initialize service feature table to be queried
  m_featureTable = new ServiceFeatureTable(QUrl(QStringLiteral("https://sampleserver6.arcgisonline.com/arcgis/rest/services/Wildfire/FeatureServer/0")), this);

  //create query parameters
  QueryParameters queryParams;

  // 1=1 will give all the features from the table
  queryParams.setWhereClause("1=1");

  connect(m_featureTable, &ServiceFeatureTable::queryFeaturesCompleted, this, [this](QUuid, FeatureQueryResult* featureQueryResult)
  {
    setBusy(false);

    if (!featureQueryResult)
      return;

    //create a feature collection table fromt the query results
    FeatureCollectionTable* featureCollectionTable = new FeatureCollectionTable(featureQueryResult, this);

    //create a feature collection from the above feature collection table
    FeatureCollection* featureCollection = new FeatureCollection(QList<FeatureCollectionTable*> {featureCollectionTable}, this);

    //create a feature collection layer
    FeatureCollectionLayer* featureCollectionLayer = new FeatureCollectionLayer(featureCollection, this);

    //add the layer to the operational layers array
    m_map->operationalLayers()->append(featureCollectionLayer);
  });

  m_featureTable->queryFeatures(queryParams);
  setBusy(true);
}
