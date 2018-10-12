// [WriteFile Name=DisplayLayerViewDrawStatus, Category=Maps]
// [Legal]
// Copyright 2016 Esri.

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
#include "DrawStatusModel.h"

DrawStatusModel::DrawStatusModel(QObject *parent) :
  QAbstractListModel(parent)
{

}

DrawStatusModel::~DrawStatusModel() =default;

int DrawStatusModel::rowCount(const QModelIndex &parent) const
{
  if (parent == QModelIndex())
  {
    return m_layerStatuses.size();
  }
  return 0;
}

QVariant DrawStatusModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid()) return QVariant();
  if (index.column() != 0) return QVariant();
  if (index.row() < 0) return QVariant();
  if (index.row() >= m_layerStatuses.size()) return QVariant();

  if (role == StatusRoles::nameRole)
  {
    return m_layerStatuses.at(index.row()).name;
  }
  else if (role == StatusRoles::statusRole)
  {
    return m_layerStatuses.at(index.row()).status;
  }
  else
  {
    return QVariant();
  }
}

QVariant DrawStatusModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Orientation::Vertical) return QVariant();
  if (section > 0) return QVariant();

  if (role == StatusRoles::nameRole)
  {
    return "Name";
  }
  else if (role == StatusRoles::statusRole)
  {
    return "Status";
  }
  else
  {
    return QVariant();
  }
}

QHash<int, QByteArray> DrawStatusModel::roleNames() const
{
  return { { StatusRoles::nameRole, "name" },
           { StatusRoles::statusRole , "status" }
         };
}

int DrawStatusModel::addStatus(const QString& name, const QString& status)
{
  const int newIndex = m_layerStatuses.size();
  emit beginInsertRows(QModelIndex(), newIndex, newIndex);
  m_layerStatuses.push_back({ name, status });
  emit endInsertRows();
  return newIndex;
}

void DrawStatusModel::setNameAt(int i, const QString& name)
{
  if (i >= m_layerStatuses.size()) return;
  if (i < 0) return;

  m_layerStatuses[i].name = name;
  emit dataChanged(index(i, 0), index(i, 0), { StatusRoles::nameRole });
}


void DrawStatusModel::setStatusAt(int i, const QString& status)
{
  if (i >= m_layerStatuses.size()) return;
  if (i < 0) return;

  m_layerStatuses[i].status = status;
  emit dataChanged(index(i, 0), index(i, 0), { StatusRoles::statusRole });
}


