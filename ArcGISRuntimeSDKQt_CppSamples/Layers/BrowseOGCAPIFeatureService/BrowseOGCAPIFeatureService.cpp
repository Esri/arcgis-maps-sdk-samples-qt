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
    initialiseOGCService(m_featureServiceUrl);
}

BrowseOGCAPIFeatureService::~BrowseOGCAPIFeatureService() = default;

void BrowseOGCAPIFeatureService::init()
{
    // Register the map view for QML
    qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
    qmlRegisterType<BrowseOGCAPIFeatureService>("Esri.Samples", 1, 0, "BrowseOGCAPIFeatureServiceSample");
}

void BrowseOGCAPIFeatureService::initialiseOGCService(const QUrl& url)
{
    // Delete existing feature service and associated information (if nullptr, nothing will happen).
    m_collectionInfo.clear();
    delete m_serviceInfo;
    delete m_featureService;
    m_featureList.clear();
    emit featureListChanged();

    // Create an OGCFeatureService object
    m_featureService = new OgcFeatureService(url, this);

    // Establish connection between loadStatusChanged and serviceLoading()
    connect(m_featureService, &OgcFeatureService::loadStatusChanged, this, &BrowseOGCAPIFeatureService::handleLoadingStatus);

    // Handle any errors that occur during feature service initialisation
    connect(m_featureService, &OgcFeatureService::errorOccurred, this, [](Error error){
        // Print error message
        qDebug() << error.message();
    });

    // Load feature service
    m_featureService->load();
}

void BrowseOGCAPIFeatureService::handleLoadingStatus(LoadStatus loadstatus)
{
    switch(loadstatus)
    {
    case LoadStatus::Loaded:
        // Update features in interface
        retrieveFeatures();
        break;
    case LoadStatus::FailedToLoad:
        // Do nothing - the error will have been handled
        break;
    case LoadStatus::Loading:
        // Do nothing
        break;
    case LoadStatus::NotLoaded:
        // Do nothing
        break;
    case LoadStatus::Unknown:
        // Do nothing
        break;
    default:
        // Do Nothing
        break;
    }
}

void BrowseOGCAPIFeatureService::retrieveFeatures()
{
    // Get OGC service metadata
    m_serviceInfo = m_featureService->serviceInfo();

    // Create a list of feature collections
    m_collectionInfo = m_serviceInfo->featureCollectionInfos();

    // Display feature list in interface
    updateListInInterface();
}

void BrowseOGCAPIFeatureService::updateListInInterface()
{
    // Clear current feature list
    m_featureList.clear();

    // Convert elements of m_collectionInfo into QStringList
    for (OgcFeatureCollectionInfo* info : m_collectionInfo)
    {
        // Add the name (title) of the feature to m_featureList
        m_featureList.push_back(info->title());
    }

    emit featureListChanged();
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

QStringList BrowseOGCAPIFeatureService::featureList() const
{
    return m_featureList;
}

void BrowseOGCAPIFeatureService::loadService(QUrl urlFromInterface)
{
    m_featureServiceUrl = urlFromInterface;
    emit urlChanged();
    initialiseOGCService(m_featureServiceUrl);
}

void BrowseOGCAPIFeatureService::loadFeatureAtIndex(int index)
{
    // Get the selected collection
    OgcFeatureCollectionInfo *info = m_collectionInfo[index];

    // Create the OGC feature collection table
    m_featureCollectionTable = new OgcFeatureCollectionTable(info);

    // Set the feature request mode to manual (only manual is currently supported)
    // In this mode you must manually populate the table - panning and zooming won't request features automatically
    m_featureCollectionTable->setFeatureRequestMode(FeatureRequestMode::ManualCache);

    // Populate the OGC feature collection table
    QueryParameters queryParameters;
    queryParameters.setMaxFeatures(1000);
    m_featureCollectionTable->populateFromService(queryParameters, false, QStringList{ /* empty */ });

    // Create a feature layer
    FeatureLayer* m_featureLayer = new FeatureLayer(m_featureCollectionTable);

    // When feature has loaded, adjust the position of the mapView
    connect(m_featureLayer, &FeatureLayer::doneLoading, this, [this, m_featureLayer](){
        // Define view insets
        //m_mapView->setViewInsets(m_mapView->viewInsets());

        // Position the map to focus on the extent of the layer
        m_mapView->setViewpointGeometry(m_featureLayer->fullExtent());

    });

    // Clear any existing layers and add current layer to map
    m_map->operationalLayers()->clear();
    m_map->operationalLayers()->append(m_featureLayer);
}
