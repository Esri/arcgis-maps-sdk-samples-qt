// [WriteFile Name=Query_OGC, Category=Layers]
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

#include "Query_OGC.h"

#include "FeatureLayer.h"
#include "Map.h"
#include "MapQuickView.h"
#include "OgcFeatureCollectionTable.h"
#include "SimpleLineSymbol.h"
#include "SimpleRenderer.h"

using namespace Esri::ArcGISRuntime;

Query_OGC::Query_OGC(QObject* parent /* = nullptr */):
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

      //Get the extent of the dataset
      m_dataSetExtent = m_ogcFeatureCollectionTable->extent();

      // display result of query on the map
//      setInitialQueryOnOgcFeatureTable();
}

Query_OGC::~Query_OGC() = default;

void Query_OGC::init()
{
    // Register the map view for QML
    qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
    qmlRegisterType<Query_OGC>("Esri.Samples", 1, 0, "Query_OGCSample");
}

MapQuickView* Query_OGC::mapView() const
{
    return m_mapView;
}

// Set the view (created in QML)
void Query_OGC::setMapView(MapQuickView* mapView)
{
    if (!mapView || mapView == m_mapView)
      return;

    m_mapView = mapView;
    m_mapView->setMap(m_map);
    m_mapView->setViewpoint(Viewpoint(32.62, 36.10, 20'000));

    createQueryConnection();

    emit mapViewChanged();
}

void Query_OGC::createQueryConnection()
{
  connect(m_mapView, &MapQuickView::navigatingChanged, this, [this]()
  {
    if (m_mapView->isNavigating())
      return;

    QueryParameters queryParameters = QueryParameters();
    // Set the query area to what is currently visible in the map view
    queryParameters.setGeometry(m_mapView->currentViewpoint(ViewpointType::BoundingGeometry).targetGeometry());
    // SpatialRelationship::Intersects will return all features that are within and crossing the perimiter of the input geometry
    queryParameters.setSpatialRelationship(SpatialRelationship::Intersects);
    // Some services have low default values for max features returned
    queryParameters.setMaxFeatures(5000);

    // Populate the feature collection table with features that match the parameters, cache them locally, and store all table fields
    m_ogcFeatureCollectionTable->populateFromService(queryParameters, false, {});
  });
}

void Query_OGC::query(const QString& whereClause)
{
  if (!m_ogcFeatureCollectionTable || !m_featureLayer || !m_mapView)
    return;

  // create the parameters
  QueryParameters queryParams;
  queryParams.setGeometry(m_mapView->currentViewpoint(ViewpointType::BoundingGeometry).targetGeometry().extent());
  queryParams.setWhereClause(whereClause);

  // query the feature tables
  m_ogcFeatureCollectionTable->queryFeatures(queryParams);
}

