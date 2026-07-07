// [WriteFile Name=DownloadRasterTilesToLocalCache, Category=Layers]
// [Legal]
// Copyright 2016 Esri.
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "DownloadRasterTilesToLocalCache.h"

// ArcGIS Maps SDK headers
#include "ArcGISTiledLayer.h"
#include "Basemap.h"
#include "Envelope.h"
#include "Error.h"
#include "ExportTileCacheJob.h"
#include "ExportTileCacheParameters.h"
#include "ExportTileCacheTask.h"
#include "GeometryEngine.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "SpatialReference.h"
#include "TaskTypes.h"
#include "TileCache.h"
#include "Viewpoint.h"

// Qt headers
#include <QFuture>
#include <QUrl>
#include <QUuid>

using namespace Esri::ArcGISRuntime;

static constexpr double s_minScale = 10000000;
static const QUrl s_worldOceanBaseForExportUrl("https://tiledbasemaps.arcgis.com/arcgis/rest/services/Ocean/World_Ocean_Base/MapServer");

DownloadRasterTilesToLocalCache::DownloadRasterTilesToLocalCache(QQuickItem* parent) :
  QQuickItem(parent)
{
}

DownloadRasterTilesToLocalCache::~DownloadRasterTilesToLocalCache() = default;

void DownloadRasterTilesToLocalCache::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DownloadRasterTilesToLocalCache>("Esri.Samples", 1, 0, "DownloadRasterTilesToLocalCacheSample");
}

void DownloadRasterTilesToLocalCache::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // create a tiled basemap from a map service that supports tile export
  ArcGISTiledLayer* tiledLayer = new ArcGISTiledLayer(s_worldOceanBaseForExportUrl, this);
  Basemap* basemap = new Basemap(tiledLayer, this);

  // create an export tile cache task when the tiled layer has finished loading
  connect(tiledLayer, &ArcGISTiledLayer::doneLoading, this, [this]()
  {
    if (!m_map->basemap()->baseLayers()->isEmpty())
    {
      createExportTileCacheTask();
    }
  });

  // create a new map instance
  m_map = new Map(basemap, this);
  m_map->setMinScale(s_minScale);

  // set an initial viewpoint
  m_map->setInitialViewpoint(Viewpoint(35, -117, s_minScale));

  // set map on the map view
  m_mapView->setMap(m_map);
}

void DownloadRasterTilesToLocalCache::createExportTileCacheTask()
{
  // Get a tile layer from the basemap
  ArcGISTiledLayer* tiledLayer = dynamic_cast<ArcGISTiledLayer*>(m_map->basemap()->baseLayers()->at(0));

  // create the task with the url and load it
  m_exportTileCacheTask = new ExportTileCacheTask(tiledLayer->url(), this);

  connect(m_exportTileCacheTask, &ExportTileCacheTask::doneLoading, this, [this](const Error& error)
  {
    if (!error.isEmpty())
    {
      emit updateStatus("Export failed");
      emit hideWindow(5000, false);
    }
  });

  m_exportTileCacheTask->load();
}

void DownloadRasterTilesToLocalCache::exportTileCacheFromCorners(double xCorner1, double yCorner1, double xCorner2, double yCorner2)
{
  // create an envelope from the QML rectangle corners
  const Point corner1 = m_mapView->screenToLocation(xCorner1, yCorner1);
  const Point corner2 = m_mapView->screenToLocation(xCorner2, yCorner2);
  const Envelope extent(corner1, corner2);
  const Geometry tileCacheExtent = GeometryEngine::project(extent, SpatialReference::webMercator());

  // generate parameters
  m_exportTileCacheTask->createDefaultExportTileCacheParametersAsync(tileCacheExtent, m_mapView->mapScale(), m_mapView->mapScale() * 0.1)
    .then(this, [this](const ExportTileCacheParameters& parameters)
  {
    onDefaultExportTileCacheParametersCompleted_(parameters);
  });
}

void DownloadRasterTilesToLocalCache::onDefaultExportTileCacheParametersCompleted_(const ExportTileCacheParameters& parameters)
{
  //! [DownloadRasterTilesToLocalCache start job]
  // execute the task and obtain the job
  ExportTileCacheJob* exportJob = m_exportTileCacheTask->exportTileCache(parameters, m_tempPath.path() + "/offlinemap.tpkx");

  // check if there is a valid job
  if (exportJob)
  {
    connect(exportJob, &ExportTileCacheJob::progressChanged, this, [this, exportJob]()
    {
      m_exportProgress = exportJob->progress();
      emit exportProgressChanged();
    });

    // connect to the job's status changed signal
    connect(exportJob, &ExportTileCacheJob::statusChanged, this, [this, exportJob](JobStatus jobStatus)
    {
      // connect to the job's status changed signal to know once it is done
      switch (jobStatus)
      {
        case JobStatus::Failed:
          emit updateStatus("Export failed");
          emit hideWindow(5000, false);
          break;
        case JobStatus::NotStarted:
          emit updateStatus("Job not started");
          break;
        case JobStatus::Paused:
          emit updateStatus("Job paused");
          break;
        case JobStatus::Started:
          emit updateStatus("In progress...");
          break;
        case JobStatus::Succeeded:
          emit updateStatus("Adding TPKX...");
          emit hideWindow(1500, true);
          displayOutputTileCache(exportJob->result());
          break;
        default:
          break;
      }
    });

    // start the export job
    exportJob->start();
  }
  //! [DownloadRasterTilesToLocalCache start job]
  else
  {
    emit updateStatus("Export failed");
    emit hideWindow(5000, false);
  }
}

// display the tile cache once the task is complete
void DownloadRasterTilesToLocalCache::displayOutputTileCache(TileCache* tileCache)
{
  // create a new tiled layer from the output tile cache
  ArcGISTiledLayer* tiledLayer = new ArcGISTiledLayer(tileCache, this);

  // add the new layer to a basemap
  Basemap* basemap = new Basemap(tiledLayer, this);

  // set the new basemap on the map
  m_map->setBasemap(basemap);

  // zoom to the new layer and hide window once loaded
  connect(tiledLayer, &ArcGISTiledLayer::doneLoading, this, [this, tiledLayer]()
  {
    if (tiledLayer->loadStatus() == LoadStatus::Loaded)
    {
      const double prevMapScale = m_mapView->mapScale();
      m_map->setMinScale(prevMapScale);
      m_map->setMaxScale(prevMapScale * 0.1);
      m_mapView->setViewpointScaleAsync(prevMapScale * 0.5);
    }
  });
}
