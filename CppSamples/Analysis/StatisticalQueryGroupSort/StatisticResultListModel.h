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

#ifndef STATISTICRESULTISTMODEL_H
#define STATISTICRESULTISTMODEL_H

// Qt headers
#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>

class StatisticResultListModel : public QAbstractListModel
{
  Q_OBJECT

public:
  enum StatisticResultRoles
  {
    SectionRole = Qt::UserRole + 1,
    StatisticRole
  };

  explicit StatisticResultListModel(QObject* parent = nullptr);
  ~StatisticResultListModel() override = default;

public:
  void addStatisticResult(const QString& section, const QString& statistic);
  void clear();
  void setupRoles();
  int size() { return m_results.size(); }

  // QAbstractItemModel interface
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  // Struct to keep track of the section and statistic strings
  struct StatisticResult
  {
  public:
    StatisticResult(const QString& section, const QString& statistic);
    ~StatisticResult() = default;

    QString section;
    QString statistic;
  };

  QHash<int, QByteArray> m_roles;
  QList<StatisticResult> m_results;
};

#endif // STATISTICRESULTISTMODEL_H
