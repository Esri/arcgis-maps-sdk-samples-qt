// [WriteFile Name=Query_OGC, Category=Layers]
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

#include "QueryOGCAPICQLFilters.h"

#include "FeatureLayer.h"
#include "Map.h"
#include "MapQuickView.h"
#include "OgcFeatureCollectionTable.h"
#include "SimpleLineSymbol.h"
#include "SimpleRenderer.h"
#include "MapTypes.h"
#include "SymbolTypes.h"
#include "LayerListModel.h"
#include "GeodatabaseTypes.h"
#include "QueryParameters.h"
#include "TimeExtent.h"
#include "Viewpoint.h"

#include <QDateTime>
#include <QFuture>

using namespace Esri::ArcGISRuntime;

QueryOGCAPICQLFilters::QueryOGCAPICQLFilters(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISTopographic, this))
{
  const QUrl serviceUrl = QUrl("https://demo.ldproxy.net/daraa");
  const QString collectionId = "TransportationGroundCrv";
  m_ogcFeatureCollectionTable = new OgcFeatureCollectionTable(serviceUrl, collectionId, this);

  // FeatureRequestMode::ManualCache specifies that features from the server will be stored locally for display and querying
  // In this mode, ServiceFeatureTable::populateFromService() must be called to populate the local cache
  m_ogcFeatureCollectionTable->setFeatureRequestMode(FeatureRequestMode::ManualCache);

  // m_ogcFeatureCollectionTable->load() will be automatically called when added to a FeatureLayer
  m_featureLayer = new FeatureLayer(m_ogcFeatureCollectionTable, this);

  m_map->operationalLayers()->append(m_featureLayer);

  // Set a renderer to it to visualize the OGC API features and zoom to features
  m_featureLayer->setRenderer(new SimpleRenderer(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::darkMagenta, 3, this), this));
}

QueryOGCAPICQLFilters::~QueryOGCAPICQLFilters() = default;

void QueryOGCAPICQLFilters::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<QueryOGCAPICQLFilters>("Esri.Samples", 1, 0, "QueryOGCAPICQLFiltersSample");
}

MapQuickView* QueryOGCAPICQLFilters::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void QueryOGCAPICQLFilters::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);
  m_mapView->setViewpointAsync(Viewpoint(32.62, 36.10, 200'000));

  QueryParameters queryParams;

  //Get the extent of the dataset
  m_dataSetExtent = m_ogcFeatureCollectionTable->extent();
  queryParams.setGeometry(m_dataSetExtent);
  queryParams.setWhereClause("F_CODE = 'AP010'");
  queryParams.setMaxFeatures(1000);

  // Populate the feature collection table with features that match the parameters, clear the cache, and store all table fields
  auto future = m_ogcFeatureCollectionTable->populateFromServiceAsync(queryParams, true, {});
  Q_UNUSED(future)

  emit mapViewChanged();
}

void QueryOGCAPICQLFilters::query(const QString& whereClause, const QString& maxFeature, const QString& fromDateString, const QString& toDateString)
{
  if (!m_ogcFeatureCollectionTable || !m_featureLayer || !m_mapView)
    return;

  // create the parameters
  QueryParameters queryParams;

  queryParams.setGeometry(m_mapView->currentViewpoint(ViewpointType::BoundingGeometry).targetGeometry().extent());
  queryParams.setWhereClause(whereClause);
  queryParams.setMaxFeatures(maxFeature.toUInt());

  if (!fromDateString.isEmpty() && !toDateString.isEmpty())
  {
    const QDateTime fromDate = QDateTime::fromString(fromDateString,"MM/dd/yyyy");
    const QDateTime toDate = QDateTime::fromString(toDateString,"MM/dd/yyyy");
    const TimeExtent timeExtent(fromDate.toUTC(), toDate.toUTC());
    queryParams.setTimeExtent(timeExtent);
  }
  
  // Populate the feature collection table with features that match the parameters,
  // clear the cache to prepare for the new query results, and store all table fields
  auto future = m_ogcFeatureCollectionTable->populateFromServiceAsync(queryParams, true, {});
  Q_UNUSED(future)
}
