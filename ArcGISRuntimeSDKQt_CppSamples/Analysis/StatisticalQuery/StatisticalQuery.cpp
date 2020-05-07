// [WriteFile Name=StatisticalQuery, Category=Analysis]
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

#include "StatisticalQuery.h"

#include "Map.h"
#include "MapQuickView.h"
#include "ArcGISVectorTiledLayer.h"
#include "FeatureLayer.h"
#include "ServiceFeatureTable.h"
#include "StatisticalQuery.h"
#include "StatisticsQueryParameters.h"
#include "StatisticDefinition.h"
#include "StatisticRecord.h"
#include "StatisticRecordIterator.h"
#include "Viewpoint.h"

#include <memory>
#include <QUrl>

using namespace Esri::ArcGISRuntime;

StatisticalQuery::StatisticalQuery(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void StatisticalQuery::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<StatisticalQuery>("Esri.Samples", 1, 0, "StatisticalQuerySample");
}

void StatisticalQuery::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a new Map with the world streets vector basemap
  m_map = new Map(Basemap::streetsVector(this), this);

  // Create feature table using the world cities URL
  m_featureTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/SampleWorldCities/MapServer/0"), this);

  // Create a new feature layer to display features in the world cities table
  FeatureLayer* featureLayer = new FeatureLayer(m_featureTable, this);
  m_map->operationalLayers()->append(featureLayer);

  // Set map to map view
  m_mapView->setMap(m_map);

  connect(m_featureTable, &ServiceFeatureTable::errorOccurred, [this](Error e)
  {
    if (e.isEmpty())
      return;

    emit showStatistics(e.message());
  });

  // connect to queryStatisticsCompleted
  connect(m_featureTable, &ServiceFeatureTable::queryStatisticsCompleted, this, [this](QUuid, StatisticsQueryResult* rawResult)
  {
    if (!rawResult)
      return;

    // Delete rawResult when we leave local scope.
    auto result = std::unique_ptr<StatisticsQueryResult>(rawResult);

    // Iterate through the results
    QObject parent;
    QString resultText;
    StatisticRecordIterator iter = result->iterator();
    while (iter.hasNext())
    {
      StatisticRecord* record = iter.next(&parent);
      const QVariantMap& statsMap = record->statistics();
      for (auto it = statsMap.cbegin(); it != statsMap.cend(); ++it)
      {
        resultText += QString("%1: %2\n").arg(it.key(), it.value().toString());
      }
    }

    // Display the Results
    emit showStatistics(resultText);
  });
}

void StatisticalQuery::queryStatistics(bool extentOnly, bool bigCitiesOnly)
{
  // create the parameters
  StatisticsQueryParameters queryParameters;

  // Add the Statistic Definitions
  QList<StatisticDefinition> definitions
  {
    StatisticDefinition("POP", StatisticType::Average, ""),
    StatisticDefinition("POP", StatisticType::Minimum, ""),
    StatisticDefinition("POP", StatisticType::Maximum, ""),
    StatisticDefinition("POP", StatisticType::Sum, ""),
    StatisticDefinition("POP", StatisticType::StandardDeviation, ""),
    StatisticDefinition("POP", StatisticType::Variance, ""),
    StatisticDefinition("POP", StatisticType::Count, "CityCount")
  };
  queryParameters.setStatisticDefinitions(definitions);

  // If only using features in the current extent, set up the spatial filter for the statistics query parameters
  if (extentOnly)
  {
    // Set the statistics query parameters geometry with the envelope
    queryParameters.setGeometry(m_mapView->currentViewpoint(ViewpointType::BoundingGeometry).targetGeometry());

    // Set the spatial relationship to Intersects (which is the default)
    queryParameters.setSpatialRelationship(SpatialRelationship::Intersects);
  }

  // If only evaluating the largest cities (over 5 million in population), set up an attribute filter
  if (bigCitiesOnly)
    queryParameters.setWhereClause("POP_RANK = 1");

  // Execute the statistical query with these parameters
  m_featureTable->queryStatistics(queryParameters);
}
