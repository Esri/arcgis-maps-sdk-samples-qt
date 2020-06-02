// [WriteFile Name=FeatureCollectionLayerFromPortal, Category=Layers]
// [Legal]
// Copyright 2020 Esri.

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

#include "FeatureCollectionLayerFromPortal.h"

#include "FeatureCollection.h"
#include "FeatureCollectionLayer.h"
#include "Map.h"
#include "MapQuickView.h"
#include "Portal.h"
#include "PortalItem.h"

using namespace Esri::ArcGISRuntime;

FeatureCollectionLayerFromPortal::FeatureCollectionLayerFromPortal(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::oceans(this), this)),
  m_itemId("32798dfad17942858d5eef82ee802f0b")
{
  const QUrl url("https://www.arcgis.com/");
  m_portal = new Portal(url, this);

//  connect(m_portalItem, &PortalItem::doneLoading, this, [this](const Error& error){
//    qDebug("inside connection");

//    if (!error.isEmpty())
//    {
//      qDebug() << error.message() << error.additionalMessage();
//      return;
//    }

//    if (m_portalItem->loadStatus() != LoadStatus::Loaded)
//      return;

//    if (m_portalItem->type() == PortalItemType::FeatureCollection)
//    {
//      qDebug("is a feature collection");
//      // create a feature collection
//      FeatureCollection* featureCollection = new FeatureCollection(m_portalItem);
//      FeatureCollectionLayer* featureCollectionLayer = new FeatureCollectionLayer(featureCollection, this);
//      m_map->operationalLayers()->append(featureCollectionLayer);
//    }
//    else
//    {
//      qDebug() << "Portal item with ID '" << m_itemId + "' is not a feature collection.";
//    }
//  });
}

void FeatureCollectionLayerFromPortal::openFeatureCollection(const QString itemId)
{
  QString trimmedItemId = itemId.trimmed();
  if (itemId.isNull() || itemId.isEmpty())
  {
    qDebug("Please enter a portal item ID");
    return;
  }

  if (m_portalItem != nullptr)
  {
    qDebug("deleted portal item");
    delete m_portalItem;
  }

  m_portalItem = new PortalItem(m_portal, trimmedItemId, this);

  connect(m_portalItem, &PortalItem::doneLoading, this, [this, trimmedItemId](const Error& error){
    qDebug("inside connection");

    if (!error.isEmpty())
    {
      qDebug() << error.message() << error.additionalMessage();
      return;
    }

    if (m_portalItem->loadStatus() != LoadStatus::Loaded)
      return;

    if (m_portalItem->type() == PortalItemType::FeatureCollection)
    {
      qDebug("is a feature collection");
      // create a feature collection
      FeatureCollection* featureCollection = new FeatureCollection(m_portalItem);
      FeatureCollectionLayer* featureCollectionLayer = new FeatureCollectionLayer(featureCollection, this);
      m_map->operationalLayers()->append(featureCollectionLayer);
    }
    else
    {
      qDebug() << "Portal item with ID '" << trimmedItemId + "' is not a feature collection.";
    }
  });
m_portalItem->load();
}

FeatureCollectionLayerFromPortal::~FeatureCollectionLayerFromPortal() = default;

void FeatureCollectionLayerFromPortal::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<FeatureCollectionLayerFromPortal>("Esri.Samples", 1, 0, "FeatureCollectionLayerFromPortalSample");
}

MapQuickView* FeatureCollectionLayerFromPortal::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void FeatureCollectionLayerFromPortal::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}
