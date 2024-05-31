#ifndef BASEMAPSTYLELISTMODEL_H
#define BASEMAPSTYLELISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QUrl>

struct StylePreview {
    StylePreview(QString name, QUrl imageUrl) :
        styleName(name),
        previewImageUrl(imageUrl)
    {}

    QString styleName;
    QUrl previewImageUrl;
};

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
    void insertRowIntoGallery(StylePreview newItem);

private:
    QList<StylePreview> m_previews;
};

#endif // BASEMAPSTYLELISTMODEL_H
