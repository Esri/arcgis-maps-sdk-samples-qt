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

    connect(m_mapView, &MapQuickView::navigatingChanged, this, [this]()
      {
        QueryParameters queryParams;

        //Get the extent of the dataset
        m_dataSetExtent = m_ogcFeatureCollectionTable->extent();
        queryParams.setGeometry(m_dataSetExtent);

        queryParams.setMaxFeatures(1000);

        // Populate the feature collection table with features that match the parameters, cache them locally, and store all table fields
        m_ogcFeatureCollectionTable->populateFromService(queryParams, false, {});
      });

    emit mapViewChanged();
}

void Query_OGC::query(const QString& whereClause, const QString& maxFeature, const QString& fromDateString, const QString& toDateString)
{
  if (!m_ogcFeatureCollectionTable || !m_featureLayer || !m_mapView)
    return;


  connect(m_mapView, &MapQuickView::navigatingChanged, this, [this, whereClause, maxFeature, fromDateString, toDateString]()
  {
      // create the parameters
      QueryParameters queryParams;

      queryParams.setGeometry(m_mapView->currentViewpoint(ViewpointType::BoundingGeometry).targetGeometry().extent());
      queryParams.setWhereClause(whereClause);
      queryParams.setMaxFeatures(maxFeature.toUInt());

      QDateTime fromDate = QDateTime::fromString(fromDateString,"dd-MM-yyyy");
      QDateTime toDate = QDateTime::fromString(toDateString,"dd-MM-yyyy");
      TimeExtent timeExtent(fromDate, toDate);
      queryParams.setTimeExtent(timeExtent);

      // query the feature tables
    //  m_ogcFeatureCollectionTable->queryFeatures(queryParams);

      // Populate the feature collection table with features that match the parameters, cache them locally, and store all table fields
      m_ogcFeatureCollectionTable->populateFromService(queryParams, false, {});
  });
}

