// Copyright 2016 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QModelIndex>
#include <QObject>
#include <QVariant>
#include <QFileInfo>
#include <QDir>
#include <QList>

#include "StatisticResultListModel.h"

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
