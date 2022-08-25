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

      connect(m_featureCollection, &FeatureCollection::loadStatusChanged, this, [this]()
      {
        if (m_featureCollection->loadStatus() == LoadStatus::Loaded)
        {
          qDebug() << m_featureCollection->tables()->size();

          FeatureCollectionTableListModel* tables = m_featureCollection->tables();
          for (FeatureTable* table : *tables)
          {
            if (table->loadStatus() == LoadStatus::Loaded)
            {
              if (table->tableName() == "DirectionPoints")
              {
                qDebug() << "Located DirectionPoints";
                auto feature = table->createFeature(this);
                auto attributes = feature->attributes();
                if (attributes)
                {
                  for (const auto attr : attributes->attributeNames())
                  {
                    if (attr == "DisplayText")
                    {
                      qDebug() << "hi";
                      auto value = attributes->attributeValue(attr);
                      auto str_val = value.toString();
                      qDebug() << str_val;
//                      if (str_val)
//                      {
//                        qDebug() << str_val;
//                      }
                    }
                  }
                }
              }

            }
          }
        }
      });
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

  //  Viewpoint vpCenter = Viewpoint(Point(123.0304, 35.338/*8,  SpatialReference::wgs84()*/), 100000);
  //  m_mapView->setViewpoint(vpCenter);

  constexpr double xMin = -10338668.348591767;
  constexpr double yMin = 5546908.424239618;
  constexpr double xMax = -18388247.594362013;
  constexpr double yMax = 2047223.989911933;
  constexpr int wkid = 102100;
  Viewpoint vpSpring(Envelope(xMin, yMin, xMax, yMax, SpatialReference(wkid)));
  constexpr float duration = 4.0f;
  //  m_mapView->setViewpoint/*Animated*/(vpSpring/*, duration, AnimationCurve::EaseInOutCubic*/);

  //122.8330046°W 45.2034128°N
  Viewpoint initialViewpoint(Point(122.8330046, 45.2034128, SpatialReference::wgs84()), 800000);
  //  m_map->setInitialViewpoint(initialViewpoint);

  //  m_mapView->

  emit mapViewChanged();
}
