// [WriteFile Name=GeotriggersToggleLayersDemo, Category=Analysis]
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

#include "GeotriggersToggleLayersDemo.h"

#include "Map.h"
#include "MapQuickView.h"
#include "MobileMapPackage.h"

#include "ArcadeExpression.h"
#include "SimulatedLocationDataSource.h"
#include "SimulationParameters.h"
#include "PolylineBuilder.h"
#include "LocationGeotriggerFeed.h"
#include "FenceGeotrigger.h"
#include "FeatureFenceParameters.h"
#include "GeotriggerMonitor.h"
#include "FenceGeotriggerNotificationInfo.h"
#include "GeotriggerNotificationInfo.h"
#include "GeotriggersTypes.h"
#include "FeatureTable.h"
#include "ServiceFeatureTable.h"
#include "FeatureLayer.h"
#include "GroupLayer.h"

#include <QDir>

using namespace Esri::ArcGISRuntime;

GeotriggersToggleLayersDemo::GeotriggersToggleLayersDemo(QObject* parent /* = nullptr */):
  QObject(parent)
{
}

GeotriggersToggleLayersDemo::~GeotriggersToggleLayersDemo() = default;

void GeotriggersToggleLayersDemo::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<GeotriggersToggleLayersDemo>("Esri.Samples", 1, 0, "GeotriggersToggleLayersDemoSample");
}

MapQuickView* GeotriggersToggleLayersDemo::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void GeotriggersToggleLayersDemo::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;

  loadMmpk();

  emit mapViewChanged();
}

void GeotriggersToggleLayersDemo::loadMmpk()
{
  MobileMapPackage* mmpk = new MobileMapPackage(QDir::homePath() + "/ArcGIS/Runtime/Data/mmpk/Geotrigger_indoor_action_switch.mmpk", this);

  connect(mmpk, &MobileMapPackage::doneLoading, this, [mmpk, this]()
  {
    m_map = mmpk->maps().at(0);
    m_mapView->setMap(m_map);

    m_indoorLayers = dynamic_cast<GroupLayer*>(m_map->operationalLayers()->at(1));

    FeatureLayer* buildingFootprint = dynamic_cast<FeatureLayer*>(m_indoorLayers->layers()->at(0));

    m_buildingFootprintTable = buildingFootprint->featureTable();

    initializeSimulatedLocationDisplay();
    runGeotriggers();
  });

  mmpk->load();
}

void GeotriggersToggleLayersDemo::runGeotriggers()
{
  m_geotriggerFeed = new LocationGeotriggerFeed(m_locationDataSource, this);

  FeatureFenceParameters* featureFenceParameters = new FeatureFenceParameters(m_buildingFootprintTable, this);

  FenceGeotrigger* fenceGeotrigger = new FenceGeotrigger(m_geotriggerFeed, FenceRuleType::EnterOrExit, featureFenceParameters, this);

  GeotriggerMonitor* geotriggerMonitor = new GeotriggerMonitor(fenceGeotrigger, this);

  connect(geotriggerMonitor, &GeotriggerMonitor::geotriggerNotification, this, [this](GeotriggerNotificationInfo* geotriggerNotificationInfo)
  {
    FenceGeotriggerNotificationInfo* fenceGeotriggerNotificationInfo = static_cast<FenceGeotriggerNotificationInfo*>(geotriggerNotificationInfo);
    if (fenceGeotriggerNotificationInfo->fenceNotificationType() == FenceNotificationType::Entered)
    {
      qDebug() << "hi";
      m_indoorLayers->setVisible(true);
      m_map->operationalLayers()->at(0)->setVisible(false);
    }
    else
    {
      m_indoorLayers->setVisible(false);
      m_map->operationalLayers()->at(0)->setVisible(true);
    }
  });

  geotriggerMonitor->start();
}

void GeotriggersToggleLayersDemo::displayInteriorLayers(bool display)
{
  if (display)
  {
    m_indoorLayers->setVisible(true);
    m_map->operationalLayers()->at(0)->setVisible(false);
  }
  else
  {
    m_indoorLayers->setVisible(false);
    m_map->operationalLayers()->at(0)->setVisible(true);
  }
}

void GeotriggersToggleLayersDemo::initializeSimulatedLocationDisplay()
{
  m_locationDataSource = new SimulatedLocationDataSource(this);

  // Create SimulationParameters starting at the current time, a velocity of 5 m/s, and a horizontal and vertical accuracy of 0.0
  SimulationParameters* simulationParameters = new SimulationParameters(QDateTime::currentDateTime(), 10, 0.0, 0.0, this);

  PolylineBuilder polylineBuilder(SpatialReference::webMercator());
  polylineBuilder.addPoint(-1.297285e+07, 4.005405e+06);
  polylineBuilder.addPoint(-1.29729e+07, 4.005405e+06);
  polylineBuilder.addPoint(-1.29731e+07, 4.0054e+06);

  m_locationDataSource->setLocationsWithPolyline(polylineBuilder.toPolyline(), simulationParameters);

  m_mapView->locationDisplay()->setDataSource(m_locationDataSource);
  m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Recenter);
  m_mapView->locationDisplay()->setInitialZoomScale(2000);
  m_mapView->locationDisplay()->start();

  m_locationDataSource->start();
}
