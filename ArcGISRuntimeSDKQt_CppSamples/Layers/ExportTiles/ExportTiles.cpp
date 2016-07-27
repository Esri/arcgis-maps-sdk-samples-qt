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

#include "ExportTiles.h"

#include <QUrl>

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

using namespace Esri::ArcGISRuntime;

ExportTiles::ExportTiles(QQuickItem* parent) :
    QQuickItem(parent),
    m_map(nullptr),
    m_mapView(nullptr),
    m_serviceUrl("http://sampleserver6.arcgisonline.com/arcgis/rest/services/World_Street_Map/MapServer"),
    m_exportTileCacheTask(nullptr)
{
}

ExportTiles::~ExportTiles()
{
}

void ExportTiles::componentComplete()
{
    QQuickItem::componentComplete();

    // find QML MapView component
    m_mapView = findChild<MapQuickView*>("mapView");

    // create a tiled basemap
    ArcGISTiledLayer* tiledLayer = new ArcGISTiledLayer(m_serviceUrl);
    Basemap* basemap = new Basemap(tiledLayer, this);

    // create a new map instance
    m_map = new Map(basemap, this);

    // set an initial viewpoint
    Envelope env(12362601, 936021, 10187678, 2567213, SpatialReference(3857));
    Viewpoint viewpoint(env);
    m_map->setInitialViewpoint(viewpoint);

    // set map on the map view
    m_mapView->setMap(m_map);

    // create the task from the tiled layer's map service info once it is loaded
    connect(tiledLayer, &ArcGISTiledLayer::doneLoading, [this, tiledLayer](Error)
    {
      m_exportTileCacheTask = new ExportTileCacheTask(tiledLayer->mapServiceInfo(), this);
      connect(m_exportTileCacheTask, &ExportTileCacheTask::doneLoading, [this](Error error)
      {
        if (!error.isEmpty())
        {
          emit updateStatus("ExportTileCacheTask failed to load.");
          emit hideWindow(5000, false);
        }
      });

      m_exportTileCacheTask->load();
    });
}

void ExportTiles::exportTileCacheFromCorners(double xCorner1, double yCorner1, double xCorner2, double yCorner2, QString dataPath)
{
    // create an envelope from the QML rectangle corners
    auto corner1 = m_mapView->screenToLocation(xCorner1, yCorner1);
    auto corner2 = m_mapView->screenToLocation(xCorner2, yCorner2);
    auto extent = Envelope(corner1, corner2);
    auto tileCacheExtent = GeometryEngine::project(extent, SpatialReference::webMercator());

    // generate parameters
    auto params = m_exportTileCacheTask->createDefaultExportTileCacheParameters(tileCacheExtent, m_mapView->mapScale(), m_exportTileCacheTask->mapServiceInfo().maxScale());

    // execute the task and obtain the job
    auto exportJob = m_exportTileCacheTask->exportTileCache(params, dataPath);

    // check if there is a valid job
    if (exportJob)
    {
        // connect to the job's status changed signal
        connect(exportJob, &ExportTileCacheJob::jobStatusChanged, [this, exportJob]()
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
    else
    {
        emit updateStatus("Export failed");
        emit hideWindow(5000, false);
    }
}

// display the tile cache once the task is complete
void ExportTiles::displayOutputTileCache(TileCache* tileCache)
{
    // create a new tiled layer from the output tile cache
    auto tiledLayer = new ArcGISTiledLayer(tileCache, this);

    // add the new layer to a basemap
    auto basemap = new Basemap(tiledLayer, this);

    // set the new basemap on the map
    m_map->setBasemap(basemap);

    // zoom to the new layer and hide window once loaded
    connect(tiledLayer, &ArcGISTiledLayer::doneLoading, [this, tiledLayer]()
    {
        if (tiledLayer->loadStatus() == LoadStatus::Loaded)
        {
            m_mapView->setViewpointScale(m_mapView->mapScale() * 0.5);
        }
    });
}
