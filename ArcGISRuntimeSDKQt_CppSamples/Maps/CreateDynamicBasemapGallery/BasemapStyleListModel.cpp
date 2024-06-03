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

#include "BasemapStyleListModel.h"

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
    bool modelIndexIsOutOfBounds = index.row() > m_previews.size();
    if (!index.isValid() || modelIndexIsOutOfBounds)
    {
        return QVariant();
    }

    switch(role)
    {
    case StyleNameRole:
        return m_previews[index.row()].styleName;
    case PreviewImageUrlRole:
        return m_previews[index.row()].previewImageUrl;
    default:
        return QVariant();
    }
}

int BasemapStyleListModel::rowCount(const QModelIndex& parent) const
{
    return m_previews.size();
}

void BasemapStyleListModel::insertRowIntoGallery(StylePreview newItem)
{
    int currentSizeOfList = m_previews.size();
    if (currentSizeOfList == 0)
    {
        beginResetModel();
        m_previews.clear();
        m_previews.append(std::move(newItem));
        endResetModel();
        return;
    }

    // Put all data in the same row - the BasemapStyleListModel should only have one column.
    QModelIndex newIndex = this->createIndex(currentSizeOfList, 0);
    beginInsertRows(newIndex, m_previews.size() - 1, m_previews.size());
    m_previews.append(std::move(newItem));
    endInsertRows();
}
