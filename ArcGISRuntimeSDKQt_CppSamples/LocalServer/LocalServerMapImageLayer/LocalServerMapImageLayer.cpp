// [WriteFile Name=LocalServerMapImageLayer, Category=LocalServer]
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "LocalServerMapImageLayer.h"

#include "Basemap.h"
#include "MapQuickView.h"
#include "Map.h"
#include "LocalServer.h"
#include "LocalMapService.h"
#include "ArcGISMapImageLayer.h"
#include "Viewpoint.h"

#include <QDir>

using namespace Esri::ArcGISRuntime;

LocalServerMapImageLayer::LocalServerMapImageLayer(QQuickItem* parent) :
  QQuickItem(parent)
{
}

LocalServerMapImageLayer::~LocalServerMapImageLayer() = default;

void LocalServerMapImageLayer::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<LocalServerMapImageLayer>("Esri.Samples", 1, 0, "LocalServerMapImageLayerSample");
}

void LocalServerMapImageLayer::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create a map using the topographic BaseMap
  m_map = new Map(Basemap::topographic(this), this);

  // Set map to map view
  m_mapView->setMap(m_map);

  QString dataPath = QDir::homePath() + "/ArcGIS/Runtime/Data";

  // create a map service
  m_localMapService = new LocalMapService(dataPath + "/mpk/RelationshipID.mpk", this);

  if (LocalServer::instance()->isInstallValid())
  {
    connectSignals();
    if (LocalServer::status() == LocalServerStatus::Started)
      m_localMapService->start();
    else
      LocalServer::start();
  }
}

void LocalServerMapImageLayer::connectSignals()
{
  // local server status
  connect(LocalServer::instance(), &LocalServer::statusChanged, this, [this]()
  {
    if (LocalServer::status() == LocalServerStatus::Started)
    {
      // start the service
      m_localMapService->start();
    }
  });

  // local map service status
  connect(m_localMapService, &LocalMapService::statusChanged, this, [this]()
  {
    if (m_localMapService->status() == LocalServerStatus::Started)
    {
      ArcGISMapImageLayer* mapImageLayer = new ArcGISMapImageLayer(QUrl(m_localMapService->url()), this);

      connect(mapImageLayer, &ArcGISMapImageLayer::loadStatusChanged, this, [this, mapImageLayer](LoadStatus status)
      {
        if (status == LoadStatus::Loaded)
        {
          m_mapView->setViewpoint(Viewpoint(mapImageLayer->fullExtent()));
        }
      });
      m_map->operationalLayers()->append(mapImageLayer);
    }
  });
}
