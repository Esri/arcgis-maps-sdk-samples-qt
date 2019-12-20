// [WriteFile Name=ExportTiles, Category=Layers]
// [Legal]
// Copyright 2016 Esri.

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

#include "ExportTiles.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "ExportTiles.h"
#include "ArcGISTiledLayer.h"
#include "ExportTileCacheTask.h"
#include "Envelope.h"
#include "GeometryEngine.h"
#include "SpatialReference.h"
#include "TileCache.h"

#include <QUrl>

using namespace Esri::ArcGISRuntime;

ExportTiles::ExportTiles(QQuickItem* parent) :
  QQuickItem(parent)
{
}

ExportTiles::~ExportTiles() = default;

void ExportTiles::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ExportTiles>("Esri.Samples", 1, 0, "ExportTilesSample");
}

void ExportTiles::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // create a tiled basemap
  ArcGISTiledLayer* tiledLayer = new ArcGISTiledLayer(m_serviceUrl, this);
  Basemap* basemap = new Basemap(tiledLayer, this);

  // create a new map instance
  m_map = new Map(basemap, this);

  // set an initial viewpoint
  Envelope env(12362601, 936021, 10187678, 2567213, SpatialReference(3857));
  Viewpoint viewpoint(env);
  m_map->setInitialViewpoint(viewpoint);

  // set map on the map view
  m_mapView->setMap(m_map);

  // create the task with the url and load it
  m_exportTileCacheTask = new ExportTileCacheTask(m_serviceUrl, this);
  connect(m_exportTileCacheTask, &ExportTileCacheTask::doneLoading, this, [this](Error error)
  {
    if (!error.isEmpty())
    {
      emit updateStatus("Export failed");
      emit hideWindow(5000, false);
    }
  });
  m_exportTileCacheTask->load();
}

void ExportTiles::exportTileCacheFromCorners(double xCorner1, double yCorner1, double xCorner2, double yCorner2)
{
  // create an envelope from the QML rectangle corners
  const Point corner1 = m_mapView->screenToLocation(xCorner1, yCorner1);
  const Point corner2 = m_mapView->screenToLocation(xCorner2, yCorner2);
  const Envelope extent(corner1, corner2);
  const Geometry tileCacheExtent = GeometryEngine::project(extent, SpatialReference::webMercator());

  // connect to sync task doneLoading signal
  connect(m_exportTileCacheTask, &ExportTileCacheTask::defaultExportTileCacheParametersCompleted, this, [this](QUuid, ExportTileCacheParameters parameters)
  {
    m_parameters = parameters;

    //! [ExportTiles start job]
    // execute the task and obtain the job
    ExportTileCacheJob* exportJob = m_exportTileCacheTask->exportTileCache(m_parameters, m_tempPath.path() + "/offlinemap.tpk");

    // check if there is a valid job
    if (exportJob)
    {
      // connect to the job's status changed signal
      connect(exportJob, &ExportTileCacheJob::jobStatusChanged, this, [this, exportJob]()
      {
        // connect to the job's status changed signal to know once it is done
        switch (exportJob->jobStatus()) {
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
          emit updateStatus("Adding TPK...");
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
    //! [ExportTiles start job]
    else
    {
      emit updateStatus("Export failed");
      emit hideWindow(5000, false);
    }
  });

  // generate parameters
  m_exportTileCacheTask->createDefaultExportTileCacheParameters(tileCacheExtent, m_mapView->mapScale(), m_exportTileCacheTask->mapServiceInfo().maxScale());
}

// display the tile cache once the task is complete
void ExportTiles::displayOutputTileCache(TileCache* tileCache)
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
      m_mapView->setViewpointScale(m_mapView->mapScale() * 0.5);
    }
  });
}
