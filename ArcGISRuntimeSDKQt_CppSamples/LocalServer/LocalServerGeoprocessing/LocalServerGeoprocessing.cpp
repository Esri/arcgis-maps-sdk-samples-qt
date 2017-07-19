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

#include "LocalServerGeoprocessing.h"

#include "Basemap.h"
#include "MapQuickView.h"
#include "Map.h"
#include "LocalServer.h"
#include "Viewpoint.h"
#include "TileCache.h"
#include "LocalGeoprocessingService.h"
#include "ArcGISTiledLayer.h"
#include "GeoprocessingTask.h"
#include "GeoprocessingJob.h"
#include "GeoprocessingParameter.h"
#include "GeoprocessingParameters.h"
#include "GeoprocessingDouble.h"
#include "ArcGISMapImageLayer.h"

#include <QDir>

using namespace Esri::ArcGISRuntime;

// constructor
LocalServerGeoprocessing::LocalServerGeoprocessing(QQuickItem* parent) :
  QQuickItem(parent)
{
}

// destructor
LocalServerGeoprocessing::~LocalServerGeoprocessing()
{
}

void LocalServerGeoprocessing::componentComplete()
{
  QQuickItem::componentComplete();

  QString dataPath = QDir::homePath() + "/ArcGIS/Runtime/Data";

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create a map using the topographic BaseMap
  m_map = new Map(Basemap::lightGrayCanvas(this), this);

  // Set map to map view
  m_mapView->setMap(m_map);

  TileCache* tileCache = new TileCache(dataPath + "/tpk/RasterHillshade.tpk", this);
  m_tiledLayer = new ArcGISTiledLayer(tileCache, this);
  m_map->operationalLayers()->append(m_tiledLayer);

  // create a gp service
  m_localGPService = new LocalGeoprocessingService(dataPath + "/gpk/Contour.gpk", this);
  m_localGPService->setServiceType(GeoprocessingServiceType::AsynchronousSubmitWithMapServerResult);

  if (LocalServer::instance()->isInstallValid())
  {
    connectSignals();
    LocalServer::start();
  }
}

void LocalServerGeoprocessing::connectSignals()
{
  connect (m_tiledLayer, &ArcGISTiledLayer::loadStatusChanged, this, [this](LoadStatus status)
  {
    if (status == LoadStatus::Loaded)
    {
      m_mapView->setViewpoint(Viewpoint(m_tiledLayer->fullExtent()));
    }
  });

  // local server status
  connect(LocalServer::instance(), &LocalServer::statusChanged, this, [this]()
  {
    if (LocalServer::status() == LocalServerStatus::Started)
    {
      m_localGPService->start();
    }
  });

  // local gp service status
  connect(m_localGPService, &LocalGeoprocessingService::statusChanged, this, [this]()
  {
    if (m_localGPService->status() == LocalServerStatus::Started)
    {
      m_gpTask = new GeoprocessingTask(QUrl(m_localGPService->url().toString() + "/Contour"));
    }
  });
}

void LocalServerGeoprocessing::setInterval(const double &value)
{
  if (m_interval <= 0.0)
    return;

  m_interval = value;
  emit intervalChanged();
}

void LocalServerGeoprocessing::generateContours()
{
  GeoprocessingParameters gpParams(GeoprocessingExecutionType::AsynchronousSubmit);
  QMap<QString, GeoprocessingParameter*> inputs = gpParams.inputs(this);
  inputs.insert("Interval", new GeoprocessingDouble(m_interval, this));

  GeoprocessingJob* gpJob = m_gpTask->createJob(gpParams);

  connect(gpJob, &GeoprocessingJob::jobDone, this, [this, gpJob]()
  {
    qDebug() << "Job status: " << (int)gpJob->jobStatus();
    if (gpJob->jobStatus() == JobStatus::Succeeded)
    {
      QUrl url = m_localGPService->url();
//      qDebug() << url.toString()
      QUrl mapServerUrl = QUrl(url.toString().replace("GPServer", "MapServer/jobs/" + gpJob->serverJobId()));
      ArcGISMapImageLayer* mapImageLayer = new ArcGISMapImageLayer(mapServerUrl);
      qDebug() << gpJob->serverJobId();
      m_map->operationalLayers()->append(mapImageLayer);
    }
  });

  connect(gpJob, &GeoprocessingJob::errorOccurred, this, [this](Error error)
  {
     qDebug() << "Error: " << error.message();
  });

  gpJob->start();
}

void LocalServerGeoprocessing::clearResults()
{
  if (m_mapView->map()->operationalLayers()->size() > 1)
    m_mapView->map()->operationalLayers()->removeAt(1);
}
