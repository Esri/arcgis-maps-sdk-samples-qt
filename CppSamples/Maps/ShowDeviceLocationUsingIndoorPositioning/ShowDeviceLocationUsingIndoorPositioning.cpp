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
#include "IndoorsLocationDataSourceCreator.h"
#include "ShowDeviceLocationUsingIndoorPositioning.h"

// ArcGIS Maps SDK headers
#include "FeatureLayer.h"
#include "IndoorsLocationDataSource.h"
#include "LayerListModel.h"
#include "LocationDisplay.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "MapViewTypes.h"
#include "PortalItem.h"

#include <QPermissions>

#ifdef Q_OS_ANDROID
#include "ArcGISRuntimeEnvironment.h"

#include <QCoreApplication>
#include <QJniObject>
#endif

using namespace Esri::ArcGISRuntime;

namespace {
const QString itemId = "8fa941613b4b4b2b8a34ad4cdc3e4bba";

const QString positioningTableName = "ips_positioning";
const QString pathwaysLayerName = "Pathways";
const QStringList globalIdSortNames = {"DateCreated", "DATE_CREATED"};

const QStringList layerNames = {"Details", "Units", "Levels"};
}

ShowDeviceLocationUsingIndoorPositioning::ShowDeviceLocationUsingIndoorPositioning(QObject* parent /* = nullptr */):
  QObject(parent)
{
  m_map = new Map(new PortalItem(itemId, this), this);
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
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  requestBluetoothThenLocationPermissions();

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
    checkPermissions();
    setupIndoorsLocationDataSource();
  });
}

void ShowDeviceLocationUsingIndoorPositioning::checkPermissions()
{
  if (qApp->checkPermission(QBluetoothPermission{}) == Qt::PermissionStatus::Denied)
  {
    emit bluetoothPermissionDenied();
  }

  QLocationPermission locationPermission{};
  locationPermission.setAccuracy(QLocationPermission::Accuracy::Precise);
  locationPermission.setAvailability(QLocationPermission::Availability::WhenInUse);
  if (qApp->checkPermission(locationPermission) == Qt::PermissionStatus::Denied)
  {
    emit locationPermissionDenied();
  }
}

// This function uses a helper class `IndoorsLocationDataSourceCreator` to construct the IndoorsLocationDataSource
void ShowDeviceLocationUsingIndoorPositioning::setupIndoorsLocationDataSource()
{
  #ifdef Q_OS_ANDROID
  ArcGISRuntimeEnvironment::setAndroidApplicationContext(QJniObject{QNativeInterface::QAndroidApplication::context()});
  #endif

  IndoorsLocationDataSourceCreator* indoorsLocationDataSourceCreator = new IndoorsLocationDataSourceCreator(this);

  connect(indoorsLocationDataSourceCreator, &IndoorsLocationDataSourceCreator::createIndoorsLocationDataSourceCompleted, this, [this](IndoorsLocationDataSource* indoorsLDS)
  {
    connect(m_mapView->locationDisplay(), &LocationDisplay::locationChanged, this, &ShowDeviceLocationUsingIndoorPositioning::locationChangedHandler);

    m_mapView->locationDisplay()->setDataSource(indoorsLDS);
    m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Navigation);
    m_mapView->locationDisplay()->start();
  });

  indoorsLocationDataSourceCreator->createIndoorsLocationDataSource(m_map, positioningTableName, pathwaysLayerName);
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
