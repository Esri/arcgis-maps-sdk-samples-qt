// [WriteFile Name=GenerateOfflineMap_Overrides, Category=Maps]
// [Legal]
// Copyright 2018 Esri.

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

#ifndef GENERATEOFFLINEMAP_OVERRIDES_H
#define GENERATEOFFLINEMAP_OVERRIDES_H

// C++ API headers
#include "AuthenticationManager.h"
#include "GenerateOfflineMapParameters.h"

namespace Esri
{
namespace ArcGISRuntime
{
class FeatureLayer;
class GenerateOfflineMapParameterOverrides;
class Map;
class MapQuickView;
class OfflineMapTask;
class PortalItem;
}
}

#include <QQuickItem>
#include <QTemporaryDir>

class GenerateOfflineMap_Overrides: public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::AuthenticationManager* authenticationManager READ authenticationManager CONSTANT)
  Q_PROPERTY(bool mapLoaded READ mapLoaded NOTIFY mapLoadedChanged)
  Q_PROPERTY(bool overridesReady READ overridesReady NOTIFY overridesReadyChanged)
  Q_PROPERTY(bool taskBusy READ taskBusy NOTIFY taskBusyChanged)

public:
  explicit GenerateOfflineMap_Overrides(QQuickItem* parent = nullptr);
  ~GenerateOfflineMap_Overrides() = default;

  void componentComplete() override;
  static void init();

public:
  Q_INVOKABLE void setAreaOfInterest(double xCorner1, double yCorner1, double xCorner2, double yCorner2);
  Q_INVOKABLE void setBasemapLOD(int min, int max);
  Q_INVOKABLE void setBasemapBuffer(int bufferMetres);
  Q_INVOKABLE void removeSystemValves();
  Q_INVOKABLE void removeServiceConnection();
  Q_INVOKABLE void setHydrantWhereClause(const QString& whereClause);
  Q_INVOKABLE void setClipWaterPipesAOI(bool clip);
  Q_INVOKABLE void takeMapOffline();

signals:
  void taskBusyChanged();
  void mapLoadedChanged();
  void hideWindow(int time, bool success);
  void updateStatus(const QString& status);
  void updateProgress(int progress);
  void showLayerErrors(const QString& error);
  void overridesReadyChanged();

private:
  static QString webMapId();
  bool taskBusy() const;
  Esri::ArcGISRuntime::AuthenticationManager* authenticationManager() const;
  bool mapLoaded() const;
  bool overridesReady() const;
  void removeFeatureLayer(const QString& layerName);
  Esri::ArcGISRuntime::FeatureLayer* getFeatureLayerByName(const QString& layerName);
  void setBusy(bool busy);

private:
  bool m_taskBusy = false;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::PortalItem* m_portalItem = nullptr;
  Esri::ArcGISRuntime::OfflineMapTask* m_offlineMapTask = nullptr;
  Esri::ArcGISRuntime::GenerateOfflineMapParameters m_parameters;
  Esri::ArcGISRuntime::GenerateOfflineMapParameterOverrides* m_parameterOverrides = nullptr;
  static const QString s_webMapId;
  bool m_mapLoaded = false;
  QTemporaryDir m_tempPath;
};

#endif // GENERATEOFFLINEMAP_OVERRIDES_H
