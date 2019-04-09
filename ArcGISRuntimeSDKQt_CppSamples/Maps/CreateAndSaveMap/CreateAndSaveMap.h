// [WriteFile Name=CreateAndSaveMap, Category=Maps]
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

#ifndef CREATEANDSAVEMAP_H
#define CREATEANDSAVEMAP_H

namespace Esri
{
namespace ArcGISRuntime
{
class AuthenticationManager;
class Map;
class MapQuickView;
class Portal;
}
}

#include <QQuickItem>

class CreateAndSaveMap : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::AuthenticationManager* authenticationManager READ authenticationManager CONSTANT)

public:
  explicit CreateAndSaveMap(QQuickItem* parent = nullptr);
  ~CreateAndSaveMap() override = default;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void createMap(const QString& basemap, const QStringList& operationalLayers);
  Q_INVOKABLE void loadPortal();
  Q_INVOKABLE void saveMap(const QString& title, const QString& tags, const QString& description);

signals:
  void portalLoaded();
  void saveMapCompleted(bool success, const QString& itemId = "", const QString& error = "");

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Portal* m_portal = nullptr;
  Esri::ArcGISRuntime::AuthenticationManager* authenticationManager() const;
};

#endif // CREATEANDSAVEMAP_H
