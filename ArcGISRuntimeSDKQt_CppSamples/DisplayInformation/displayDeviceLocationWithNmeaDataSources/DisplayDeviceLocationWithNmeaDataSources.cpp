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
#include "SimulationParameters.h"

#include "Map.h"
#include "MapQuickView.h"

#include <QFile>
#include <QThread>

using namespace Esri::ArcGISRuntime;

DisplayDeviceLocationWithNmeaDataSources::DisplayDeviceLocationWithNmeaDataSources(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISNavigationNight, this))
{
  m_nmeaLocationDataSource = new NmeaLocationDataSource(SpatialReference::wgs84(), this);
  m_simulatedLocationDataSource = new SimulatedLocationDataSource(this);

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

  emit mapViewChanged();
  start();
}

void DisplayDeviceLocationWithNmeaDataSources::start() {
  m_mockDataFile.setFileName(":/Samples/DisplayInformation/DisplayDeviceLocationWithNmeaDataSources/redlands.nmea");
  qDebug() << "File found" << m_mockDataFile.exists();

  m_mockDataFile.open(QIODevice::ReadOnly);

  m_mockData = m_mockDataFile.readAll();

  SimulationParameters* simulationParameters = new SimulationParameters(QDateTime::currentDateTime(), 30.0, 0.0, 0.0, this);
  m_mapView->locationDisplay()->setDataSource(m_nmeaLocationDataSource);

  m_nmeaLocationDataSource->start();
  m_mapView->locationDisplay()->start();

  m_nmeaLocationDataSource->pushData(m_mockData);
  recenter();

  return;
}

void DisplayDeviceLocationWithNmeaDataSources::recenter() {
  m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Recenter);

  return;
}

void DisplayDeviceLocationWithNmeaDataSources::reset() {
  return;
}

