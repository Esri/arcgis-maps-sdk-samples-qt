// [WriteFile Name=DisplayDeviceLocationWithNmeaDataSources, Category=Maps]
// [Legal]
// Copyright 2021 Esri.

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

#include "Map.h"
#include "MapQuickView.h"
#include "NmeaLocationDataSource.h"
#include "MapTypes.h"
#include "MapViewTypes.h"
#include "LocationDisplay.h"
#include "SpatialReference.h"
#include "Point.h"

#include <QFile>
#include <QFuture>
#include <QTimer>

using namespace Esri::ArcGISRuntime;

DisplayDeviceLocationWithNmeaDataSources::DisplayDeviceLocationWithNmeaDataSources(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISNavigation, this)),
  m_timer(new QTimer(this))
{
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

  m_mapView->setViewpointCenterAsync(Point(-117.191, 34.0306, SpatialReference::wgs84()), 100'000);

  // Create a new NMEA location data source
  m_nmeaLocationDataSource = new NmeaLocationDataSource(SpatialReference::wgs84(), this);
  m_mapView->locationDisplay()->setDataSource(m_nmeaLocationDataSource);
  m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Recenter);

  emit mapViewChanged();
}

void DisplayDeviceLocationWithNmeaDataSources::start()
{
  // Enable receiving NMEA location data from external device
  m_nmeaLocationDataSource->start();

  // Display the user's location
  m_mapView->locationDisplay()->start();

  // Load simulated NMEA sentences for sample
  if (m_mockNmeaSentences.isEmpty())
  {
    const QString filePath = ":/Samples/Maps/DisplayDeviceLocationWithNmeaDataSources/redlands.nmea";
    if(!loadMockDataFile(filePath))
    {
      qDebug() << "Unable to load file at path:" << filePath;
      m_nmeaSimulationActive = false;
      emit nmeaSimulationActiveChanged();

      return;
    }
  }

  // Simulate pushing data to the NMEA location data source
  connect(m_timer, &QTimer::timeout, this, [this]()
  {
    const QByteArray line = m_mockNmeaSentences.at(m_mockDataIterator);

    // In a non-simulated scenario, incoming NMEA sentences from a serial port or bluetooth device would be pushed to the location data source in real time
    // NMEA sentences can be pushed individually or in multiple lines separated by line breaks
    // Sentences pass information such as direction, velocity, and location and are grouped together to provide detailed information about a user's position
    m_nmeaLocationDataSource->pushData(line);

    ++m_mockDataIterator;
    if (m_mockDataIterator >= m_mockNmeaSentences.size())
      m_mockDataIterator = 0;
  });

  m_mockDataIterator = 0;
  m_timer->start(1000);
}

void DisplayDeviceLocationWithNmeaDataSources::reset()
{
  // Disable simulated data
  m_timer->stop();
  disconnect(m_timer, &QTimer::timeout, nullptr, nullptr);

  // Stop displaying user's location
  m_mapView->locationDisplay()->stop();

  // Stop receiving location data
  m_nmeaLocationDataSource->stop();
}

bool DisplayDeviceLocationWithNmeaDataSources::loadMockDataFile(const QString& filePath)
{

  // Load simulated NMEA sentences to display for sample
  QFile mockDataFile(filePath);

  if(!mockDataFile.exists())
    return false;

  mockDataFile.open(QIODevice::ReadOnly);

  while (!mockDataFile.atEnd()) {
    QByteArray line = mockDataFile.readLine();

    // In this simulated data stream, blocks of NMEA sentences start with $GPGGA (which provides the device's position)
    if (line.startsWith("$GPGGA"))
      m_mockNmeaSentences.append(line);

    // Additional sentences that provide information such as direction and velocity follow and are separated by line breaks
    else
      m_mockNmeaSentences.last() += line;
  }

  mockDataFile.close();
  return true;
}

