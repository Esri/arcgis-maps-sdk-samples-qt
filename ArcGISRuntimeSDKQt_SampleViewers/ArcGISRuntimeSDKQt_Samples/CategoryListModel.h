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

#ifndef CATEGORYLISTMODEL_H
#define CATEGORYLISTMODEL_H

class SampleCategory;

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>

class CategoryListModel : public QAbstractListModel
{
  Q_OBJECT

public:
  enum CategoryRoles
  {
    NameRole = Qt::UserRole + 1,
    DisplayNameRole,
    PathRole,
    ThumbnailRole,
    BackgroundThumbnailRole
  };

  explicit CategoryListModel(QObject* parent = nullptr);
  ~CategoryListModel() override = default;

  Q_INVOKABLE SampleCategory* get(int index) const { return m_categories.at(index); }
  void addCategory(SampleCategory* category);
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  void setupRoles();

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  QHash<int, QByteArray> m_roles;
  QList<SampleCategory*> m_categories;
};

#endif // CATEGORYLISTMODEL_H
