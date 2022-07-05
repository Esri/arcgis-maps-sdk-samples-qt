// Copyright 2022 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "IndoorsLocationDataSourceCreator.h"

#include "ArcGISFeatureTable.h"
#include "FeatureLayer.h"
#include "IndoorsLocationDataSource.h"
#include "Map.h"

using namespace Esri::ArcGISRuntime;

IndoorsLocationDataSourceCreator::IndoorsLocationDataSourceCreator(QObject* parent /* = nullptr */):
  QObject(parent)
{
}

IndoorsLocationDataSourceCreator::~IndoorsLocationDataSourceCreator() = default;

void IndoorsLocationDataSourceCreator::createIndoorsLocationDataSource(Map* map, const QString &positioningTableName, const QString &pathwaysTableName, const QStringList &globalIdSortNames)
{
  qDebug() << "createIndoorsLocationDataSource";
  if (map->loadStatus() != LoadStatus::Loaded)
  {
    qDebug() << "map is not done loading, retrying when done loading";
    connect(map, &Map::doneLoading, this, [=]()
    {
      createIndoorsLocationDataSource(map, positioningTableName, pathwaysTableName, globalIdSortNames);
      map->disconnect();
    });
    return;
  }

  m_map = map;

  m_positioningTableName = positioningTableName;
  m_pathwaysTableName = pathwaysTableName;
  m_globalIdSortNames = globalIdSortNames;

  findPositioningTable();
  findPathwaysTable();
}

void IndoorsLocationDataSourceCreator::findPositioningTable()
{
  qDebug() << "findPositioningTable";
  FeatureTableListModel* tables = m_map->tables();

  for (FeatureTable* table : *tables)
  {
    connect(table, &FeatureTable::doneLoading, this, [table, this]()
    {
      qDebug() << "table done loading";
      if (table->tableName() == m_positioningTableName)
      {
        qDebug() << "positioning table found ";
        m_positioningTable = table;
        // Finding the GlobalId requires the positioning table
        findGlobalId();
      }
    });
    table->load();
  }
}

void IndoorsLocationDataSourceCreator::findPathwaysTable()
{
  qDebug() << "findPathwaysTable";
  LayerListModel* layers = m_map->operationalLayers();

  for (Layer* layer : *layers)
  {
    if(FeatureLayer* featureLayer = dynamic_cast<FeatureLayer*>(layer))
    {
      if (featureLayer->name() == m_pathwaysTableName)
      {
        qDebug() << "pathways table found";
        m_pathwaysTable = dynamic_cast<ArcGISFeatureTable*>(featureLayer->featureTable());
        if (m_pathwaysTable && m_positioningTable && !m_globalId.isNull())
          returnIndoorsLocationDataSource();
        return;
      }
    }
  }
}

void IndoorsLocationDataSourceCreator::findGlobalId()
{
  qDebug() << "findGlobalId";
  const QList<Field> fields = m_positioningTable->fields();
  Field dateCreatedField;

  for (const Field &field : fields)
  {
    if (m_globalIdSortNames.contains(field.name(), Qt::CaseInsensitive))
    {
      qDebug() << "Found global id sort field" << field.name();
      dateCreatedField = field;
    }
  }

  if (dateCreatedField.isEmpty())
  {
    qWarning() << "Date created field not found";
    return;
  }

  QueryParameters queryParameters;
  queryParameters.setMaxFeatures(1);
  queryParameters.setWhereClause("1=1");
  queryParameters.setOrderByFields({OrderBy(dateCreatedField.name(), SortOrder::Descending)});

  connect(m_positioningTable, &FeatureTable::queryFeaturesCompleted, this, [this](QUuid, FeatureQueryResult* rawFeatureQueryResult)
  {
    qDebug() << "global id - queryFeaturesCompleted";
    // Delete rawFeatureQueryResult pointer when we leave scope.
    auto featureQueryResult = std::unique_ptr<FeatureQueryResult>(rawFeatureQueryResult);

    FeatureIterator iter = featureQueryResult->iterator();
    if (iter.hasNext())
    {
      Feature* feat = iter.next();
      m_globalId = feat->attributes()->attributesMap().value("GlobalID").toUuid();
      qDebug() << "Found global id" << m_globalId;
      if (m_pathwaysTable && m_positioningTable && !m_globalId.isNull())
        returnIndoorsLocationDataSource();
      return;
    }
  });
  m_positioningTable->queryFeatures(queryParameters);
}

void IndoorsLocationDataSourceCreator::returnIndoorsLocationDataSource()
{
  emit createIndoorsLocationDataSourceCompleted(new IndoorsLocationDataSource(m_positioningTable, m_pathwaysTable, m_globalId, this));
}
