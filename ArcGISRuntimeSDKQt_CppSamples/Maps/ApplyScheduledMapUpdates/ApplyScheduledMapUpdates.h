// [WriteFile Name=ApplyScheduledMapUpdates, Category=Maps]
// [Legal]
// Copyright 2019 Esri.

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

#ifndef APPLYSCHEDULEDMAPUPDATES_H
#define APPLYSCHEDULEDMAPUPDATES_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class MobileMapPackage;
class OfflineMapSyncTask;
class OfflineMapSyncJob;
}
}

#include "Error.h"
#include <QObject>
#include <QTemporaryDir>

class ApplyScheduledMapUpdates : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  explicit ApplyScheduledMapUpdates(QObject* parent = nullptr);
  ~ApplyScheduledMapUpdates();

  static void init();
  Q_INVOKABLE void updateMap();

signals:
  void mapViewChanged();
  void updateUi(bool enabled, const QString& text, const QString& detailedText);

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void connectSyncSignals();
  void setMapToMapView();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::MobileMapPackage* m_mobileMapPackage = nullptr;
  Esri::ArcGISRuntime::OfflineMapSyncTask* m_offlineSyncTask = nullptr;
  Esri::ArcGISRuntime::OfflineMapSyncJob* m_syncJob = nullptr;
  QMetaObject::Connection m_syncJobConnection;
  QTemporaryDir m_TempDir;

private slots:
  void onMmpkDoneLoading(Esri::ArcGISRuntime::Error e);
};

#endif // APPLYSCHEDULEDMAPUPDATES_H
