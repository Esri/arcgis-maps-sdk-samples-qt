// [WriteFile Name=StatisticalQueryGroupSort, Category=Analysis]
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

#include "StatisticalQueryGroupSort.h"
#include "ServiceFeatureTable.h"
#include "StatisticDefinition.h"
#include "StatisticRecord.h"
#include "StatisticRecordIterator.h"
#include "StatisticsQueryParameters.h"
#include "StatisticsQueryResult.h"
#include "Field.h"

#include "StatisticResultListModel.h"

#include <QStringList>
#include <QVariantList>
#include <QList>
#include <QScopedPointer>

using namespace Esri::ArcGISRuntime;

StatisticalQueryGroupSort::StatisticalQueryGroupSort(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_resultsModel(new StatisticResultListModel(this))
{
}

void StatisticalQueryGroupSort::init()
{
  qmlRegisterType<StatisticalQueryGroupSort>("Esri.Samples", 1, 0, "StatisticalQueryGroupSortSample");
}

void StatisticalQueryGroupSort::componentComplete()
{
  QQuickItem::componentComplete();

  // Create the Service Feature Table
  m_featureTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/Census/MapServer/3"), this);
  connectSignals();
  m_featureTable->load();

  // Setup default values for the Options page
  addStatisticDefinition("POP2007", "Sum");
  addStatisticDefinition("POP2007", "Average");
  addStatisticDefinition("AGE_5_17", "Minimum");
  addOrderBy("SUB_REGION", "Ascending");
  m_statisticTypes << "Average" << "Count" << "Maximum" << "Minimum"
                   << "Standard Deviation" << "Sum" << "Variance";
  emit statisticTypesChanged();
}

void StatisticalQueryGroupSort::connectSignals()
{
  connect(m_featureTable, &ServiceFeatureTable::doneLoading, this, [this](Error error)
  {
    if (!error.isEmpty())
      return;

    for (Field field : m_featureTable->fields())
    {
      m_fields << field.name();
    }
    emit fieldsChanged();
  });

  connect(m_featureTable, &ServiceFeatureTable::errorOccurred, this, [this](Error error)
  {
    if (error.isEmpty())
      return;

    m_resultsModel->clear();
    addResultToModel("", QString("Error. %1").arg(error.message()));
  });

  connect(m_featureTable, &ServiceFeatureTable::queryStatisticsCompleted, this, [this](QUuid, StatisticsQueryResult* rawResult)
  {
    // Delete rawResult when we leave local scope.
    QScopedPointer<StatisticsQueryResult> result(rawResult);

    if (!result)    
      return;    

    // clear previous results
    m_resultsModel->clear();

    // iterate the results and add to a model
    StatisticRecordIterator iter = result->iterator();
    while (iter.hasNext())
    {
      // get the statistic record
      StatisticRecord* record = iter.next(result.data());

      // get the group string
      QStringList sectionStrings;
      const QVariantMap& groupsMap = record->group();
      for (auto it = groupsMap.cbegin(); it != groupsMap.cend(); ++it)
      {
          sectionStrings << QString("\"%1\":\"%2\"").arg(it.key(), it.value().toString());
      }
      const QString sectionString = sectionStrings.join(',');

      // obtain the statistics
      const QVariantMap& statsMap = record->statistics();
      for (auto it = statsMap.cbegin(); it != statsMap.cend(); ++it)
      {
        const QString statString = QString("%1: %2").arg(it.key(), it.value().toString());
        addResultToModel(sectionString, statString);
      }
    }
  });
}

void StatisticalQueryGroupSort::queryStatistics()
{
  // create the parameter object
  StatisticsQueryParameters params;

  // add the statistic definitions
  QList<StatisticDefinition> statisticDefinitionList;
  for (QVariant def : m_statisticDefinitions)
  {
    QVariantMap definitionMap = def.toMap();
    StatisticDefinition statisticDefinition;
    statisticDefinition.setOnFieldName(definitionMap["field"].toString());
    statisticDefinition.setStatisticType(statisticStringToEnum(definitionMap["statistic"].toString()));
    statisticDefinitionList.append(statisticDefinition);
  }
  params.setStatisticDefinitions(statisticDefinitionList);

  // set the grouping fields
  params.setGroupByFieldNames(groupingFields());

  // add the order by objects
  QList<OrderBy> orderByList;
  for (QVariant ob : m_orderBys)
  {
    QVariantMap orderByMap = ob.toMap();
    OrderBy orderBy;
    orderBy.setFieldName(orderByMap["field"].toString());
    orderBy.setSortOrder(orderStringToEnum(orderByMap["order"].toString()));
    orderByList.append(orderBy);
  }
  params.setOrderByFields(orderByList);

  // execute the query
  m_featureTable->queryStatistics(params);
}

void StatisticalQueryGroupSort::addStatisticDefinition(const QString& field, const QString& statistic)
{
  // only add if the definition does not already exist
  for (QVariant def : m_statisticDefinitions)
  {
    QVariantMap definitionMap = def.toMap();
    if (field == definitionMap["field"].toString())
    {
      if (statistic == definitionMap["statistic"].toString())
        return;
    }
  }

  // add the definition to the list
  QVariantMap statisticDefinition;
  statisticDefinition["field"] = field;
  statisticDefinition["statistic"] = statistic;
  m_statisticDefinitions.append(statisticDefinition);
  emit statisticDefinitionsChanged();
}

void StatisticalQueryGroupSort::removeStatisticDefinition(int index)
{
  m_statisticDefinitions.removeAt(index);
  emit statisticDefinitionsChanged();
}

void StatisticalQueryGroupSort::addOrderBy(const QString& field, const QString& order)
{
  // only add if the orderby is not in the list
  for (QVariant orderBy : m_orderBys)
  {
    QVariantMap orderByMap = orderBy.toMap();
    if (field == orderByMap["field"].toString())
    {
      if (order == orderByMap["order"].toString())
      {
        emit orderBysChanged();
        return;
      }
    }
  }

  // add the order by to the list
  QVariantMap orderBy;
  orderBy["field"] = field;
  orderBy["order"] = order;
  m_orderBys.append(orderBy);
  emit orderBysChanged();
}

void StatisticalQueryGroupSort::removeOrderBy(const QString& field)
{
  // remove the order by if it exists in the list
  for (int i = 0; i < m_orderBys.size(); i++)
  {
    QVariantMap orderByMap = m_orderBys.at(i).toMap();
    if (field == orderByMap["field"].toString())
    {
      m_orderBys.removeAt(i);
      emit orderBysChanged();
      return;
    }
  }
}

void StatisticalQueryGroupSort::removeOrderBy(int index)
{
  // remove the order by at a given index
  m_orderBys.removeAt(index);
  emit orderBysChanged();
}

void StatisticalQueryGroupSort::updateOrder(int index)
{
  // get the existing values
  QVariantMap orderByMap;
  orderByMap["field"] = m_orderBys.at(index).toMap()["field"].toString();
  const QString currentOrder = m_orderBys.at(index).toMap()["order"].toString();

  // update the order text
  if (currentOrder == "Ascending")
    orderByMap["order"] = "Descending";
  else
    orderByMap["order"] = "Ascending";

  // update the variant list with the new value
  m_orderBys.removeAt(index);
  m_orderBys.insert(index, orderByMap);

  emit orderBysChanged();
}

void StatisticalQueryGroupSort::addGroupingField(const QString& field)
{
  // add a new grouping field
  if (!m_groupingFields.contains(field))
  {
    m_groupingFields.append(field);
    emit groupingFieldsChanged();
  }
}

void StatisticalQueryGroupSort::removeGroupingField(const QString& field)
{
  // remove a grouping field
  if (m_groupingFields.contains(field))
  {
    int i = m_groupingFields.indexOf(field);
    m_groupingFields.removeAt(i);
    emit groupingFieldsChanged();

    // also update the orderby list
    removeOrderBy(field);
  }
}

// helper to convert from statistic type string to enum
StatisticType StatisticalQueryGroupSort::statisticStringToEnum(const QString& statistic) const
{
  if (statistic == "Average")
    return StatisticType::Average;
  else if (statistic == "Count")
    return StatisticType::Count;
  else if (statistic == "Maximum")
    return StatisticType::Maximum;
  else if (statistic == "Minimum")
    return StatisticType::Minimum;
  else if (statistic == "Standard Deviation")
    return StatisticType::StandardDeviation;
  else if (statistic == "Sum")
    return StatisticType::Sum;
  else
    return StatisticType::Variance;
}

// helper to convert from sort order string to enum
SortOrder StatisticalQueryGroupSort::orderStringToEnum(const QString& order) const
{
  if (order == "Ascending")
    return SortOrder::Ascending;
  else
    return SortOrder::Descending;
}

void StatisticalQueryGroupSort::addResultToModel(const QString& section, const QString& resultString)
{
  m_resultsModel->addStatisticResult(section, resultString);
  emit resultsModelChanged();
}

QAbstractListModel* StatisticalQueryGroupSort::resultsModel() const
{
  return m_resultsModel;
}
