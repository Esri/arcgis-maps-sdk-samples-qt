// [WriteFile Name=ClusterFeatures, Category=DisplayInformation]
// [Legal]
// Copyright 2023 Esri.

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

#include "ClusterFeatures.h"

#include "AggregateGeoElement.h"
#include "AttributeListModel.h"
#include "CalloutData.h"
#include "Error.h"
#include "Feature.h"
#include "FeatureLayer.h"
#include "FeatureReduction.h"
#include "FeatureTable.h"
#include "GeoElement.h"
#include "IdentifyLayerResult.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "Popup.h"
#include "PortalItem.h"

#include <QFuture>

using namespace Esri::ArcGISRuntime;

ClusterFeatures::ClusterFeatures(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(new PortalItem("8916d50c44c746c1aafae001552bad23", this), this))
{
  connect(m_map, &Map::doneLoading, this, [this](const Error& e)
          {
            if (!e.isEmpty())
            {
              qWarning() << e.message() << e.additionalMessage();
              return;
            }
            m_powerPlantsLayer = static_cast<FeatureLayer*>(m_map->operationalLayers()->first());
            m_taskRunning = false;
            emit taskRunningChanged();
          });
}

ClusterFeatures::~ClusterFeatures() = default;

void ClusterFeatures::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ClusterFeatures>("Esri.Samples", 1, 0, "ClusterFeaturesSample");
}

MapQuickView* ClusterFeatures::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ClusterFeatures::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](const QMouseEvent& mouseClick)
          {
            if (m_map->loadStatus() != LoadStatus::Loaded || m_taskRunning)
              return;

            if (!m_powerPlantsLayer)
              m_powerPlantsLayer = static_cast<FeatureLayer*>(m_map->operationalLayers()->first());

            m_taskRunning = true;
            emit taskRunningChanged();

            m_mapView->calloutData()->setVisible(false);

            m_mapView->identifyLayerAsync(m_powerPlantsLayer, mouseClick.position(), 3, false)
                .then(this, [this](IdentifyLayerResult* rawIdentifyResult)
                      {
                        m_taskRunning = false;
                        emit taskRunningChanged();

                        auto identifyResult = std::unique_ptr<IdentifyLayerResult>(rawIdentifyResult);

                        // Invalid identify result
                        if (!identifyResult)
                          return;

                        if (!identifyResult->error().isEmpty())
                        {
                          qDebug() << "Identify error occurred: " << identifyResult->error().message();
                          return;
                        }

                        if (identifyResult->geoElements().isEmpty())
                          return;


                        GeoElement* geoElement = identifyResult->geoElements().first();
                        if (AggregateGeoElement* aggregateGeoElement = dynamic_cast<AggregateGeoElement*>(geoElement))
                        {
                          const auto aggregateAttributes = aggregateGeoElement->attributes()->attributesMap();
                          qDebug() << aggregateAttributes;

                          m_mapView->calloutData()->setTitle("Cluster summary");
                          m_mapView->calloutData()->setDetail(
                              "This cluster represents " + aggregateAttributes.value("cluster_count").toString() + " features.\n" +
                              "The predominant fuel type for generating power within this cluster is: " + aggregateAttributes.value("fuel1").toString());
                        }
                        else
                        {
                          Feature* feature = static_cast<Feature*>(geoElement);

                          const auto attributes = feature->attributes()->attributesMap();
                          qDebug() << attributes;

                          m_mapView->calloutData()->setTitle(attributes.value("name").toString());
                          m_mapView->calloutData()->setDetail("This is an individual power plant with fuel type: "+ attributes.value("fuel1").toString());
                        }

                        m_mapView->calloutData()->setLocation(Point(geoElement->geometry()));
                        m_mapView->calloutData()->setVisible(true);
                      });
          });

  emit mapViewChanged();
}

bool ClusterFeatures::toggleClustering()
{
  if (m_map->loadStatus() != LoadStatus::Loaded)
    return false;

  if (!m_powerPlantsLayer)
    m_powerPlantsLayer = static_cast<FeatureLayer*>(m_map->operationalLayers()->first());

  m_powerPlantsLayer->featureReduction()->setEnabled(!m_powerPlantsLayer->featureReduction()->isEnabled());

  return true;
}

bool ClusterFeatures::taskRunning() const
{
  return m_taskRunning;
}
