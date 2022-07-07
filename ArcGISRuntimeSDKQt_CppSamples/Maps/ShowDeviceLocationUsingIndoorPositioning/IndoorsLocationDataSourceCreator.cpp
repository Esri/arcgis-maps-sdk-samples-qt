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

void IndoorsLocationDataSourceCreator::createIndoorsLocationDataSource(Map* map, const QString &positioningTableName, const QString &pathwaysTableName, const QUuid &globalId)
{
  if (map->loadStatus() != LoadStatus::Loaded)
  {
    connect(map, &Map::doneLoading, this, [=]()
    {
      createIndoorsLocationDataSource(map, positioningTableName, pathwaysTableName, globalId);
      map->disconnect();
    });
    return;
  }

  m_map = map;

  m_positioningTableName = positioningTableName;
  m_pathwaysTableName = pathwaysTableName;
  m_globalId = globalId;

  findPositioningTable();
  findPathwaysTable();
}

// An IPS positioning feature table is stored with an IPS-enabled map. Each row in the table contains an indoor positioning file.
// The IndoorsLocationDataSource will use the most recently created row unless given an alternative GlobalId in the constructor.
void IndoorsLocationDataSourceCreator::findPositioningTable()
{
  FeatureTableListModel* tables = m_map->tables();

  for (FeatureTable* table : *tables)
  {
    connect(table, &FeatureTable::doneLoading, this, [table, this]()
    {
      if (table->tableName() == m_positioningTableName)
      {
        m_positioningTable = table;

        if (m_pathwaysTable && m_positioningTable)
          returnIndoorsLocationDataSource();
      }
    });
    table->load();
  }
}

// The pathways table is an ArcGISFeatureTable with line features that represent paths through the indoor space.
// Locations provided by the IndoorsLocationDataSource are snapped to the lines in this feature class.
void IndoorsLocationDataSourceCreator::findPathwaysTable()
{
  LayerListModel* layers = m_map->operationalLayers();

  for (Layer* layer : *layers)
  {
    if(FeatureLayer* featureLayer = dynamic_cast<FeatureLayer*>(layer))
    {
      if (featureLayer->name() == m_pathwaysTableName)
      {
        m_pathwaysTable = dynamic_cast<ArcGISFeatureTable*>(featureLayer->featureTable());
        if (m_pathwaysTable && m_positioningTable)
          returnIndoorsLocationDataSource();
        return;
      }
    }
  }
}

void IndoorsLocationDataSourceCreator::returnIndoorsLocationDataSource()
{
  // The IndoorsLocationDataSource constructor takes an optional GlobalId to identify which row of the positioning table to use.
  // If not specified, the constructor will use the row from the most recent survey
  if (m_globalId.isNull())
    emit createIndoorsLocationDataSourceCompleted(new IndoorsLocationDataSource(m_positioningTable, m_pathwaysTable, this));
  else
    emit createIndoorsLocationDataSourceCompleted(new IndoorsLocationDataSource(m_positioningTable, m_pathwaysTable, m_globalId, this));
}
