// [WriteFile Name=ShowDeviceLocationUsingIndoorPositioning, Category=Maps]
// [Legal]
// Copyright 2022 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "ShowDeviceLocationUsingIndoorPositioning.h"

// ArcGIS Maps SDK headers
#include "Error.h"
#include "FeatureTable.h"
#include "FeatureTableListModel.h"
#include "FloorLevel.h"
#include "FloorManager.h"
#include "IndoorPositioningDefinition.h"
#include "IndoorsLocationDataSource.h"
#include "LocationDisplay.h"
#include "LocationSourcePropertiesKeys.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "MapViewTypes.h"
#include "PortalItem.h"

// Qt headers
#include <QMetaObject>
#include <QPermissions>

// Platform specific headers
#ifdef Q_OS_ANDROID
#include "ArcGISRuntimeEnvironment.h"

#include <QCoreApplication>
#include <QJniObject>
#endif

using namespace Esri::ArcGISRuntime;

namespace
{
  const QString itemId = "8fa941613b4b4b2b8a34ad4cdc3e4bba";
} // namespace

ShowDeviceLocationUsingIndoorPositioning::ShowDeviceLocationUsingIndoorPositioning(QObject* parent /* = nullptr */) :
  QObject(parent)
{
#ifdef Q_OS_ANDROID
  ArcGISRuntimeEnvironment::setAndroidApplicationContext(QJniObject{QNativeInterface::QAndroidApplication::context()});
#endif

  m_map = new Map(new PortalItem(itemId, this), this);
  connect(m_map, &Map::doneLoading, this, [this](const Error& loadError)
  {
    if (!loadError.isEmpty())
    {
      failSetup(loadError.message());
      return;
    }

    trySetupIndoorsLocationDataSource();
  });
}

ShowDeviceLocationUsingIndoorPositioning::~ShowDeviceLocationUsingIndoorPositioning() = default;

void ShowDeviceLocationUsingIndoorPositioning::stopLocationDisplay()
{
  if (m_mapView)
  {
    m_mapView->locationDisplay()->stop();
  }
}

void ShowDeviceLocationUsingIndoorPositioning::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ShowDeviceLocationUsingIndoorPositioning>("Esri.Samples", 1, 0, "ShowDeviceLocationUsingIndoorPositioningSample");
}

MapQuickView* ShowDeviceLocationUsingIndoorPositioning::mapView() const
{
  return m_mapView;
}

bool ShowDeviceLocationUsingIndoorPositioning::isLoading() const
{
  return m_isLoading;
}

// Set the view (created in QML)
void ShowDeviceLocationUsingIndoorPositioning::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  // workaround for https://bugreports.qt.io/browse/QTBUG-134211
  QMetaObject::invokeMethod(this, [this]()
  {
    requestBluetoothThenLocationPermissions();
  }, Qt::QueuedConnection);

  emit mapViewChanged();
}

void ShowDeviceLocationUsingIndoorPositioning::requestBluetoothThenLocationPermissions()
{
  qApp->requestPermission(QBluetoothPermission{}, [this](const QPermission& permission)
  {
    Q_UNUSED(permission);
    requestLocationPermissionThenSetupILDS();
  });
}

void ShowDeviceLocationUsingIndoorPositioning::requestLocationPermissionThenSetupILDS()
{
  QLocationPermission locationPermission{};
  locationPermission.setAccuracy(QLocationPermission::Accuracy::Precise);
  locationPermission.setAvailability(QLocationPermission::Availability::WhenInUse);
  qApp->requestPermission(locationPermission, [this](const QPermission& permission)
  {
    Q_UNUSED(permission);
    validatePermissions();
    trySetupIndoorsLocationDataSource();
  });
}

