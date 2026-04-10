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

#ifndef SHOWDEVICELOCATIONUSINGINDOORPOSITIONING_H
#define SHOWDEVICELOCATIONUSINGINDOORPOSITIONING_H

// ArcGIS Maps SDK headers
#include "Location.h"

// Qt headers
#include <QMap>
#include <QObject>

namespace Esri::ArcGISRuntime
{
  class ArcGISFeatureTable;
  class Error;
  class FeatureTable;
  class Map;
  class MapQuickView;
  class IndoorsLocationDataSource;
} // namespace Esri::ArcGISRuntime

Q_MOC_INCLUDE("MapQuickView.h")

class ShowDeviceLocationUsingIndoorPositioning : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QVariantMap locationProperties MEMBER m_locationProperties NOTIFY locationPropertiesChanged)
  Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)

public:
  explicit ShowDeviceLocationUsingIndoorPositioning(QObject* parent = nullptr);
  ~ShowDeviceLocationUsingIndoorPositioning();

  static void init();

  Q_INVOKABLE void stopLocationDisplay();

signals:
  void mapViewChanged();
  void locationPropertiesChanged();
  void isLoadingChanged();
  void locationPermissionDenied();
  void bluetoothPermissionDenied();
  void errorOccurred(const QString& errorMessage);

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  bool isLoading() const;

  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void trySetupIndoorsLocationDataSource();
  void locationChangedHandler(const Esri::ArcGISRuntime::Location& loc);
  void changeFloorDisplay(const QString& floorLevelId);
  void requestLocationPermissionThenSetupILDS();
  void requestBluetoothThenLocationPermissions();
  void validatePermissions();
  void setLoading(bool isLoading);
  void handlePrerequisiteDoneLoading(const Esri::ArcGISRuntime::Error& loadError);
  void completeIndoorsLocationSetup();
  void showGroundFloor();
  void failSetup(const QString& errorMessage);

  bool m_allPermissionsGranted = false;
  bool m_hasReceivedFirstLocation = false;
  bool m_isLoading = true;
  bool m_isSettingUpIndoors = false;
  bool m_setupFailed = false;
  int m_pendingPrerequisiteLoads = 0;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::IndoorsLocationDataSource* m_indoorsLocationDataSource = nullptr;
  QVariantMap m_locationProperties;
};

#endif // SHOWDEVICELOCATIONUSINGINDOORPOSITIONING_H
