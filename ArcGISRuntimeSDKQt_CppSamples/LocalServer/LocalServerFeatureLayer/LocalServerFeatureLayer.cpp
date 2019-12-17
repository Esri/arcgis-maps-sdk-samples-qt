// [WriteFile Name=LocalServerFeatureLayer, Category=LocalServer]
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

#include "LocalServerFeatureLayer.h"

#include "Basemap.h"
#include "MapQuickView.h"
#include "Map.h"
#include "LocalServer.h"
#include "LocalFeatureService.h"
#include "ServiceFeatureTable.h"
#include "FeatureLayer.h"
#include "Viewpoint.h"

#include <QDir>

using namespace Esri::ArcGISRuntime;

LocalServerFeatureLayer::LocalServerFeatureLayer(QQuickItem* parent) :
  QQuickItem(parent)
{
}

LocalServerFeatureLayer::~LocalServerFeatureLayer() = default;

void LocalServerFeatureLayer::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<LocalServerFeatureLayer>("Esri.Samples", 1, 0, "LocalServerFeatureLayerSample");
}

void LocalServerFeatureLayer::componentComplete()
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

  // create a feature service
  m_localFeatureService = new LocalFeatureService(dataPath + "/mpk/PointsofInterest.mpk", this);

  if (LocalServer::instance()->isInstallValid())
  {
    connectSignals();
    if (LocalServer::status() == LocalServerStatus::Started)
      m_localFeatureService->start();
    else
      LocalServer::start();
  }
}

void LocalServerFeatureLayer::connectSignals()
{
  // local server status
  connect(LocalServer::instance(), &LocalServer::statusChanged, this, [this]()
  {
    if (LocalServer::status() == LocalServerStatus::Started)
    {
      qDebug() << "Local server started";
    }
  });

  // local feature service status
  connect(m_localFeatureService, &LocalFeatureService::statusChanged, this, [this]()
  {
    if (m_localFeatureService->status() == LocalServerStatus::Started)
    {
      // create the feature layer
      ServiceFeatureTable* svt = new ServiceFeatureTable(QUrl(m_localFeatureService->url().toString() + "/0"), this);
      FeatureLayer* featureLayer = new FeatureLayer(svt, this);
      connect(featureLayer, &FeatureLayer::loadStatusChanged, this, [this, featureLayer](LoadStatus status)
      {
        if (status == LoadStatus::Loaded)
        {
          m_mapView->setViewpoint(Viewpoint(featureLayer->fullExtent()));
        }
      });
      m_map->operationalLayers()->append(featureLayer);
    }
  });
}
