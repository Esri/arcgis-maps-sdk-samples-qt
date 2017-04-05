// [WriteFile Name=FormatCoordinates, Category=Geometry]
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

#ifndef DISPLAY_MAP_H
#define DISPLAY_MAP_H

namespace Esri
{
    namespace ArcGISRuntime
    {
        class Map;
        class MapQuickView;
        class GraphicsOverlay;
    }
}

#include <QQuickItem>

class FormatCoordinates : public QQuickItem
{
    Q_OBJECT

public:
    explicit FormatCoordinates(QQuickItem* parent = nullptr);
    ~FormatCoordinates();

    void componentComplete() Q_DECL_OVERRIDE;

private:
    Esri::ArcGISRuntime::Map* m_map = nullptr;
    Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
    Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;

    const double m_startLongitude = 0.0;
    const double m_startLatitude = 0.0;
};

#endif // DISPLAY_MAP_H

