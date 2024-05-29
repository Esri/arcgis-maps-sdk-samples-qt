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

#ifndef DATAITEMLISTMODEL_H
#define DATAITEMLISTMODEL_H

class DataItem;

#include "Iterable.h"

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>

class DataItemListModel : public QAbstractListModel, public Esri::ArcGISRuntime::Iterable<DataItem*>
{
  Q_OBJECT

public:
  Q_PROPERTY(int size READ size NOTIFY sizeChanged)

  enum CategoryRoles
  {
    ItemIdRole = Qt::UserRole + 1,
    PathRole,
    SizeRole
  };

  explicit DataItemListModel(QObject* parent = nullptr);
  ~DataItemListModel() override = default;

  Q_INVOKABLE DataItem* at(int index) const override { return m_dataItems.at(index); }
  void addDataItem(DataItem* dataItem);
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  int size() const override { return m_dataItems.size(); }
  void setupRoles();

signals:
  void sizeChanged();

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  QHash<int, QByteArray> m_roles;
  QList<DataItem*> m_dataItems;
};

#endif // DATAITEMLISTMODEL_H
