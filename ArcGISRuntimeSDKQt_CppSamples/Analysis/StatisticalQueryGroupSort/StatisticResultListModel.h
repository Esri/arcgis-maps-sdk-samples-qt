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

#ifndef STATISTICRESULTISTMODEL_H
#define STATISTICRESULTISTMODEL_H

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
