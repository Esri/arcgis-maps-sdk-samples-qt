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
    Q_PROPERTY(QStringList featureList READ featureList NOTIFY featureListChanged)

public:
    explicit BrowseOGCAPIFeatureService(QObject* parent = nullptr);
    ~BrowseOGCAPIFeatureService();

    static void init();

    Q_INVOKABLE void loadService(QUrl urlFromInterface);
    Q_INVOKABLE void loadFeatureAtIndex(int index);

signals:
    void mapViewChanged();
    void urlChanged();
    void featureListChanged();

private:
    Esri::ArcGISRuntime::MapQuickView* mapView() const;
    void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
    QUrl featureServiceUrl() const;
    QStringList featureList() const;

    Esri::ArcGISRuntime::Map* m_map = nullptr;
    Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

    QUrl m_featureServiceUrl;
    Esri::ArcGISRuntime::OgcFeatureService* m_featureService = nullptr;
    Esri::ArcGISRuntime::OgcFeatureServiceInfo* m_serviceInfo = nullptr;
    QList<Esri::ArcGISRuntime::OgcFeatureCollectionInfo*> m_collectionInfo;
    QStringList m_featureList;
    Esri::ArcGISRuntime::OgcFeatureCollectionTable* m_featureCollectionTable;

    void initialiseOGCService(const QUrl Url);
    void handleLoadingStatus(Esri::ArcGISRuntime::LoadStatus loadstatus);
    void retrieveFeatures();
    void updateListInInterface();
};

#endif // BROWSEOGCAPIFEATURESERVICE_H
