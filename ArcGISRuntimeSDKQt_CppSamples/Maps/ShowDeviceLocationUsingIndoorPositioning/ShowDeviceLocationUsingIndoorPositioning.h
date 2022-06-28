// [WriteFile Name=ShowDeviceLocationUsingIndoorPositioning, Category=Maps]
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

#ifndef SHOWDEVICELOCATIONUSINGINDOORPOSITIONING_H
#define SHOWDEVICELOCATIONUSINGINDOORPOSITIONING_H

namespace Esri
{
namespace ArcGISRuntime
{
class ArcGISFeatureTable;
class FeatureTable;
class Location;
class Map;
class MapQuickView;
}
}

#include <QObject>

class ShowDeviceLocationUsingIndoorPositioning : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  explicit ShowDeviceLocationUsingIndoorPositioning(QObject* parent = nullptr);
  ~ShowDeviceLocationUsingIndoorPositioning();

  static void init();

signals:
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void findPositioningTable();
  void findPathwaysTable();
  void findGlobalID();
  void setupIndoorsLocationDataSource(QUuid globalID);
  void locationChanged(Esri::ArcGISRuntime::Location loc);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureTable* m_positioningTable = nullptr;
  Esri::ArcGISRuntime::ArcGISFeatureTable* m_pathwaysTable = nullptr;
};

#endif // SHOWDEVICELOCATIONUSINGINDOORPOSITIONING_H
