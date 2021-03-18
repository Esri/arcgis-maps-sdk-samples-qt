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

#ifndef DISPLAYDEVICELOCATIONWITHNMEADATASOURCES_H
#define DISPLAYDEVICELOCATIONWITHNMEADATASOURCES_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class NmeaLocation;
class NmeaLocationDataSource;
class SimulatedLocationDataSource;
}
}

#include <QObject>
#include <QByteArray>
#include <QFile>
#include <QTimer>

class DisplayDeviceLocationWithNmeaDataSources : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool sampleStarted MEMBER m_sampleStarted NOTIFY sampleStartedChanged)
  Q_PROPERTY(bool useSimulatedData MEMBER m_useSimulatedData NOTIFY dataSourceChanged)

public:
  explicit DisplayDeviceLocationWithNmeaDataSources(QObject* parent = nullptr);
  ~DisplayDeviceLocationWithNmeaDataSources();

  static void init();

  Q_INVOKABLE void start();
  Q_INVOKABLE void reset();
  Q_INVOKABLE void changeDataSource();

signals:
  void mapViewChanged();
  void sampleStartedChanged();
  void dataSourceChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  bool loadMockDataFile(QString filePath);
  void recenter();
  void startNmeaSimulation();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::NmeaLocationDataSource* m_nmeaLocationDataSource = nullptr;
  Esri::ArcGISRuntime::NmeaLocation* m_nmeaData = nullptr;
  Esri::ArcGISRuntime::SimulatedLocationDataSource* m_simulatedLocationDataSource = nullptr;
  QList<QByteArray> m_mockNmeaSentences;
  int m_mockDataIterator;
  QTimer* m_timer = new QTimer(this);
  bool m_sampleStarted = false;
  bool m_useSimulatedData = true;
};

#endif // DISPLAYDEVICELOCATIONWITHNMEADATASOURCES_H
