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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "BrowseOGCAPIFeatureService.h"
#include "OgcFeatureService.h"
#include "OgcFeatureServiceInfo.h"
#include "OgcFeatureCollectionTable.h"
#include "OgcFeatureCollectionInfo.h"
#include "FeatureLayer.h"

#include "Map.h"
#include "MapQuickView.h"

#include<QQmlEngine>

using namespace Esri::ArcGISRuntime;

BrowseOGCAPIFeatureService::BrowseOGCAPIFeatureService(QObject* parent /* = nullptr */):
    QObject(parent),
    m_map(new Map(BasemapStyle::ArcGISTopographic, this))
{
    // Initialise OGC feature service
    m_featureServiceUrl = "https://demo.ldproxy.net/daraa";
    loadFeatureService(m_featureServiceUrl);
}

BrowseOGCAPIFeatureService::~BrowseOGCAPIFeatureService() = default;

void BrowseOGCAPIFeatureService::init()
{
    // Register the map view for QML
    qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
    qmlRegisterType<BrowseOGCAPIFeatureService>("Esri.Samples", 1, 0, "BrowseOGCAPIFeatureServiceSample");
}

void BrowseOGCAPIFeatureService::loadFeatureService(const QUrl& url)
{
    // Delete existing feature service and associated objects
    m_collectionInfo.clear();
    delete m_serviceInfo;
    delete m_featureService;
    m_featureCollectionList.clear();
    emit featureCollectionListChanged();

    // Instantiate new OGCFeatureService object using url
    m_featureService = new OgcFeatureService(url, this);

    // Connect loadStatusChanged and checkIfServiceLoaded()
    connect(m_featureService, &OgcFeatureService::loadStatusChanged, this, &BrowseOGCAPIFeatureService::checkIfServiceLoaded);

    m_featureService->load();
}

void BrowseOGCAPIFeatureService::checkIfServiceLoaded(LoadStatus loadstatus)
{
    if (loadstatus != LoadStatus::Loaded)
    {
        return;
    }
    retrieveCollectionInfos();
}

void BrowseOGCAPIFeatureService::retrieveCollectionInfos()
{
    // Assign OGC service metadata to m_serviceInfo property
    m_serviceInfo = m_featureService->serviceInfo();

    // Assign information from each feature collection to the m_collectionInfo property
    m_collectionInfo = m_serviceInfo->featureCollectionInfos();

    createFeatureCollectionList();
}

void BrowseOGCAPIFeatureService::createFeatureCollectionList()
{
    m_featureCollectionList.clear();

    // Populate list with names of each feature collection
    for (OgcFeatureCollectionInfo* info : m_collectionInfo)
    {
        m_featureCollectionList.push_back(info->title());
    }

    emit featureCollectionListChanged();
}

MapQuickView* BrowseOGCAPIFeatureService::mapView() const
{
    return m_mapView;
}

// Set the view (created in QML)
void BrowseOGCAPIFeatureService::setMapView(MapQuickView* mapView)
{
    if (!mapView || mapView == m_mapView)
        return;

    m_mapView = mapView;
    m_mapView->setMap(m_map);

    emit mapViewChanged();
}

QUrl BrowseOGCAPIFeatureService::featureServiceUrl() const
{
    return m_featureServiceUrl;
}

QStringList BrowseOGCAPIFeatureService::featureCollectionList() const
{
    return m_featureCollectionList;
}

// When "Load Service" button is pressed in interface (from QML)
void BrowseOGCAPIFeatureService::loadService(QUrl urlFromInterface)
{
    m_featureServiceUrl = urlFromInterface;
    emit urlChanged();
    loadFeatureService(m_featureServiceUrl);
}

void BrowseOGCAPIFeatureService::loadFeatureCollection(int selectedFeature)
{
    // Create a CollectionInfo object from the selected collection
    OgcFeatureCollectionInfo *info = m_collectionInfo[selectedFeature];

    // Assign the CollectedInfo to the m_featureCollectionTable property
    m_featureCollectionTable = new OgcFeatureCollectionTable(info);

    // Set the feature request mode to manual (only manual is currently supported)
    // In this mode you must manually populate the table - panning and zooming won't request features automatically
    m_featureCollectionTable->setFeatureRequestMode(FeatureRequestMode::ManualCache);

    // Populate the OGC feature collection table
    QueryParameters queryParameters;
    queryParameters.setMaxFeatures(1000);
    m_featureCollectionTable->populateFromService(queryParameters, false, QStringList{ /* empty */ });

    // Delete existing feature layer and create new layer from selected collection
    delete m_featureLayer;
    m_featureLayer = new FeatureLayer(m_featureCollectionTable);

    // Connect LoadStatusChanged with CheckIfLayerLoaded
    connect(m_featureLayer, &FeatureLayer::loadStatusChanged, this, &BrowseOGCAPIFeatureService::checkIfLayerLoaded);

    // Load feature layer
    m_featureLayer->load();
}

void BrowseOGCAPIFeatureService::checkIfLayerLoaded(LoadStatus loadStatus)
{
    if (loadStatus != LoadStatus::Loaded)
    {
        return;
    }
    addFeatureToMap();
}

void BrowseOGCAPIFeatureService::addFeatureToMap()
{
    // Adjust the viewpoint to match the extent of the layer
    m_mapView->setViewpointGeometry(m_featureLayer->fullExtent());

    // Clear any existing layers and add current layer to map
    m_map->operationalLayers()->clear();
    m_map->operationalLayers()->append(m_featureLayer);
}
