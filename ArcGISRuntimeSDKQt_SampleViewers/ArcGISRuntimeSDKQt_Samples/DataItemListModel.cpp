// [Legal]
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
// [Legal]

#include "pch.hpp"

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QModelIndex>
#include <QObject>
#include <QVariant>

#include "DataItemListModel.h"
#include "DataItem.h"

DataItemListModel::DataItemListModel(QObject* parent) :
  QAbstractListModel(parent)
{
  setupRoles();
}

void DataItemListModel::setupRoles()
{
  m_roles[ItemIdRole] = "itemId";
  m_roles[PathRole] = "path";
  m_roles[SizeRole] = "size";
}

void DataItemListModel::addDataItem(DataItem* dataItem)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  m_dataItems << dataItem;
  endInsertRows();
  emit sizeChanged();
}
int DataItemListModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return m_dataItems.count();
}

QVariant DataItemListModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= m_dataItems.count())
    return QVariant();

  DataItem* dataItem = m_dataItems[index.row()];
  QVariant retVal;

  switch (role)
  {
  case ItemIdRole:
    retVal = dataItem->itemId();
    break;
  case PathRole:
    retVal = dataItem->path();
    break;
  case SizeRole:
    retVal = m_dataItems.length();
    break;
  default:
    break;
  }

  return retVal;
}

QHash<int, QByteArray> DataItemListModel::roleNames() const
{
  return m_roles;
}
