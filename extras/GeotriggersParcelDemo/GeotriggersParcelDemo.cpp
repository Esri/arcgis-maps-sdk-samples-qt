// [WriteFile Name=GeotriggersParcelDemo, Category=Analysis]
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

#include "GeotriggersParcelDemo.h"

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
#include "Geodatabase.h"
#include "GeodatabaseFeatureTable.h"

#include <QDir>

using namespace Esri::ArcGISRuntime;

GeotriggersParcelDemo::GeotriggersParcelDemo(QObject* parent /* = nullptr */):
  QObject(parent)
{
}

GeotriggersParcelDemo::~GeotriggersParcelDemo() = default;

void GeotriggersParcelDemo::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<GeotriggersParcelDemo>("Esri.Samples", 1, 0, "GeotriggersParcelDemoSample");
}

MapQuickView* GeotriggersParcelDemo::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void GeotriggersParcelDemo::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;

  loadMmpk();

  emit mapViewChanged();
}

void GeotriggersParcelDemo::loadMmpk()
{
  MobileMapPackage* mmpk = new MobileMapPackage(QDir::homePath() + "/ArcGIS/Runtime/Data/mmpk/parcels.mmpk", this);

  connect(mmpk, &MobileMapPackage::doneLoading, this, [mmpk, this]()
  {
    m_map = mmpk->maps().at(0);
    m_mapView->setMap(m_map);
    m_parcelsLayer = dynamic_cast<FeatureLayer*>(m_map->operationalLayers()->at(0));
    m_parcelsTable = m_parcelsLayer->featureTable();
    m_parcelsTable->load();

    initializeSimulatedLocationDisplay();
    runGeotriggers();
  });

  mmpk->load();
}

void GeotriggersParcelDemo::runGeotriggers()
{
  m_geotriggerFeed = new LocationGeotriggerFeed(m_locationDataSource, this);
  m_geotriggerFeed->setFilter(new ArcadeExpression("$locationupdate.horizontalaccuracy <= 10", this));

  FeatureFenceParameters* featureFenceParameters = new FeatureFenceParameters(m_parcelsTable, this);
  //featureFenceParameters->setWhereClause("RecAC > .18");

  FenceGeotrigger* fenceGeotrigger = new FenceGeotrigger(m_geotriggerFeed, FenceRuleType::EnterOrExit, featureFenceParameters, this);
  fenceGeotrigger->setFeedAccuracyMode(FenceGeotriggerFeedAccuracyMode::UseGeometryWithAccuracy);
  //fenceGeotrigger->setEnterExitSpatialRelationship(FenceEnterExitSpatialRelationship::EnterContainsAndExitDoesNotContain);

  GeotriggerMonitor* geotriggerMonitor = new GeotriggerMonitor(fenceGeotrigger, this);

  connect(geotriggerMonitor, &GeotriggerMonitor::geotriggerNotification, this, [this](GeotriggerNotificationInfo* geotriggerNotificationInfo)
  {
    FenceGeotriggerNotificationInfo* fenceGeotriggerNotificationInfo = static_cast<FenceGeotriggerNotificationInfo*>(geotriggerNotificationInfo);
    if (fenceGeotriggerNotificationInfo->fenceNotificationType() == FenceNotificationType::Entered)
    {
      m_parcelsLayer->selectFeature(dynamic_cast<Feature*>(fenceGeotriggerNotificationInfo->fenceGeoElement()));
    }
    else
    {
      m_parcelsLayer->clearSelection();
    }
  });

  geotriggerMonitor->start();

  delay();

  m_mapView->locationDisplay()->start();
  m_locationDataSource->start();
}

void GeotriggersParcelDemo::delay()
{
    QTime dieTime= QTime::currentTime().addSecs(10);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void GeotriggersParcelDemo::initializeSimulatedLocationDisplay()
{
  m_locationDataSource = new SimulatedLocationDataSource(this);

  // Create SimulationParameters starting at the current time, a velocity of 5 m/s, and a horizontal and vertical accuracy of 0.0
  SimulationParameters* simulationParameters = new SimulationParameters(QDateTime::currentDateTime(), 50, 0.0, 0.0, this);

  PolylineBuilder polylineBuilder(SpatialReference::webMercator());
  polylineBuilder.addPoint(-1.29641e+07, 4.00798e+06);
  polylineBuilder.addPoint(-1.29657e+07, 4.00224e+06);
  polylineBuilder.addPoint(-1.2966e+07,4.00609e+06);

  m_locationDataSource->setLocationsWithPolyline(polylineBuilder.toPolyline(), simulationParameters);

  m_mapView->locationDisplay()->setDataSource(m_locationDataSource);
  m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Recenter);
  m_mapView->locationDisplay()->start();
  m_locationDataSource->stop();
}
