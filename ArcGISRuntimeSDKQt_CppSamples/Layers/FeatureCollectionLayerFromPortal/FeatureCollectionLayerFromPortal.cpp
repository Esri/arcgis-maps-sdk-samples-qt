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
  m_portal(new Portal(QUrl("https://www.arcgis.com/"), this))
{
}

void FeatureCollectionLayerFromPortal::openFeatureCollection(const QString& itemId)
{
  if (m_portalItem != nullptr && m_portalItem->loadStatus() == LoadStatus::Loading)
  {
    m_messageText = "Portal item loading in progress";
    emit messageTextChanged();
    return;
  }

  const QString trimmedItemId = itemId.trimmed();
  if (itemId.isNull() || itemId.isEmpty())
  {
    m_messageText = "Please enter a portal item ID";
    emit messageTextChanged();
    return;
  }

  if (m_portalItem != nullptr)
  {
    delete m_portalItem;
    m_portalItem = nullptr;
  }

  m_portalItem = new PortalItem(m_portal, trimmedItemId, this);

  connect(m_portalItem, &PortalItem::doneLoading, this, [this](const Error& error){

    if (!error.isEmpty())
    {
      qDebug() << error.message() << error.additionalMessage();
      return;
    }

    if (m_portalItem->loadStatus() != LoadStatus::Loaded)
      return;

    if (m_portalItem->type() == PortalItemType::FeatureCollection)
    {
      // create a feature collection
      FeatureCollection* featureCollection = new FeatureCollection(m_portalItem, this);
      FeatureCollectionLayer* featureCollectionLayer = new FeatureCollectionLayer(featureCollection, this);
      m_map->operationalLayers()->append(featureCollectionLayer);
    }
    else
    {
      m_messageText = "Portal item with ID '" + m_portalItem->itemId() + "' is not a feature collection.";
      emit messageTextChanged();
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
