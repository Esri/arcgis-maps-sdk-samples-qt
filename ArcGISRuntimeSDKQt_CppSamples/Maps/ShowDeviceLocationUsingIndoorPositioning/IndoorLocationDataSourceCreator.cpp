#include "IndoorLocationDataSourceCreator.h"

#include "FeatureTableListModel.h"
#include "FeatureTable.h"
#include "FeatureLayer.h"
#include "ArcGISFeatureTable.h"
#include "IndoorsLocationDataSource.h"
#include "Map.h"

using namespace Esri::ArcGISRuntime;

IndoorLocationDataSourceCreator::IndoorLocationDataSourceCreator(QObject* parent /* = nullptr */):
  QObject(parent)
{
}

IndoorLocationDataSourceCreator::~IndoorLocationDataSourceCreator() = default;

void IndoorLocationDataSourceCreator::createIndoorLocationDataSource(Map* map, const QString &positioningTableName, const QString &pathwaysTableName, const QStringList &globalIdSortNames)
{
  if (map->loadStatus() != LoadStatus::Loaded)
  {
    connect(m_map, &Map::doneLoading, this, [&]()
    {
      createIndoorLocationDataSource(m_map, positioningTableName, pathwaysTableName, globalIdSortNames);
    });
    return;
  }

  m_map = map;

  m_positioningTableName = positioningTableName;
  m_pathwaysTableName = pathwaysTableName;
  m_globalIdSortNames = globalIdSortNames;

  findPositioningTable();
  findPathwaysTable();
  findGlobalId();
}

void IndoorLocationDataSourceCreator::findPositioningTable()
{
  FeatureTableListModel* tables = m_map->tables();

  for (FeatureTable* table : *tables)
  {
    connect(table, &FeatureTable::doneLoading, this, [table, this]()
    {
      if (table->tableName() == m_positioningTableName)
      {
        m_positioningTable = table;
        if (m_pathwaysTable && m_positioningTable && !m_globalId.isNull())
          returnIndoorLocationDataSource();
      }
    });
    table->load();
  }
}

void IndoorLocationDataSourceCreator::findPathwaysTable()
{
  LayerListModel* layers = m_map->operationalLayers();
  for (Layer* layer : *layers)
  {
    if(FeatureLayer* featureLayer = dynamic_cast<FeatureLayer*>(layer))
    {
      if (featureLayer->name() == m_pathwaysTableName)
      {
        m_pathwaysTable = dynamic_cast<ArcGISFeatureTable*>(featureLayer->featureTable());
        if (m_pathwaysTable && m_positioningTable && !m_globalId.isNull())
          returnIndoorLocationDataSource();
      }
    }
  }
}

void IndoorLocationDataSourceCreator::findGlobalId()
{
  const QList<Field> fields = m_positioningTable->fields();
  Field dateCreatedField;

  for (const Field &field : fields)
  {
    if (m_globalIdSortNames.contains(field.name(), Qt::CaseInsensitive))
      dateCreatedField = field;
  }

  if (dateCreatedField.isEmpty())
  {
    qWarning() << "Date created field not found";
    return;
  }

  QueryParameters queryParameters;
  queryParameters.setMaxFeatures(1);
  queryParameters.setWhereClause("1=1");
  queryParameters.orderByFields().append(OrderBy(dateCreatedField.name(), SortOrder::Descending));

  connect(m_positioningTable, &FeatureTable::queryFeaturesCompleted, this, [this](QUuid, FeatureQueryResult* rawFeatureQueryResult)
  {
    // Delete rawFeatureQueryResult pointer when we leave scope.
    auto featureQueryResult = std::unique_ptr<FeatureQueryResult>(rawFeatureQueryResult);

    FeatureIterator iter = featureQueryResult->iterator();
    if (iter.hasNext())
    {
      Feature* feat = iter.next();
      m_globalId = feat->attributes()->attributesMap().value("GlobalID").toUuid();
      if (m_pathwaysTable && m_positioningTable && !m_globalId.isNull())
        returnIndoorLocationDataSource();
    }
  });
  m_positioningTable->queryFeatures(queryParameters);
}

void IndoorLocationDataSourceCreator::returnIndoorLocationDataSource()
{
  emit createIndoorLocationDataSourceCompleted(new IndoorsLocationDataSource(m_positioningTable, m_pathwaysTable, m_globalId, this));
}
