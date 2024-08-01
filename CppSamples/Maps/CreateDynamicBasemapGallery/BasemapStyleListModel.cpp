// [WriteFile Name=CreateDynamicBasemapGallery, Category=Maps]
// [Legal]
// Copyright 2024 Esri.
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

// sample headers
#include "BasemapStyleListModel.h"

// ArcGIS Maps SDK headers
#include "BasemapStyleInfo.h"

using namespace Esri::ArcGISRuntime;

BasemapStyleListModel::BasemapStyleListModel(QObject* parent /* = nullptr */)
    : QAbstractListModel(parent){}

QHash<int, QByteArray> BasemapStyleListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[StyleNameRole] = "styleName";
    roles[PreviewImageUrlRole] = "previewImageUrl";
    return roles;
}

QVariant BasemapStyleListModel::data(const QModelIndex& index, int role) const
{
    bool modelIndexIsOutOfBounds = index.row() >= m_previews.size();
    if (!index.isValid() || modelIndexIsOutOfBounds)
    {
        return QVariant();
    }

    switch(role)
    {
    case StyleNameRole:
        return m_previews[index.row()]->styleName();
    case PreviewImageUrlRole:
        return m_previews[index.row()]->thumbnailUrl();
    default:
        return QVariant();
    }
}

int BasemapStyleListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return m_previews.size();
}

void BasemapStyleListModel::insertItemsIntoGallery(const QList<BasemapStyleInfo*> infos)
{
    beginResetModel();
    m_previews.clear();
    for (auto index = 0; index < infos.size(); ++index)
    {
        m_previews.insert(index, infos.at(index));
        connect(m_previews.at(index), &BasemapStyleInfo::thumbnailUrlChanged, this, [this, index]() 
        {
            QModelIndex modelIndex = createIndex(index, 0);
            emit dataChanged(modelIndex, modelIndex);
        });
    }
    endResetModel();
}
