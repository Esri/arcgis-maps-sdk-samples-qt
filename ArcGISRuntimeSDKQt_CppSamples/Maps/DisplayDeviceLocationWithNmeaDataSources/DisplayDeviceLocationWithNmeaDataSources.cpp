// [WriteFile Name=DisplayDeviceLocationWithNmeaDataSources, Category=Maps]
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

#include "LocationDisplay.h"
#include "NmeaLocationDataSource.h"

#include "Map.h"
#include "MapQuickView.h"

#include <QFile>
#include <QTimer>

using namespace Esri::ArcGISRuntime;

DisplayDeviceLocationWithNmeaDataSources::DisplayDeviceLocationWithNmeaDataSources(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISNavigationNight, this))
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

  m_mapView->setViewpointCenter(Point(-117.191, 34.0306, SpatialReference::wgs84()), 100'000);

  // Create a new NMEA location data source.
  m_nmeaLocationDataSource = new NmeaLocationDataSource(SpatialReference::wgs84(), this);
  m_mapView->locationDisplay()->setDataSource(m_nmeaLocationDataSource);

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
    QString filePath = ":/Samples/Maps/DisplayDeviceLocationWithNmeaDataSources/redlands.nmea";
    if(!loadMockDataFile(filePath))
    {
      qDebug() << "Unable to load file at path:" << filePath;
      m_sampleStarted = false;
      return;
    }
  }

  // Simulate pushing data to the NMEA location data source
  connect(m_timer, &QTimer::timeout, this, [this]()
  {
    QByteArray line = m_mockNmeaSentences.at(m_mockDataIterator);
    m_nmeaLocationDataSource->pushData(line);

    ++m_mockDataIterator;
    if (m_mockDataIterator >= m_mockNmeaSentences.size())
      m_mockDataIterator = 0;
  });

  m_mockDataIterator = 0;
  m_timer->start(1000);
  m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Recenter);
}

void DisplayDeviceLocationWithNmeaDataSources::reset() {
  // Disable simulated data
  m_timer->stop();
  disconnect(m_timer, &QTimer::timeout, nullptr, nullptr);

  // Stop displaying user's location
  m_mapView->locationDisplay()->stop();
  // Stop receiving location data
  m_nmeaLocationDataSource->stop();
  m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Off);
}

bool DisplayDeviceLocationWithNmeaDataSources::loadMockDataFile(QString filePath)
{
  // Load simulated NMEA sentences to display for sample
  QFile mockDataFile;
  mockDataFile.setFileName(filePath);

  if(!mockDataFile.exists())
    return false;

  mockDataFile.open(QIODevice::ReadOnly);

  m_mockNmeaSentences.append(mockDataFile.readLine());
  while (!mockDataFile.atEnd()) {
    QByteArray line = mockDataFile.readLine();
    if (line.startsWith("$GPGGA"))
      m_mockNmeaSentences.append(line);
    else
      m_mockNmeaSentences.last() += line;
  }

  mockDataFile.close();
  return true;
}

