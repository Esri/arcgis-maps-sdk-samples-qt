// [WriteFile Name=FeatureLayerQuery, Category=Features]
// [Legal]
// Copyright 2016 Esri.
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

#endif // PCH_BUILD
#ifdef PCH_BUILD
#include "pch.hpp"

// sample headers
#include "FeatureLayerQuery.h"

// C++ API headers
#include "Basemap.h"
#include "Feature.h"
#include "FeatureIterator.h"
#include "FeatureLayer.h"
#include "FeatureQueryResult.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "MapViewTypes.h"
#include "Point.h"
#include "QueryParameters.h"
#include "ServiceFeatureTable.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleRenderer.h"
#include "SpatialReference.h"
#include "SymbolTypes.h"
#include "Viewpoint.h"

// Qt headers
#include <QColor>
#include <QFuture>
#include <QList>
#include <QUrl>
#include <QUuid>

// STL headers
#include <memory>

using namespace Esri::ArcGISRuntime;

FeatureLayerQuery::FeatureLayerQuery(QQuickItem* parent) :
  QQuickItem(parent)
{
}

FeatureLayerQuery::~FeatureLayerQuery() = default;

void FeatureLayerQuery::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<FeatureLayerQuery>("Esri.Samples", 1, 0, "FeatureLayerQuerySample");
}

void FeatureLayerQuery::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create a map using the topographic basemap
  m_map = new Map(BasemapStyle::ArcGISTopographic, this);
  m_map->setInitialViewpoint(Viewpoint(Point(-11e6, 5e6, SpatialReference(102100)), 9e7));

  // Set map to map view
  m_mapView->setMap(m_map);

  // create the feature table
  m_featureTable = new ServiceFeatureTable(QUrl("https://services.arcgis.com/jIL9msH9OI208GCb/arcgis/rest/services/USA_Daytime_Population_2016/FeatureServer/0"), this);
  // create the feature layer using the feature table
  m_featureLayer = new FeatureLayer(m_featureTable, this);

  // line symbol for the outline
  SimpleLineSymbol* outline = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("black"), 2.0f, this);
  // fill symbol
  SimpleFillSymbol* sfs = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor(255, 255, 0, 153), outline, this);
  // create the renderer using the symbology created above
  SimpleRenderer* renderer = new SimpleRenderer(sfs, this);
  // set the renderer for the feature layer
  m_featureLayer->setRenderer(renderer);
  m_featureLayer->setMaxScale(10000);

  // add the feature layer to the map
  m_map->operationalLayers()->append(m_featureLayer);

  connect(m_featureTable, &ServiceFeatureTable::loadStatusChanged, this, [this](LoadStatus loadStatus)
  {
    loadStatus == LoadStatus::Loaded ? m_initialized = true : m_initialized = false;
    emit layerInitializedChanged();
  });
}

bool FeatureLayerQuery::layerInitialized() const
{
  return m_initialized;
}

void FeatureLayerQuery::runQuery(const QString& stateName)
{
  // create a query parameter object and set the where clause
  QueryParameters queryParams;
  queryParams.setWhereClause(QString("STATE_NAME LIKE '" + formatStateNameForQuery(stateName) + "%'"));
  m_featureTable->queryFeaturesAsync(queryParams).then(this, [this](FeatureQueryResult* rawQueryResult)
  {
    auto queryResult = std::unique_ptr<FeatureQueryResult>(rawQueryResult);

    if (queryResult && !queryResult->iterator().hasNext())
    {
      m_queryResultsCount = 0;
      emit queryResultsCountChanged();
      return;
    }

    // clear any existing selection
    m_featureLayer->clearSelection();
    QList<Feature*> features;

    // iterate over the result object
    while (queryResult->iterator().hasNext())
    {
      Feature* feature = queryResult->iterator().next(this);
      // add each feature to the list
      features.append(feature);
    }

    // select the feature
    m_featureLayer->selectFeatures(features);
    // zoom to the first feature
    m_mapView->setViewpointGeometryAsync(features.at(0)->geometry(), 30);
    // set the count for QML property
    m_queryResultsCount = static_cast<int>(features.count());
    emit queryResultsCountChanged();
  });
}

QString FeatureLayerQuery::formatStateNameForQuery(const QString& stateName) const
{
  // format state names as expected by the service, for instance "Rhode Island"
  if (stateName.isEmpty())
    return QString();

  const QStringList words = stateName.split(" ", Qt::SkipEmptyParts);
  QStringList formattedWords;

  for (const QString& word : words)
  {
    QString formattedWord = word.toLower();
    formattedWord[0] = formattedWord[0].toUpper();
    formattedWords.append(formattedWord);
  }

  return QString(formattedWords.join(" "));
}

int FeatureLayerQuery::queryResultsCount() const
{
  return m_queryResultsCount;
}
