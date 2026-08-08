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

#ifndef PERFMETRICSMODEL_H
#define PERFMETRICSMODEL_H

// Qt headers
#include <QAbstractListModel>

// Ordered label -> value rows behind the overlay's Repeater; internal to the perf monitor module.
class PerfMetricsModel : public QAbstractListModel
{
public:
  enum class Role : int
  {
    Label = Qt::UserRole + 1,
    Value,
  };

  using QAbstractListModel::QAbstractListModel;

  int rowCount(const QModelIndex& parent) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  void upsert(const QString& label, double value);
  void clear();

private:
  struct Row
  {
    QString label;
    double value;
  };

  QList<Row> m_rows;
};

#endif // PERFMETRICSMODEL_H
