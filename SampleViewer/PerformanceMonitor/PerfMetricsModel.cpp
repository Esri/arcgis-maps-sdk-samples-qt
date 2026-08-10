// [Legal]
// Copyright 2026 Esri.
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

// Other headers
#include "PerfMetricsModel.h"

int PerfMetricsModel::rowCount(const QModelIndex& parent) const
{
  return parent.isValid() ? 0 : static_cast<int>(m_rows.size());
}

QVariant PerfMetricsModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid() || index.row() < 0 || index.row() >= m_rows.size())
  {
    return {};
  }

  const Row& row = m_rows.at(index.row());
  if (static_cast<Role>(role) == Role::Label)
  {
    return row.label;
  }

  return static_cast<Role>(role) == Role::Value ? QVariant(row.value) : QVariant();
}

QHash<int, QByteArray> PerfMetricsModel::roleNames() const
{
  return {{static_cast<int>(Role::Label), "label"}, {static_cast<int>(Role::Value), "value"}};
}

void PerfMetricsModel::upsert(const QString& label, double value)
{
  for (int i = 0; i < static_cast<int>(m_rows.size()); ++i)
  {
    if (m_rows[i].label == label)
    {
      m_rows[i].value = value;
      emit dataChanged(index(i), index(i), {static_cast<int>(Role::Value)});
      return;
    }
  }

  const int row = static_cast<int>(m_rows.size());
  beginInsertRows(QModelIndex(), row, row);
  m_rows.append({label, value});
  endInsertRows();
}

void PerfMetricsModel::clear()
{
  if (m_rows.isEmpty())
  {
    return;
  }

  beginResetModel();
  m_rows.clear();
  endResetModel();
}
