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

#ifndef SHOW_MAGNIFIER_H
#define SHOW_MAGNIFIER_H

namespace Esri
{
    namespace ArcGISRuntime
    {
        class Map;
        class MapQuickView;
    }
}

#include <QQuickItem>

class ShowMagnifier : public QQuickItem
{
    Q_OBJECT

public:
    ShowMagnifier(QQuickItem* parent = 0);
    ~ShowMagnifier();

    void componentComplete() Q_DECL_OVERRIDE;

private:
    Esri::ArcGISRuntime::Map* m_map;
    Esri::ArcGISRuntime::MapQuickView* m_mapView;
};

#endif // SHOW_MAGNIFIER_H

