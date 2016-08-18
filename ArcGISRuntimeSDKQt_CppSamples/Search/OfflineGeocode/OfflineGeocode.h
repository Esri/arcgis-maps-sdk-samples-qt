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
        class SuggestListModel;
    }
}

#include "Point.h"
#include "SuggestResult.h"
#include "GeocodeParameters.h"
#include "ReverseGeocodeParameters.h"

#include <QQuickItem>

class OfflineGeocode : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(Esri::ArcGISRuntime::CalloutData* calloutData READ calloutData NOTIFY calloutDataChanged)
    Q_PROPERTY(Esri::ArcGISRuntime::SuggestListModel* suggestions READ suggestions NOTIFY suggestionsChanged)
    Q_PROPERTY(bool geocodeInProgress READ geocodeInProgress NOTIFY geocodeInProgressChanged)
    Q_PROPERTY(bool suggestInProgress READ suggestInProgress NOTIFY suggestInProgressChanged)
    Q_PROPERTY(bool noResults READ noResults NOTIFY noResultsChanged)

public:
    OfflineGeocode(QQuickItem* parent = nullptr);
    ~OfflineGeocode();

    void componentComplete() Q_DECL_OVERRIDE;
    Q_INVOKABLE void geocodeWithSuggestion(int index);  
    Q_INVOKABLE void geocodeWithText(const QString& address);
    Q_INVOKABLE void setSuggestionsText(const QString& searchText);

signals:
    void noResultsChanged();
    void calloutDataChanged();
    void suggestionsChanged();
    void suggestInProgressChanged();
    void geocodeInProgressChanged();
    void dismissSuggestions();

private:
    Esri::ArcGISRuntime::CalloutData* calloutData() const;
    Esri::ArcGISRuntime::SuggestListModel* suggestions() const;
    bool geocodeInProgress() const;
    bool noResults() const;
    bool suggestInProgress() const;
    void connectSignals();

private:
    bool m_isReverseGeocode;
    bool m_geocodeInProgress;
    bool m_isPressAndHold;
    bool m_noResults;
    bool m_suggestInProgress;
    QString m_dataPath;
    Esri::ArcGISRuntime::Map* m_map;
    Esri::ArcGISRuntime::Point m_clickedPoint;
    Esri::ArcGISRuntime::Graphic* m_pinGraphic;
    Esri::ArcGISRuntime::MapQuickView* m_mapView;
    Esri::ArcGISRuntime::CalloutData* m_calloutData;
    Esri::ArcGISRuntime::LocatorTask* m_locatorTask;    
    Esri::ArcGISRuntime::ArcGISTiledLayer* m_tiledLayer;
    Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay;       
    Esri::ArcGISRuntime::SuggestListModel* m_suggestListModel;   
    Esri::ArcGISRuntime::GeocodeParameters m_geocodeParameters;
    Esri::ArcGISRuntime::ReverseGeocodeParameters m_reverseGeocodeParameters;
};

#endif // OFFLINEGEOCODE_H
