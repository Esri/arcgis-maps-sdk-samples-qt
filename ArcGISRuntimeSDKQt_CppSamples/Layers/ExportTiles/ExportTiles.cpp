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
  m_basemap = new Basemap(BasemapStyle::ArcGISImagery, this);

  // create an export tile cache task when basemap has finished loading
  connect(m_basemap, &Basemap::doneLoading, this, [this]()
  {
    if (m_basemap->baseLayers()->size() > 0)
      createExportTileCacheTask();
  });

  m_basemap->load();

  // create a new map instance
  m_map = new Map(m_basemap, this);

  // set an initial viewpoint
  m_map->setInitialViewpoint(Viewpoint(35, -117, 1e7));
  m_map->setMinScale(1e7);

  // set map on the map view
  m_mapView->setMap(m_map);
}

void ExportTiles::createExportTileCacheTask()
{
  // Get a tile layer from the basemap
  m_tiledLayer = dynamic_cast<ArcGISTiledLayer*>(m_basemap->baseLayers()->at(0));

  // create the task with the url and load it
  m_exportTileCacheTask = new ExportTileCacheTask(m_tiledLayer->url(), this);

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
    //! [ExportTiles start job]
    // execute the task and obtain the job

    m_map->setMinScale(m_mapView->mapScale());
    m_map->setMaxScale(m_mapView->mapScale()/10);

    QList<int> levelsOfDetail = createLevelsOfDetail(m_map->minScale(), m_map->maxScale());

    parameters.setLevelIds(levelsOfDetail);
    ExportTileCacheJob* exportJob = m_exportTileCacheTask->exportTileCache(parameters, m_tempPath.path() + "/offlinemap.tpkx");

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
    //! [ExportTiles start job]
    else
    {
      emit updateStatus("Export failed");
      emit hideWindow(5000, false);
    }
  });

  // generate parameters
  m_exportTileCacheTask->createDefaultExportTileCacheParameters(tileCacheExtent, m_mapView->mapScale(), m_tiledLayer->maxScale());
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

int convertScaleToLevelOfDetail(double scale)
{
  /*
   * This is a helper function to convert map scale to zoom levels used by most mapping APIs
   * For more information about zoom levels and scale visit:
   * https://developers.arcgis.com/documentation/mapping-apis-and-services/reference/zoom-levels-and-scale
   */

  QList<double> scaleList =
  {
    591657527.591555, // Global
    295828763.795777,
    147914381.897889, // Subcontinent
    73957190.948944,
    36978595.474472,
    18489297.737236,  // Large country
    9244648.868618,
    4622324.434309,   // Small country/US state
    2311162.217155,
    1155581.108577,   // Large metropolitan area
    577790.554289,
    288895.277144,    // City
    144447.638572,    // Town
    72223.819286,     // Village
    36111.909643,
    18055.954822,     // Small road
    9027.977411,      // Street
    4513.988705,      // Street block
    2256.994353,      // Address
    1128.497176,      // Street intersection
    564.248588,
    282.124294,
    141.062147,
    70.5310735
  };

  int levelOfDetail = 0;
  while (levelOfDetail < scaleList.size() && scale < scaleList[levelOfDetail])
  {
    ++levelOfDetail;
  }
  return levelOfDetail;
}

QList<int> ExportTiles::createLevelsOfDetail(double minScale, double maxScale)
{
  QList<int> levelsOfDetail;
  int minLevel = ::convertScaleToLevelOfDetail(minScale);
  int maxLevel = ::convertScaleToLevelOfDetail(maxScale);

  for (int i = minLevel; i < maxLevel; ++i)
  {
    levelsOfDetail.push_back(i);
  }

  return levelsOfDetail;
}


