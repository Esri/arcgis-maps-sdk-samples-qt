// [WriteFile Name=OfflineRouting, Category=Analysis]
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

#include "OfflineRouting.h"

#include "ArcGISTiledLayer.h"
#include "Map.h"
#include "MapQuickView.h"
#include "TileCache.h"

#include <QDir>

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
  QString defaultDataPath()
  {
    QString dataPath;

  #ifdef Q_OS_ANDROID
    dataPath = "/sdcard";
  #elif defined Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  #else
    dataPath = QDir::homePath();
  #endif

    return dataPath;
  }
} // namespace

OfflineRouting::OfflineRouting(QObject* parent /* = nullptr */):
  QObject(parent)
{
  const QString fileLocation = QString("%1/ArcGIS/Runtime/Data/tpk/san_diego/streetmap_SD.tpk").arg(defaultDataPath());
  TileCache* tileCache = new TileCache(fileLocation, this);
  ArcGISTiledLayer* tiledLayer = new ArcGISTiledLayer(tileCache, this);
  Basemap* basemap = new Basemap(tiledLayer, this);
  m_map = new Map(basemap, this);
  m_map->setMinScale(100000);
}

OfflineRouting::~OfflineRouting() = default;

void OfflineRouting::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<OfflineRouting>("Esri.Samples", 1, 0, "OfflineRoutingSample");
}

MapQuickView* OfflineRouting::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void OfflineRouting::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  const QString geodatabaseLocation = QString("%1/ArcGIS/Runtime/Data/tpk/san_diego/sandiego.geodatabase").arg(defaultDataPath());

  emit mapViewChanged();
}
