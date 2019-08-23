// [WriteFile Name=GenerateOfflineMap, Category=Maps]
// [Legal]
// Copyright 2017 Esri.

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

#ifndef GENERATEOFFLINEMAP_H
#define GENERATEOFFLINEMAP_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class PortalItem;
class AuthenticationManager;
class OfflineMapTask;
}
}

#include <QQuickItem>
#include <QTemporaryDir>

class GenerateOfflineMap : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::AuthenticationManager* authenticationManager READ authenticationManager CONSTANT)
  Q_PROPERTY(bool mapLoaded MEMBER m_mapLoaded NOTIFY mapLoadedChanged)

public:
  explicit GenerateOfflineMap(QQuickItem* parent = nullptr);
  ~GenerateOfflineMap() override = default;

  void componentComplete() override;
  static void init();

public:
  Q_INVOKABLE void generateMapByExtent(double xCorner1, double yCorner1, double xCorner2, double yCorner2);

signals:
  void mapLoadedChanged();
  void hideWindow(int time, bool success);
  void updateStatus(const QString& status);
  void updateProgress(int progress);
  void showLayerErrors(const QString& error);

private:
  static const QString webMapId() { return s_webMapId; }
  Esri::ArcGISRuntime::AuthenticationManager* authenticationManager() const;

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::PortalItem* m_portalItem = nullptr;
  Esri::ArcGISRuntime::OfflineMapTask* m_offlineMapTask = nullptr;
  static const QString s_webMapId;  
  bool m_mapLoaded = false;
  QTemporaryDir m_tempPath;
};

#endif // GENERATEOFFLINEMAP_H
