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

#ifndef CREATEDYNAMICBASEMAPGALLERY_H
#define CREATEDYNAMICBASEMAPGALLERY_H

#include <QAbstractListModel>
#include <QMap>
#include <QObject>
#include <QUrl>

Q_MOC_INCLUDE("MapQuickView.h")
Q_MOC_INCLUDE("BasemapStyleInfo.h")
Q_MOC_INCLUDE("BasemapStyleListModel.h")

class BasemapStyleListModel;
class QAbstractListModel;

namespace Esri::ArcGISRuntime {
class BasemapStyleInfo;
enum class BasemapStyleLanguageStrategy;
class Map;
class MapQuickView;
} // namespace Esri::ArcGISRuntime

class CreateDynamicBasemapGallery : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY
                   mapViewChanged)
    Q_PROPERTY(QAbstractListModel* gallery READ gallery NOTIFY galleryChanged)
    Q_PROPERTY(QStringList languageStrategies READ languageStrategies NOTIFY languageStrategiesChanged)
    Q_PROPERTY(QStringList languages READ languages NOTIFY languagesChanged)
    Q_PROPERTY(QStringList worldviews READ worldviews NOTIFY worldviewsChanged)
    Q_PROPERTY(int indexOfSelectedStyle READ indexOfSelectedStyle NOTIFY selectedStyleChanged)

public:
    explicit CreateDynamicBasemapGallery(QObject* parent = nullptr);
    ~CreateDynamicBasemapGallery() override;

    static void init();

    Esri::ArcGISRuntime::MapQuickView* mapView() const;
    void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
    QAbstractListModel* gallery() const;
    const QStringList& languageStrategies() const;
    const QStringList& languages() const;
    const QStringList& worldviews() const;
    const int indexOfSelectedStyle() const;

    Q_INVOKABLE void updateSelectedStyle(const QString& styleName);
    Q_INVOKABLE void loadBasemap(const QString& selectedStrategy, const QString& selectedLanguage, const QString& selectedWorldview);

signals:
    void mapViewChanged();
    void selectedStyleChanged();
    void galleryChanged();
    void languageStrategiesChanged();
    void languagesChanged();
    void worldviewsChanged();

private:
    void createGallery();
    void updateLanguageStrategiesList();
    void updateLanguagesList();
    void updateWorldviewsList();

    Esri::ArcGISRuntime::Map* m_map = nullptr;
    Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
    QList<Esri::ArcGISRuntime::BasemapStyleInfo*> m_styleInfos;
    BasemapStyleListModel* m_gallery = nullptr;
    Esri::ArcGISRuntime::BasemapStyleInfo* m_selectedStyle = nullptr;
    QStringList m_languageStrategies;
    QStringList m_languages;
    QStringList m_worldviews;
};

#endif // CREATEDYNAMICBASEMAPGALLERY_H
