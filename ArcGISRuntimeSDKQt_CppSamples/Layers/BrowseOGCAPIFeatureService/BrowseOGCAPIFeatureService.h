// [WriteFile Name=BrowseOGCAPIFeatureService, Category=Layers]
// [Legal]
// Copyright 2021 Esri.

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

#ifndef BROWSEOGCAPIFEATURESERVICE_H
#define BROWSEOGCAPIFEATURESERVICE_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
}
}

#include <QObject>

#include "OgcFeatureService.h"
#include "OgcFeatureCollectionInfo.h"
#include "OgcFeatureCollectionTable.h"

class BrowseOGCAPIFeatureService : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
    Q_PROPERTY(QUrl featureServiceUrl READ featureServiceUrl NOTIFY urlChanged)
    Q_PROPERTY(QStringList featureCollectionList READ featureCollectionList NOTIFY featureCollectionListChanged)

public:
    explicit BrowseOGCAPIFeatureService(QObject* parent = nullptr);
    ~BrowseOGCAPIFeatureService();

    static void init();

    Q_INVOKABLE void loadService(QUrl urlFromInterface);
    Q_INVOKABLE void loadFeatureCollection(int selectedFeature);

signals:
    void mapViewChanged();
    void urlChanged();
    void featureCollectionListChanged();

private:
    Esri::ArcGISRuntime::MapQuickView* mapView() const;
    void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
    QUrl featureServiceUrl() const;
    QStringList featureCollectionList() const;

    Esri::ArcGISRuntime::Map* m_map = nullptr;
    Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

    QUrl m_featureServiceUrl;
    Esri::ArcGISRuntime::OgcFeatureService* m_featureService = nullptr;
    Esri::ArcGISRuntime::OgcFeatureServiceInfo* m_serviceInfo = nullptr;
    QList<Esri::ArcGISRuntime::OgcFeatureCollectionInfo*> m_collectionInfo;
    QStringList m_featureCollectionList;
    Esri::ArcGISRuntime::OgcFeatureCollectionTable* m_featureCollectionTable;
    Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;

    void loadFeatureService(const QUrl& url);
    void checkIfServiceLoaded(Esri::ArcGISRuntime::LoadStatus loadstatus);
    void retrieveCollectionInfos();
    void createFeatureCollectionList();
    void checkIfLayerLoaded(Esri::ArcGISRuntime::LoadStatus loadStatus);
    void addFeatureToMap();
};

#endif // BROWSEOGCAPIFEATURESERVICE_H
