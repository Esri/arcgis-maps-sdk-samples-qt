// [Legal]
// Copyright 2025 Esri.
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

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SNAPSOURCESLISTMODEL_H
#define SNAPSOURCESLISTMODEL_H

// Qt headers
#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>

namespace Esri::ArcGISRuntime {
  class SnapSourceSettings;
}

class SnapSourcesListModel : public QAbstractListModel
{
  Q_OBJECT

public:
  enum SnapSourceRoles 
  {
    NameRole = Qt::UserRole + 1,
    IsEnabledRole
  };

  explicit SnapSourcesListModel(QObject* parent = nullptr);
  ~SnapSourcesListModel() override = default;

public:
  void setSnapSourceSettings(QList<Esri::ArcGISRuntime::SnapSourceSettings*> snapSourceSettings);
  QList<Esri::ArcGISRuntime::SnapSourceSettings*> snapSourceSettings() const;
  void setupRoles();
  int size() const;
  void clear();

  // QAbstractItemModel interface
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  QHash<int, QByteArray> m_roles;
  QList<Esri::ArcGISRuntime::SnapSourceSettings*> m_snapSourceSettings;
  QString determineName(Esri::ArcGISRuntime::SnapSourceSettings* snapSourceSettings) const;
};

#endif // SNAPSOURCESLISTMODEL_H
