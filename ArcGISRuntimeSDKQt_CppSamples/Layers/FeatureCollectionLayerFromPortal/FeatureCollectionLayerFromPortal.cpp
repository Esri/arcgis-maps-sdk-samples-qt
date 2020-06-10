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
namespace
{
const QString featureCollectionItemId("32798dfad17942858d5eef82ee802f0b");
}

FeatureCollectionLayerFromPortal::FeatureCollectionLayerFromPortal(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::oceans(this), this))
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
      FeatureCollection* featureCollection = new FeatureCollection(m_portalItem, this);
      FeatureCollectionLayer* featureCollectionLayer = new FeatureCollectionLayer(featureCollection, this);
      m_map->operationalLayers()->append(featureCollectionLayer);
    }
    else
    {
      qWarning() << "Portal item with ID " << featureCollectionItemId << " is not a feature collection.";
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
