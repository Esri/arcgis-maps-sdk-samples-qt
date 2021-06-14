// [WriteFile Name=DisplayOgcApiCollection, Category=Layers]
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

#include "DisplayOgcApiCollection.h"

#include "FeatureLayer.h"
#include "Map.h"
#include "MapQuickView.h"
#include "OgcFeatureCollectionTable.h"
#include "QueryParameters.h"

using namespace Esri::ArcGISRuntime;

DisplayOgcApiCollection::DisplayOgcApiCollection(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::topographic(this), this))
{
  m_ogcFeatureCollectionTable = new OgcFeatureCollectionTable(QUrl("https://demo.ldproxy.net/daraa"), "TransportationGroundCrv", this);
  m_ogcFeatureCollectionTable->setFeatureRequestMode(FeatureRequestMode::ManualCache);

  m_queryParameters = QueryParameters();
  m_queryParameters.setSpatialRelationship(SpatialRelationship::Intersects);
  m_queryParameters.setMaxFeatures(5000);

  connect(m_ogcFeatureCollectionTable, &OgcFeatureCollectionTable::loadStatusChanged, this, [this]()
  {
    qDebug() << (int)m_ogcFeatureCollectionTable->loadStatus();
    if (m_ogcFeatureCollectionTable->loadStatus() == LoadStatus::Loaded)
    {
      m_featureLayer = new FeatureLayer(m_ogcFeatureCollectionTable, this);

      m_map->operationalLayers()->append(m_featureLayer);
    }
  });

  m_ogcFeatureCollectionTable->load();

}

DisplayOgcApiCollection::~DisplayOgcApiCollection() = default;

void DisplayOgcApiCollection::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DisplayOgcApiCollection>("Esri.Samples", 1, 0, "DisplayOgcApiCollectionSample");
}

MapQuickView* DisplayOgcApiCollection::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void DisplayOgcApiCollection::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  m_mapView->setViewpoint(Viewpoint(32.62, 36.10, 20'000));

  connect(m_ogcFeatureCollectionTable, &OgcFeatureCollectionTable::populateFromServiceCompleted, this, [this](QUuid taskId, FeatureQueryResult* featureQueryResult)
  {
    qDebug() << "populate from service result: " << featureQueryResult->iterator().features(this).length();

    qDebug() << "ogc feature collection table num of features: " << m_ogcFeatureCollectionTable->numberOfFeatures();

    qDebug() << "map features: " << m_featureLayer->attribution();
    qDebug() << Qt::endl;

    //m_featureLayer = new FeatureLayer(m_ogcFeatureCollectionTable, this);
    //m_map->operationalLayers()->append(m_featureLayer);
  });

  connect(m_mapView, &MapQuickView::drawStatusChanged, this, [this]()
  {
    if (m_mapView->drawStatus() == DrawStatus::Completed)
    {
      m_queryParameters.setGeometry(m_mapView->currentViewpoint(ViewpointType::BoundingGeometry).targetGeometry());
      m_ogcFeatureCollectionTable->populateFromService(m_queryParameters, false, {});
    }
  });

  emit mapViewChanged();
}
