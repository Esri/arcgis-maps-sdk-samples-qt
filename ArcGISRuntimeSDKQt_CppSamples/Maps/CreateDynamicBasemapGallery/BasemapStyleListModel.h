// [WriteFile Name=CreateDynamicBasemapGallery, Category=Maps]
// [Legal]
// Copyright 2024 Esri.

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

#ifndef BASEMAPSTYLELISTMODEL_H
#define BASEMAPSTYLELISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QUrl>

namespace Esri::ArcGISRuntime {
class BasemapStyleInfo;
}

class BasemapStyleListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum previewRoles {
        StyleNameRole = Qt::UserRole + 1,
        PreviewImageUrlRole
    };

    BasemapStyleListModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    void insertRowIntoGallery(const Esri::ArcGISRuntime::BasemapStyleInfo* newItem);

private:
    QList<const Esri::ArcGISRuntime::BasemapStyleInfo*> m_previews;
};

#endif // BASEMAPSTYLELISTMODEL_H
