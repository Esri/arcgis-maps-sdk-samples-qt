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

#ifndef STATISTICALQUERYGROUPSORT_H
#define STATISTICALQUERYGROUPSORT_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class ServiceFeatureTable;
    enum class SortOrder;
    enum class StatisticType;
  }
}

class StatisticResultListModel;

#include <QQuickItem>
#include <QVariantList>
#include <QStringList>
#include <QAbstractListModel>
#include <QObject>

class StatisticalQueryGroupSort : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(QStringList fields READ fields NOTIFY fieldsChanged)
  Q_PROPERTY(QVariantList statisticDefinitions READ statisticDefinitions NOTIFY statisticDefinitionsChanged)
  Q_PROPERTY(QVariantList orderBys READ orderBys NOTIFY orderBysChanged)
  Q_PROPERTY(QStringList statisticTypes READ statisticTypes NOTIFY statisticTypesChanged)
  Q_PROPERTY(QStringList groupingFields READ groupingFields NOTIFY groupingFieldsChanged)
  Q_PROPERTY(QAbstractListModel* resultsModel READ resultsModel NOTIFY resultsModelChanged)

public:
  explicit StatisticalQueryGroupSort(QQuickItem* parent = nullptr);
  ~StatisticalQueryGroupSort() override = default;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void queryStatistics();
  Q_INVOKABLE void addStatisticDefinition(const QString& field, const QString& statistic);
  Q_INVOKABLE void removeStatisticDefinition(int index);
  Q_INVOKABLE void addOrderBy(const QString& field, const QString& order);
  Q_INVOKABLE void removeOrderBy(const QString& field);
  Q_INVOKABLE void removeOrderBy(int index);
  Q_INVOKABLE void updateOrder(int index);
  Q_INVOKABLE void addGroupingField(const QString& field);
  Q_INVOKABLE void removeGroupingField(const QString& field);

signals:
  void fieldsChanged();
  void statisticDefinitionsChanged();
  void orderBysChanged();
  void statisticTypesChanged();
  void groupingFieldsChanged();
  void resultsModelChanged();

private:
  QStringList fields() const { return m_fields; }
  QVariantList statisticDefinitions() const { return m_statisticDefinitions; }
  QVariantList orderBys() const { return m_orderBys; }
  QAbstractListModel* resultsModel() const;
  QStringList statisticTypes() const { return m_statisticTypes; }
  QStringList groupingFields() const { return m_groupingFields; }
  void connectSignals();
  Esri::ArcGISRuntime::StatisticType statisticStringToEnum(const QString& statistic) const;
  Esri::ArcGISRuntime::SortOrder orderStringToEnum(const QString& order) const;
  void addResultToModel(const QString& section, const QString& resultString);

  Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable = nullptr;
  QStringList m_fields;
  QVariantList m_statisticDefinitions;
  QVariantList m_orderBys;
  QStringList m_statisticTypes;
  QStringList m_groupingFields;
  StatisticResultListModel* m_resultsModel = nullptr;
};

#endif // STATISTICALQUERYGROUPSORT_H
