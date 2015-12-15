// [WriteFile Name=ManageBookmarks, Category=Maps]
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

#ifndef MANAGE_BOOKMARKS_H
#define MANAGE_BOOKMARKS_H

namespace Esri
{
    namespace ArcGISRuntime
    {
        class Map;
        class MapQuickView;
        class Viewpoint;
    }
}

class QStringList;
class QString;

#include <QQuickItem>

class ManageBookmarks : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QStringList bookmarkList READ bookmarkList NOTIFY bookmarkListChanged)

public:
    ManageBookmarks(QQuickItem* parent = 0);
    ~ManageBookmarks();

    void componentComplete() Q_DECL_OVERRIDE;
    Q_INVOKABLE void goToBookmark(QString bookmarkName);
    Q_INVOKABLE void addBookmark(QString newBookmarkName);

signals:
    void bookmarkListChanged();

private:
    void createInitialBookmarks();
    void createBookmark(QString name, Esri::ArcGISRuntime::Viewpoint viewpoint);
    QStringList bookmarkList() const;

private:
    Esri::ArcGISRuntime::Map* m_map;
    Esri::ArcGISRuntime::MapQuickView* m_mapView;
    QStringList m_bookmarkList;
    QMap<QString, Esri::ArcGISRuntime::Viewpoint> m_bookmarks;
};

#endif // MANAGE_BOOKMARKS_H

