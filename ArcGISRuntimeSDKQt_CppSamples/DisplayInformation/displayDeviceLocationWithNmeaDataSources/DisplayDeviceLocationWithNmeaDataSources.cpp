// [WriteFile Name=DisplayDeviceLocationWithNmeaDataSources, Category=DisplayInformation]
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

#include "DisplayDeviceLocationWithNmeaDataSources.h"

#include "Location.h"
#include "LocationDisplay.h"
#include "NmeaLocation.h"
#include "NmeaLocationDataSource.h"
#include "SimulatedLocationDataSource.h"

#include "Map.h"
#include "MapQuickView.h"

#include <QFile>

using namespace Esri::ArcGISRuntime;

DisplayDeviceLocationWithNmeaDataSources::DisplayDeviceLocationWithNmeaDataSources(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISNavigationNight, this))
{
  m_nmeaLocationDataSource = new NmeaLocationDataSource(SpatialReference::wgs84(), this);
//  m_mockLocations = {};
//  m_mockDataSource = makeDataSource(m_mockLocations);
}

DisplayDeviceLocationWithNmeaDataSources::~DisplayDeviceLocationWithNmeaDataSources() = default;

void DisplayDeviceLocationWithNmeaDataSources::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DisplayDeviceLocationWithNmeaDataSources>("Esri.Samples", 1, 0, "DisplayDeviceLocationWithNmeaDataSourcesSample");
}

MapQuickView* DisplayDeviceLocationWithNmeaDataSources::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void DisplayDeviceLocationWithNmeaDataSources::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  start();

  emit mapViewChanged();
}

//SimulatedLocationDataSource* DisplayDeviceLocationWithNmeaDataSources::makeDataSource(QList<Location> locations)
//{
//  QFile simulatedNmeaDataFile(":/Samples/DisplayInformation/DisplayDeviceLocationWithNmeaDataSources/redlands.nmea");
//  qDebug() << simulatedNmeaDataFile.exists();

//  simulatedNmeaDataFile.open(QIODevice::ReadOnly);

//  QByteArray data = simulatedNmeaDataFile.readAll();
//  qDebug() << data;

//  m_nmeaLocationDataSource->pushData(data);


//  SimulatedLocationDataSource* mockDataSource = new SimulatedLocationDataSource(this);
//  mockDataSource->setLocations(locations);
//  return mockDataSource;
//}

void DisplayDeviceLocationWithNmeaDataSources::start() {

  QFile simulatedNmeaDataFile(":/Samples/DisplayInformation/DisplayDeviceLocationWithNmeaDataSources/redlands.nmea");
  qDebug() << "File found" << simulatedNmeaDataFile.exists();

  simulatedNmeaDataFile.open(QIODevice::ReadOnly);

  QByteArray data = simulatedNmeaDataFile.readAll();

  m_nmeaLocationDataSource->pushData(data);
  m_mapView->locationDisplay()->setDataSource(m_nmeaLocationDataSource);

  m_nmeaLocationDataSource->start();
  m_mapView->locationDisplay()->start();
  return;
}

void DisplayDeviceLocationWithNmeaDataSources::recenter() {
  return;
}

void DisplayDeviceLocationWithNmeaDataSources::reset() {
  return;
}

