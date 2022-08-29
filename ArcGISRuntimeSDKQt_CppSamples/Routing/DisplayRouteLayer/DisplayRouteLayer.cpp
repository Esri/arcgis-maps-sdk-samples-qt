// [WriteFile Name=DisplayRouteLayer, Category=Routing]
// [Legal]
// Copyright 2022 Esri.

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

#include "DisplayRouteLayer.h"

#include "FeatureCollection.h"
#include "FeatureCollectionLayer.h"
#include "FeatureCollectionTable.h"
#include "Map.h"
#include "MapQuickView.h"
#include "Portal.h"
#include "PortalItem.h"
#include "ServiceFeatureTable.h"

#include <QJsonValue>

using namespace Esri::ArcGISRuntime;
namespace
{
const QString featureCollectionItemId("0e3c8e86b4544274b45ecb61c9f41336");
}

DisplayRouteLayer::DisplayRouteLayer(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISTopographic, this))
{
  Portal* portal = new Portal(this);
  m_portalItem = new PortalItem(portal, featureCollectionItemId, this);

  connect(m_portalItem, &PortalItem::doneLoading, this, [this](const Error& loadError)
  {
    if (!loadError.isEmpty())
    {
      qWarning() << loadError.message() << loadError.additionalMessage();
      return;
    }

    // if the portal item is a feature collection, add the feature collection to the map's operational layers
    if (m_portalItem->type() == PortalItemType::FeatureCollection)
    {
      m_featureCollection = new FeatureCollection(m_portalItem, this);
      m_featureCollectionLayer = new FeatureCollectionLayer(m_featureCollection, this);
      m_map->operationalLayers()->append(m_featureCollectionLayer);
    }
    else
    {
      qWarning() << "Portal item with ID " << featureCollectionItemId << " is not a feature collection.";
    }
  });

  m_portalItem->load();
}

DisplayRouteLayer::~DisplayRouteLayer() = default;

void DisplayRouteLayer::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DisplayRouteLayer>("Esri.Samples", 1, 0, "DisplayRouteLayerSample");
}

MapQuickView* DisplayRouteLayer::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void DisplayRouteLayer::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);
  Point vpCenter(-122.8309, 45.2281, SpatialReference(4326));
  m_mapView->setViewpointCenter(vpCenter, 800000);

  emit mapViewChanged();
}

void DisplayRouteLayer::getDirections()
{
  if (!m_featureCollectionLayer)
    return;

  FeatureCollectionTableListModel* tables = m_featureCollection->tables();
  for (FeatureTable* table : *tables)
  {
    if (table->loadStatus() == LoadStatus::Loaded)
    {
      if (table->tableName() == "DirectionPoints")
      {

        m_directionsTable = table;

        connect(m_directionsTable, &ServiceFeatureTable::queryFeaturesCompleted, this, [this](QUuid, FeatureQueryResult* featureQueryResult)
        {
          if (!featureQueryResult)
          {
            return;
          }
          else
          {
            m_featureQueryResult = featureQueryResult;
          }

          while (featureQueryResult && featureQueryResult->iterator().hasNext())
          {
            m_feature = static_cast<ArcGISFeature*>(featureQueryResult->iterator().next(this));
            m_featureDirection = m_featureDirection + "\n" + m_feature->attributes()->attributeValue(QStringLiteral("DisplayText")).toString();
            qDebug() << m_featureDirection;
            featureQueryResult->iterator().next();
            emit directionsChanged();
          }
        });

        QueryParameters queryParams;
        queryParams.setWhereClause("1=1");
        m_directionsTable->queryFeatures(queryParams);
      }
    }
  }
}


QString DisplayRouteLayer::directions() const
{
  return m_featureDirection;
}
