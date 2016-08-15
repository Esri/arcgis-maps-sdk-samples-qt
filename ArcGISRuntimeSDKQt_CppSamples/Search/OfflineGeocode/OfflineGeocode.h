// [WriteFile Name=OfflineGeocode, Category=Search]
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

#ifndef OFFLINEGEOCODE_H
#define OFFLINEGEOCODE_H

namespace Esri
{
    namespace ArcGISRuntime
    {
        class Map;
        class Graphic;
        class LocatorTask;
        class CalloutData;
        class MapQuickView;
        class GraphicsOverlay;
        class ArcGISTiledLayer;
    }
}

#include "Point.h"
#include "ReverseGeocodeParameters.h"
#include "GeocodeParameters.h"

#include <QQuickItem>

class OfflineGeocode : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(Esri::ArcGISRuntime::CalloutData* calloutData READ calloutData NOTIFY calloutDataChanged)

public:
    OfflineGeocode(QQuickItem* parent = nullptr);
    ~OfflineGeocode();

    void componentComplete() Q_DECL_OVERRIDE;
    Q_INVOKABLE void geocodeWithText(const QString& address);

signals:
    void calloutDataChanged();

private:
    Esri::ArcGISRuntime::CalloutData* calloutData() const;
    void connectSignals();

private:
    bool m_isReverseGeocode;
    bool m_isPressAndHold;
    QString m_dataPath;
    Esri::ArcGISRuntime::Map* m_map;
    Esri::ArcGISRuntime::MapQuickView* m_mapView;
    Esri::ArcGISRuntime::CalloutData* m_calloutData;
    Esri::ArcGISRuntime::LocatorTask* m_locatorTask;
    Esri::ArcGISRuntime::Graphic* m_pinGraphic;
    Esri::ArcGISRuntime::ArcGISTiledLayer* m_tiledLayer;
    Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay;
    Esri::ArcGISRuntime::GeocodeParameters m_geocodeParameters;
    Esri::ArcGISRuntime::ReverseGeocodeParameters m_reverseGeocodeParameters;
    Esri::ArcGISRuntime::Point m_clickedPoint;

};

#endif // OFFLINEGEOCODE_H
