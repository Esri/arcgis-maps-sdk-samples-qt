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

namespace Esri::ArcGISRuntime
{
class ArcGISFeatureTable;
class FeatureTable;
class Map;
class MapQuickView;
}

#include "Location.h"

#include <QObject>
#include <QMap>

Q_MOC_INCLUDE("MapQuickView.h")

class ShowDeviceLocationUsingIndoorPositioning : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QVariantMap locationProperties READ locationProperties NOTIFY locationPropertiesChanged)

public:
  explicit ShowDeviceLocationUsingIndoorPositioning(QObject* parent = nullptr);
  ~ShowDeviceLocationUsingIndoorPositioning();

  static void init();

<<<<<<< HEAD
  Q_INVOKABLE void stopLocationDisplay();

=======
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd
signals:
  void mapViewChanged();
  void locationPropertiesChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  QVariantMap locationProperties() const;

  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void setupIndoorsLocationDataSource();
  void locationChangedHandler(const Esri::ArcGISRuntime::Location& loc);
  void changeFloorDisplay();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureTable* m_positioningTable = nullptr;
  Esri::ArcGISRuntime::ArcGISFeatureTable* m_pathwaysTable = nullptr;
  QVariantMap m_locationProperties;
  int m_currentFloor;
};

#endif // SHOWDEVICELOCATIONUSINGINDOORPOSITIONING_H
