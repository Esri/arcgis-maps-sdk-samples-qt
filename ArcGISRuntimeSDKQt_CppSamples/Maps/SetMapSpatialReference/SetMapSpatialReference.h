// [WriteFile Name=SetMapSpatialReference, Category=Maps]
// [Legal]
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
// [Legal]

#ifndef SET_MAP_SPATIAL_REFERENCE_H
#define SET_MAP_SPATIAL_REFERENCE_H

namespace Esri
{
    namespace ArcGISRuntime
    {
        class Map;
        class MapQuickView;
        class Basemap;
        class ArcGISMapImageLayer;
    }
}

#include <QQuickItem>

class SetMapSpatialReference : public QQuickItem
{
    Q_OBJECT

public:
    SetMapSpatialReference(QQuickItem* parent = 0);
    ~SetMapSpatialReference();

    void componentComplete() Q_DECL_OVERRIDE;

private:
    Esri::ArcGISRuntime::Map* m_map;
    Esri::ArcGISRuntime::MapQuickView* m_mapView;
    Esri::ArcGISRuntime::Basemap* m_basemap;
    Esri::ArcGISRuntime::ArcGISMapImageLayer* m_imageLayer;
};

#endif // SET_MAP_SPATIAL_REFERENCE_H

