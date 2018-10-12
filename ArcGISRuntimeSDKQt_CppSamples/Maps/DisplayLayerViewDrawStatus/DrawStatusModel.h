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

#ifndef DRAWSTATUSMODEL_H
#define DRAWSTATUSMODEL_H

#include <QAbstractListModel>

class DrawStatusModel : public QAbstractListModel
{
  Q_OBJECT
public:
  enum StatusRoles {
    nameRole = Qt::UserRole + 1,
    statusRole
  };
  Q_ENUM(StatusRoles)

  DrawStatusModel(QObject* parent = nullptr);

  ~DrawStatusModel();

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;

  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  QHash<int, QByteArray> roleNames() const override;

  int addStatus(const QString& name, const QString& status);

  void setNameAt(int index, const QString& name);

  void setStatusAt(int index, const QString& status);

private:
  struct LayerStatus {
    QString name;
    QString status;
  };
  QList<LayerStatus> m_layerStatuses;
};

#endif // DRAWSTATUSMODEL_H
