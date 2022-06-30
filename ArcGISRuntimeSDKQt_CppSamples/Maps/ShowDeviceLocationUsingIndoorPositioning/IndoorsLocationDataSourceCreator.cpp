#include "IndoorsLocationDataSourceCreator.h"

#include "FeatureTableListModel.h"
#include "FeatureTable.h"
#include "FeatureLayer.h"
#include "ArcGISFeatureTable.h"
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
    connect(map, &Map::doneLoading, this, [=]()
    {
      createIndoorsLocationDataSource(map, positioningTableName, pathwaysTableName, globalIdSortNames);
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
        qDebug() << (int)m_pathwaysTable->featureTableType();
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
    qDebug() << "global id - queryFeaturesCompleted";
    // Delete rawFeatureQueryResult pointer when we leave scope.
    auto featureQueryResult = std::unique_ptr<FeatureQueryResult>(rawFeatureQueryResult);

    FeatureIterator iter = featureQueryResult->iterator();
    if (iter.hasNext())
    {
      Feature* feat = iter.next();
      m_globalId = feat->attributes()->attributesMap().value("GlobalID").toUuid();
      if (m_pathwaysTable && m_positioningTable && !m_globalId.isNull())
        returnIndoorsLocationDataSource();
      return;
    }
  });
  m_positioningTable->queryFeatures(queryParameters);
}

void IndoorsLocationDataSourceCreator::returnIndoorsLocationDataSource()
{
  emit createIndoorsLocationDataSourceCompleted(new IndoorsLocationDataSource(m_positioningTable, m_pathwaysTable, /*m_globalId,*/ this));
}
