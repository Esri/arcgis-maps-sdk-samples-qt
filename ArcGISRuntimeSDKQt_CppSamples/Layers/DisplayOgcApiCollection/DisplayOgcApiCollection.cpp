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
#include "SimpleLineSymbol.h"
#include "SimpleRenderer.h"

using namespace Esri::ArcGISRuntime;

DisplayOgcApiCollection::DisplayOgcApiCollection(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISTopographic, this))
{
  const QString serviceUrl = "https://demo.ldproxy.net/daraa";
  const QString collectionId = "TransportationGroundCrv";
  m_ogcFeatureCollectionTable = new OgcFeatureCollectionTable(QUrl(serviceUrl), collectionId, this);
  m_ogcFeatureCollectionTable->setFeatureRequestMode(FeatureRequestMode::ManualCache);

  connect(m_ogcFeatureCollectionTable, &OgcFeatureCollectionTable::loadStatusChanged, this, [this]()
  {
    if (m_ogcFeatureCollectionTable->loadStatus() == LoadStatus::Loaded)
    {
      m_featureLayer = new FeatureLayer(m_ogcFeatureCollectionTable, this);
      m_featureLayer->setRenderer(new SimpleRenderer(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::blue, 3, this), this));
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

  createQueryConnection();

  emit mapViewChanged();
}

void DisplayOgcApiCollection::createQueryConnection()
{
  connect(m_mapView, &MapQuickView::navigatingChanged, this, [this]()
  {
    if (!m_mapView->isNavigating())
    {
      QueryParameters queryParameters = QueryParameters();
      queryParameters.setSpatialRelationship(SpatialRelationship::Intersects);
      queryParameters.setMaxFeatures(5000);

      Geometry queryArea = m_mapView->currentViewpoint(ViewpointType::BoundingGeometry).targetGeometry();
      queryParameters.setGeometry(queryArea);

      m_ogcFeatureCollectionTable->populateFromService(queryParameters, false, {});
    }
  });
}
