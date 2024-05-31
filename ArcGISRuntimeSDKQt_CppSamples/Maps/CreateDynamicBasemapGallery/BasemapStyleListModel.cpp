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
    if (!index.isValid())
        return QVariant();
    if (role == StyleNameRole)
    {
        return m_previews[index.row()].styleName;
    }
    if (role == PreviewImageUrlRole)
    {
        return m_previews[index.row()].previewImageUrl;
    }
    return QVariant();
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

    QModelIndex newIndex = this->createIndex(currentSizeOfList, 0);
    beginInsertRows(newIndex, m_previews.size() - 1, m_previews.size());
    m_previews.append(std::move(newItem));
    endInsertRows();
}
