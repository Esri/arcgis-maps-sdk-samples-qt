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
#include "FeatureLayer.h"
#include "IndoorsLocationDataSource.h"
#include "LayerListModel.h"
#include "LocationDisplay.h"
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
  const QStringList layerNames = {"Details", "Units", "Levels"};
} // namespace

ShowDeviceLocationUsingIndoorPositioning::ShowDeviceLocationUsingIndoorPositioning(QObject* parent /* = nullptr */) :
  QObject(parent)
{
  m_map = new Map(new PortalItem(itemId, this), this);
  connect(m_map, &Map::doneLoading, this, [this](const Error& loadError)
  {
    if (!loadError.isEmpty())
    {
      return;
    }

    trySetupIndoorsLocationDataSource();
  });
}

ShowDeviceLocationUsingIndoorPositioning::~ShowDeviceLocationUsingIndoorPositioning() = default;

void ShowDeviceLocationUsingIndoorPositioning::stopLocationDisplay()
{
  m_mapView->locationDisplay()->stop();
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
  if (const Qt::PermissionStatus status = qApp->checkPermission(QBluetoothPermission{}); status == Qt::PermissionStatus::Denied)
  {
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
}

void ShowDeviceLocationUsingIndoorPositioning::trySetupIndoorsLocationDataSource()
{
  // we must have both the map loaded and all permissions granted
  if (m_indoorsLocationDataSource || (m_map->loadStatus() != LoadStatus::Loaded || !m_allPermissionsGranted))
  {
    return;
  }

#ifdef Q_OS_ANDROID
  ArcGISRuntimeEnvironment::setAndroidApplicationContext(QJniObject{QNativeInterface::QAndroidApplication::context()});
#endif

  IndoorPositioningDefinition* indoorPositioningDef = m_map->indoorPositioningDefinition();
  if (!indoorPositioningDef)
  {
    qDebug() << "The map is missing an indoor positioning definition";
    return;
  }

  m_indoorsLocationDataSource = new IndoorsLocationDataSource(indoorPositioningDef, this);
  LocationDisplay* locationDisplay = m_mapView->locationDisplay();

  connect(locationDisplay, &LocationDisplay::locationChanged, this, &ShowDeviceLocationUsingIndoorPositioning::locationChangedHandler);

  locationDisplay->setDataSource(m_indoorsLocationDataSource);
  locationDisplay->setAutoPanMode(LocationDisplayAutoPanMode::Navigation);
  locationDisplay->start();
}

// Change currently displayed location information and change floor display if necessary
void ShowDeviceLocationUsingIndoorPositioning::locationChangedHandler(const Location& loc)
{
  if (m_locationProperties["floor"] != m_currentFloor)
  {
    m_currentFloor = m_locationProperties["floor"].toInt();
    changeFloorDisplay();
  }
  m_locationProperties = loc.additionalSourceProperties();
  m_locationProperties["horizontalAccuracy"] = QVariant::fromValue(loc.horizontalAccuracy());

  emit locationPropertiesChanged();
}

void ShowDeviceLocationUsingIndoorPositioning::changeFloorDisplay()
{
  for (Layer* layer : *(m_map->operationalLayers()))
  {
    if (layerNames.contains(layer->name()))
    {
      if (layer->layerType() == LayerType::FeatureLayer)
      {
        FeatureLayer* featureLayer = static_cast<FeatureLayer*>(layer);
        featureLayer->setDefinitionExpression(QString{"VERTICAL_ORDER = %1"}.arg(m_currentFloor));
      }
    }
  }
}

QVariantMap ShowDeviceLocationUsingIndoorPositioning::locationProperties() const
{
  return m_locationProperties;
}
