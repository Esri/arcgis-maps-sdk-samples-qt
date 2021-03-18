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
#include <QTimer>

using namespace Esri::ArcGISRuntime;

DisplayDeviceLocationWithNmeaDataSources::DisplayDeviceLocationWithNmeaDataSources(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISNavigationNight, this))
{
  m_useSimulatedData = true;
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

  m_mapView->setViewpointCenter(Point());

  m_nmeaLocationDataSource = new NmeaLocationDataSource(SpatialReference::wgs84(), this);
  m_mapView->locationDisplay()->setDataSource(m_nmeaLocationDataSource);

  emit mapViewChanged();
}

void DisplayDeviceLocationWithNmeaDataSources::changeDataSource()
{
  m_useSimulatedData =! m_useSimulatedData;
}

void DisplayDeviceLocationWithNmeaDataSources::start()
{
  if (m_useSimulatedData)
    startNmeaSimulation();
  else
  {
    qDebug() << "Device not found";
    m_sampleStarted = false;
  }

  if (!m_sampleStarted)
    emit sampleStartedChanged();

  else
    m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Recenter);
}

void DisplayDeviceLocationWithNmeaDataSources::startNmeaSimulation()
{
  m_nmeaLocationDataSource->start();
  m_mapView->locationDisplay()->start();


  if (m_mockNmeaSentences.isEmpty())
  {
    QString filePath = ":/Samples/DisplayInformation/DisplayDeviceLocationWithNmeaDataSources/redlands.nmea";
    if(!loadMockDataFile(filePath))
    {
      qDebug() << "Unable to load file at path:" << filePath;
      m_sampleStarted = false;
      return;
    }
  }

  m_mockDataIterator = 0;

  connect(m_timer, &QTimer::timeout, this, [this]()
  {
    while(true)
    {
      QByteArray line = m_mockNmeaSentences.at(m_mockDataIterator);
      m_nmeaLocationDataSource->pushData(line);
      ++m_mockDataIterator;

      if (m_mockDataIterator >= m_mockNmeaSentences.size()-1)
        m_mockDataIterator = 0;

      if (line.startsWith("$GPGGA")) {
        break;
      }
    }
  });

  m_timer->start(1000);
}

void DisplayDeviceLocationWithNmeaDataSources::reset() {
  m_timer->stop();


  disconnect(m_timer, &QTimer::timeout, nullptr, nullptr);


  m_mapView->locationDisplay()->stop();
  m_nmeaLocationDataSource->stop();
  m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Off);
}

bool DisplayDeviceLocationWithNmeaDataSources::loadMockDataFile(QString filePath)
{
  QFile mockDataFile;
  mockDataFile.setFileName(filePath);

  if(!mockDataFile.exists())
    return false;

  mockDataFile.open(QIODevice::ReadOnly);

  while (!mockDataFile.atEnd())
    m_mockNmeaSentences.append(mockDataFile.readLine());

  mockDataFile.close();

  return true;
}

