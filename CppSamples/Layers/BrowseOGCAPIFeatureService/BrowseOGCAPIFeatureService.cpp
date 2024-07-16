// [WriteFile Name=BrowseOGCAPIFeatureService, Category=Layers]
// [Legal]
// Copyright 2021 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "BrowseOGCAPIFeatureService.h"

// C++ API headers
#include "Envelope.h"
#include "Error.h"
#include "FeatureLayer.h"
#include "GeodatabaseTypes.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "OgcFeatureCollectionInfo.h"
#include "OgcFeatureCollectionTable.h"
#include "OgcFeatureService.h"
#include "OgcFeatureServiceInfo.h"
#include "QueryParameters.h"

// Qt headers
#include <QFuture>
#include <QQmlEngine>

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

QString BrowseOGCAPIFeatureService::errorMessage() const
{
  return m_errorMessage;
}

void BrowseOGCAPIFeatureService::setErrorMessage(const QString& message)
{
  m_errorMessage = message;
  emit errorMessageChanged();
}

void BrowseOGCAPIFeatureService::handleError(const Esri::ArcGISRuntime::Error& error)
{
  if (error.additionalMessage().isEmpty())
    setErrorMessage(error.message());
  else
    setErrorMessage(error.message() + "\n" + error.additionalMessage());
}

void BrowseOGCAPIFeatureService::loadFeatureService(const QUrl& url)
{
  if (m_featureService && m_featureService->loadStatus() == LoadStatus::Loading)
    return;

  clearExistingFeatureService();

  // Instantiate new OGCFeatureService object using url
  m_featureService = new OgcFeatureService(url, this);

  // Retrieve collection info when feature service has loaded
  connect(m_featureService, &OgcFeatureService::doneLoading, this, &BrowseOGCAPIFeatureService::retrieveCollectionInfos);

  // Connect errorOccurred to handleError()
  connect(m_featureService, &OgcFeatureService::errorOccurred, this, &BrowseOGCAPIFeatureService::handleError);

  // Connect to loadingChanged() to enable and disable UI buttons
  connect(m_featureService, &OgcFeatureService::loadStatusChanged, this, &BrowseOGCAPIFeatureService::serviceOrFeatureLoadingChanged);

  m_featureService->load();
}

void BrowseOGCAPIFeatureService::clearExistingFeatureService()
{
  m_collectionInfo.clear();
  if (m_serviceInfo != nullptr)
  {
    delete m_serviceInfo;
    m_serviceInfo = nullptr;
  }
  if (m_featureService != nullptr)
  {
    delete m_featureService;
    m_featureService = nullptr;
  }

  m_featureCollectionList.clear();
  emit featureCollectionListChanged();
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
  for (OgcFeatureCollectionInfo* info : std::as_const(m_collectionInfo))
  {
    m_featureCollectionList.push_back(info->title());
  }

  emit featureCollectionListChanged();
}

void BrowseOGCAPIFeatureService::loadService(const QUrl& urlFromInterface)
{
  m_featureServiceUrl = urlFromInterface;
  emit urlChanged();
  loadFeatureService(m_featureServiceUrl);
}

void BrowseOGCAPIFeatureService::loadFeatureCollection(int selectedFeature)
{
  clearExistingFeatureLayer();

  // Create a CollectionInfo object from the selected feature collection
  OgcFeatureCollectionInfo* info = m_collectionInfo[selectedFeature];

  // Assign the CollectedInfo to the m_featureCollectionTable property
  m_featureCollectionTable = new OgcFeatureCollectionTable(info, this);

  // Set the feature request mode to manual (only manual is currently supported)
  // In this mode you must manually populate the table - panning and zooming won't request features automatically
  m_featureCollectionTable->setFeatureRequestMode(FeatureRequestMode::ManualCache);

  // Populate the OGC feature collection table
  QueryParameters queryParameters;
  queryParameters.setMaxFeatures(1000);
  auto future = m_featureCollectionTable->populateFromServiceAsync(queryParameters, false, QStringList{ /* empty */ });
  Q_UNUSED(future)

  // Create new Feature Layer from selected collection
  m_featureLayer = new FeatureLayer(m_featureCollectionTable, this);

  // Add feature layer to operational layers when it has finished loading
  connect(m_featureLayer, &FeatureLayer::doneLoading, this, &BrowseOGCAPIFeatureService::addFeatureLayerToMap);

  // Connect errorOccurred to handleError()
  connect(m_featureLayer, &FeatureLayer::errorOccurred, this, &BrowseOGCAPIFeatureService::handleError);

  // Connect to loadingChanged() to enable and disable UI buttons
  connect(m_featureLayer, &FeatureLayer::loadStatusChanged, this, &BrowseOGCAPIFeatureService::serviceOrFeatureLoadingChanged);

  m_featureLayer->load();
}

void BrowseOGCAPIFeatureService::clearExistingFeatureLayer()
{
  if (m_featureCollectionTable != nullptr)
  {
    delete m_featureCollectionTable;
    m_featureCollectionTable = nullptr;
  }

  if (m_featureLayer != nullptr)
  {
    delete m_featureLayer;
    m_featureLayer = nullptr;
  }
}

void BrowseOGCAPIFeatureService::addFeatureLayerToMap()
{
  // Adjust the viewpoint to match the extent of the layer
  m_mapView->setViewpointGeometryAsync(m_featureLayer->fullExtent());

  // Clear any existing layers and add current layer to map
  m_map->operationalLayers()->clear();
  m_map->operationalLayers()->append(m_featureLayer);
}

bool BrowseOGCAPIFeatureService::serviceOrFeatureLoading() const
{
  if (m_featureService && m_featureService->loadStatus() == LoadStatus::Loading)
    return true;

  else if (m_featureLayer && m_featureLayer->loadStatus() == LoadStatus::Loading)
    return true;

  return false;
}