void ShowDeviceLocationUsingIndoorPositioning::validatePermissions()
{
  bool bluetoothPermissionGranted = false;
  bool permissionsDenied = false;
  if (const Qt::PermissionStatus status = qApp->checkPermission(QBluetoothPermission{}); status == Qt::PermissionStatus::Denied)
  {
    permissionsDenied = true;
    emit bluetoothPermissionDenied();
  }
  else if (status == Qt::PermissionStatus::Granted)
  {
    bluetoothPermissionGranted = true;
  }

  bool locationPermissionGranted = false;
  QLocationPermission locationPermission{};
  locationPermission.setAccuracy(QLocationPermission::Accuracy::Precise);
  locationPermission.setAvailability(QLocationPermission::Availability::WhenInUse);
  if (const Qt::PermissionStatus status = qApp->checkPermission(locationPermission); status == Qt::PermissionStatus::Denied)
  {
    permissionsDenied = true;
    emit locationPermissionDenied();
  }
  else if (status == Qt::PermissionStatus::Granted)
  {
    locationPermissionGranted = true;
  }

  if (bluetoothPermissionGranted && locationPermissionGranted)
  {
    m_allPermissionsGranted = true;
  }
  else if (permissionsDenied)
  {
    setLoading(false);
  }
}

void ShowDeviceLocationUsingIndoorPositioning::setLoading(bool isLoading)
{
  if (m_isLoading == isLoading)
  {
    return;
  }

  m_isLoading = isLoading;
  emit isLoadingChanged();
}

void ShowDeviceLocationUsingIndoorPositioning::trySetupIndoorsLocationDataSource()
{
  if (!m_mapView || m_setupFailed || m_isSettingUpIndoors || m_indoorsLocationDataSource)
  {
    return;
  }

  // we must have both the map loaded and all permissions granted
  if (m_map->loadStatus() != LoadStatus::Loaded || !m_allPermissionsGranted)
  {
    return;
  }

  IndoorPositioningDefinition* indoorPositioningDef = m_map->indoorPositioningDefinition();
  if (!indoorPositioningDef)
  {
    failSetup(tr("Cannot initialize indoors location data source."));
    return;
  }

  m_isSettingUpIndoors = true;
  setLoading(true);
  m_pendingPrerequisiteLoads = 0;

  for (FeatureTable* table : *m_map->tables())
  {
    if (!table)
    {
      continue;
    }

    if (table->loadStatus() == LoadStatus::Loaded)
    {
      continue;
    }

    if (table->loadStatus() == LoadStatus::FailedToLoad)
    {
      failSetup(table->loadError().message());
      return;
    }

    ++m_pendingPrerequisiteLoads;
    connect(table, &FeatureTable::doneLoading, this, &ShowDeviceLocationUsingIndoorPositioning::handlePrerequisiteDoneLoading);
    if (table->loadStatus() == LoadStatus::NotLoaded)
    {
      table->load();
    }
  }

  if (FloorManager* floorManager = m_map->floorManager())
  {
    if (floorManager->loadStatus() == LoadStatus::FailedToLoad)
    {
      failSetup(floorManager->loadError().message());
      return;
    }

    if (floorManager->loadStatus() != LoadStatus::Loaded)
    {
      ++m_pendingPrerequisiteLoads;
      connect(floorManager, &FloorManager::doneLoading, this, &ShowDeviceLocationUsingIndoorPositioning::handlePrerequisiteDoneLoading);
      if (floorManager->loadStatus() == LoadStatus::NotLoaded)
      {
        floorManager->load();
      }
    }
  }

  if (indoorPositioningDef->loadStatus() == LoadStatus::FailedToLoad)
  {
    failSetup(indoorPositioningDef->loadError().message());
    return;
  }

  if (indoorPositioningDef->loadStatus() != LoadStatus::Loaded)
  {
    ++m_pendingPrerequisiteLoads;
    connect(indoorPositioningDef, &IndoorPositioningDefinition::doneLoading, this, &ShowDeviceLocationUsingIndoorPositioning::handlePrerequisiteDoneLoading);
    if (indoorPositioningDef->loadStatus() == LoadStatus::NotLoaded)
    {
      indoorPositioningDef->load();
    }
  }

  if (m_pendingPrerequisiteLoads == 0)
  {
    completeIndoorsLocationSetup();
  }
}

