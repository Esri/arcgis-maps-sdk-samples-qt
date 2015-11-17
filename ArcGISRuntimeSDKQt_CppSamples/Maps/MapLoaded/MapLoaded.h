// Copyright 2015 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef MAP_LOADED_H
#define MAP_LOADED_H

namespace Esri
{
    namespace ArcGISRuntime
    {
        class Map;
        class MapQuickView;
    }
}

class QString;

#include <QQuickItem>

class MapLoaded : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QString mapLoadStatus READ mapLoadStatus NOTIFY mapLoadStatusChanged)

public:
    MapLoaded(QQuickItem* parent = 0);
    ~MapLoaded();

    void componentComplete() Q_DECL_OVERRIDE;

signals:
    void mapLoadStatusChanged();

private:
    QString mapLoadStatus() const;

private:
    Esri::ArcGISRuntime::Map* m_map;
    Esri::ArcGISRuntime::MapQuickView* m_mapView;
    QString m_loadStatus;
};

#endif // MAP_LOADED_H

