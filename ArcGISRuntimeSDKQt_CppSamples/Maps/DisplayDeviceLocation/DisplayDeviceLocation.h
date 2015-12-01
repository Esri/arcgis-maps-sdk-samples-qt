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

#ifndef DISPLAY_DEVICE_LOCATION_H
#define DISPLAY_DEVICE_LOCATION_H

namespace Esri
{
    namespace ArcGISRuntime
    {
        class Map;
        class MapQuickView;
    }
}

#include <QQuickItem>

class DisplayDeviceLocation : public QQuickItem
{
    Q_OBJECT

public:
    DisplayDeviceLocation(QQuickItem* parent = 0);
    ~DisplayDeviceLocation();

    void componentComplete() Q_DECL_OVERRIDE;
    Q_INVOKABLE void startLocationDisplay();
    Q_INVOKABLE void stopLocationDisplay();
    Q_INVOKABLE void setAutoPanMode(QString autoPanMode);

private:
    Esri::ArcGISRuntime::Map* m_map;
    Esri::ArcGISRuntime::MapQuickView* m_mapView;
};

#endif // DISPLAY_DEVICE_LOCATION_H