void ShowDeviceLocationUsingIndoorPositioning::handlePrerequisiteDoneLoading(const Error& loadError)
{
  if (m_setupFailed || !m_isSettingUpIndoors)
  {
    return;
  }

  if (!loadError.isEmpty())
  {
    failSetup(loadError.message());
    return;
  }

  if (m_pendingPrerequisiteLoads > 0)
  {
    --m_pendingPrerequisiteLoads;
  }

  if (m_pendingPrerequisiteLoads == 0)
  {
    completeIndoorsLocationSetup();
  }
}

void ShowDeviceLocationUsingIndoorPositioning::completeIndoorsLocationSetup()
{
  if (m_setupFailed || m_indoorsLocationDataSource || !m_mapView)
  {
    return;
  }

  IndoorPositioningDefinition* indoorPositioningDef = m_map->indoorPositioningDefinition();
  if (!indoorPositioningDef || indoorPositioningDef->loadStatus() != LoadStatus::Loaded)
  {
    failSetup(tr("Cannot initialize indoors location data source."));
    return;
  }

  showGroundFloor();
  m_hasReceivedFirstLocation = false;

  m_indoorsLocationDataSource = new IndoorsLocationDataSource(indoorPositioningDef, this);
  LocationDisplay* locationDisplay = m_mapView->locationDisplay();

  connect(locationDisplay, &LocationDisplay::locationChanged, this, &ShowDeviceLocationUsingIndoorPositioning::locationChangedHandler);

  locationDisplay->setDataSource(m_indoorsLocationDataSource);
  locationDisplay->setAutoPanMode(LocationDisplayAutoPanMode::CompassNavigation);
  locationDisplay->start();
  m_isSettingUpIndoors = false;
}

void ShowDeviceLocationUsingIndoorPositioning::showGroundFloor()
{
  FloorManager* floorManager = m_map->floorManager();
  if (!floorManager || floorManager->loadStatus() != LoadStatus::Loaded)
  {
    return;
  }

  for (FloorLevel* level : floorManager->levels())
  {
    if (level)
    {
      level->setVisible(level->verticalOrder() == 0);
    }
  }
}

void ShowDeviceLocationUsingIndoorPositioning::failSetup(const QString& errorMessage)
{
  if (m_setupFailed)
  {
    return;
  }

  m_setupFailed = true;
  m_isSettingUpIndoors = false;
  m_pendingPrerequisiteLoads = 0;
  setLoading(false);
  emit errorOccurred(errorMessage.isEmpty() ? tr("Cannot initialize indoors location data source.") : errorMessage);
}

// Change currently displayed location information and change floor display if necessary
void ShowDeviceLocationUsingIndoorPositioning::locationChangedHandler(const Location& loc)
{
  m_locationProperties = loc.additionalSourceProperties();
  m_locationProperties["horizontalAccuracy"] = QVariant::fromValue(loc.horizontalAccuracy());

  if (!m_hasReceivedFirstLocation)
  {
    m_hasReceivedFirstLocation = true;
    setLoading(false);
  }

  const QVariant floor = m_locationProperties.value(LocationSourcePropertiesKeys::floor());
  const QString floorLevelId = m_locationProperties.value(LocationSourcePropertiesKeys::floorLevelId()).toString();
  if (floor.isValid() && !floorLevelId.isEmpty())
  {
    changeFloorDisplay(floorLevelId);
  }

  emit locationPropertiesChanged();
}

void ShowDeviceLocationUsingIndoorPositioning::changeFloorDisplay(const QString& floorLevelId)
{
  FloorManager* floorManager = m_map->floorManager();
  if (!floorManager || floorManager->loadStatus() != LoadStatus::Loaded)
  {
    return;
  }

  for (FloorLevel* level : floorManager->levels())
  {
    if (level)
    {
      level->setVisible(level->levelId() == floorLevelId);
    }
  }
}
