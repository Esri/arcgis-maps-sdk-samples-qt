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
#include "StatisticResultListModel.h"

// Qt headers
#include <QAbstractListModel>
#include <QByteArray>
#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QObject>
#include <QVariant>

StatisticResultListModel::StatisticResultListModel(QObject* parent) :
  QAbstractListModel(parent)
{
  setupRoles();
}

void StatisticResultListModel::setupRoles()
{
  m_roles[SectionRole] = "section";
  m_roles[StatisticRole] = "statistic";
}

void StatisticResultListModel::addStatisticResult(const QString& section, const QString& statistic)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  StatisticResult result(section, statistic);
  m_results.append(result);
  endInsertRows();
}

int StatisticResultListModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return m_results.count();
}

QVariant StatisticResultListModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= m_results.count())
    return QVariant();

  StatisticResult result = m_results[index.row()];

  QVariant retVal;

  switch (role)
  {
  case SectionRole:
    retVal = result.section;
    break;
  case StatisticRole:
    retVal = result.statistic;
    break;
  default:
    break;
  }

  return retVal;
}

QHash<int, QByteArray> StatisticResultListModel::roleNames() const
{
  return m_roles;
}

void StatisticResultListModel::clear()
{
  beginResetModel();
  m_results.clear();
  endResetModel();
}

// c'tor for StatisticResult struct
StatisticResultListModel::StatisticResult::StatisticResult(const QString& section, const QString& statistic):
  section(section),
  statistic(statistic)
{
}
