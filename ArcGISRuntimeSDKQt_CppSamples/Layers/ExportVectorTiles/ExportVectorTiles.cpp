// [WriteFile Name=ExportVectorTiles, Category=Layers]
// [Legal]
// Copyright 2022 Esri.

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

#include "ExportVectorTiles.h"

#include "ArcGISVectorTiledLayer.h"
#include "ExportVectorTilesTask.h"
#include "ExportVectorTilesParameters.h"
#include "GeometryEngine.h"
#include "GraphicsOverlay.h"
#include "Map.h"
#include "MapQuickView.h"
#include "SimpleLineSymbol.h"
#include "MapTypes.h"
#include "GraphicsOverlayListModel.h"
#include "GraphicListModel.h"
#include "SymbolTypes.h"
#include "LayerListModel.h"
#include "ServiceTypes.h"
#include "ExportVectorTilesJob.h"
#include "Error.h"
#include "ExportVectorTilesResult.h"
#include "Viewpoint.h"
#include "Graphic.h"
#include "Envelope.h"
#include "Point.h"
#include "Basemap.h"
#include "SpatialReference.h"

#include <QFuture>
#include <QTemporaryDir>
#include <QUuid>

using namespace Esri::ArcGISRuntime;

ExportVectorTiles::ExportVectorTiles(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISStreetsNight, this))
{
}

ExportVectorTiles::~ExportVectorTiles() = default;

void ExportVectorTiles::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ExportVectorTiles>("Esri.Samples", 1, 0, "ExportVectorTilesSample");
}

MapQuickView* ExportVectorTiles::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ExportVectorTiles::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);
  m_mapView->setViewpointAsync(Viewpoint(34.049, -117.181, 1e4));

  m_graphicsOverlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  // Create the graphic that will be used to show the export extent
  m_exportAreaGraphic = new Graphic(this);
  m_exportAreaGraphic->setSymbol(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::green, 3, this));

  m_graphicsOverlay->graphics()->append(m_exportAreaGraphic);

  emit mapViewChanged();
}

void ExportVectorTiles::startExport(double xSW, double ySW, double xNE, double yNE)
{
  if (!m_map->basemap() || m_map->basemap()->baseLayers()->isEmpty() || m_map->basemap()->baseLayers()->first()->layerType() != LayerType::ArcGISVectorTiledLayer)
    return;

  // Get the first layer of the basemap baselayers as a vector tiled layer for export
  ArcGISVectorTiledLayer* vectorTiledLayer = static_cast<ArcGISVectorTiledLayer*>(m_map->basemap()->baseLayers()->first());
  ExportVectorTilesTask* exportTask = new ExportVectorTilesTask(vectorTiledLayer->url(), this);

  // Create an envelope from the QML rectangle corners
  const Point corner1 = m_mapView->screenToLocation(xSW, ySW);
  const Point corner2 = m_mapView->screenToLocation(xNE, yNE);
  const Envelope extent = Envelope(corner1, corner2);
  // Normalize the central meridian to export tiles if the export area crosses the antemeridian
  const Geometry exportArea = GeometryEngine::normalizeCentralMeridian(GeometryEngine::project(extent, vectorTiledLayer->spatialReference()));

  m_exportAreaGraphic->setGeometry(exportArea);

  // Instantiate export parameters to create the export job with
  exportTask->createDefaultExportVectorTilesParametersAsync(exportArea, m_mapView->mapScale() * 0.1)
      .then(this, [this, exportTask](ExportVectorTilesParameters exportParameters)
  {
    // Using the reduced fonts service will reduce the download size of a vtpk by around 80 Mb
    // It is useful for taking the basemap offline but not recommended if you plan to later upload the vtpk
    exportParameters.setEsriVectorTilesDownloadOption(EsriVectorTilesDownloadOption::UseReducedFontsService);

    // Create a path to store the vector tile package, the file cannot already exist
    const QString vectorTileCachePath = m_tempDir.path() + QString("/vectorTiles%1.vtpk").arg(QDateTime::currentMSecsSinceEpoch());
    // Create a path to an empty directory to store the styling resources (in this case, the night mode version of the layer)
    const QString itemResourcePath = m_tempDir.path() + QString("/itemResources%1").arg(QDateTime::currentMSecsSinceEpoch());

    // Create a job that will download the vector tiles to the given paths
    m_exportJob = exportTask->exportVectorTiles(exportParameters, vectorTileCachePath, itemResourcePath);

    connect(m_exportJob, &ExportVectorTilesJob::jobDone, this, [this]()
    {
      if (m_exportJob->error().isEmpty())
      {
        VectorTileCache* vectorTileCache = m_exportJob->result()->vectorTileCache();
        ItemResourceCache* itemResourceCache = m_exportJob->result()->itemResourceCache();

        // Create a vector tiled layer when the download is completed
        ArcGISVectorTiledLayer* exportedLayer = new ArcGISVectorTiledLayer(vectorTileCache, itemResourceCache, this);
        m_map->setBasemap(new Basemap(exportedLayer, this));
        m_isUsingOfflineBasemap = true;
        m_exportJob->disconnect();
      }
    });

    connect(m_exportJob, &Job::progressChanged, this, [this]()
    {
      m_exportProgress = m_exportJob->progress();
      emit exportProgressChanged();
    });

    connect(m_exportJob, &Job::statusChanged, this, [this](JobStatus s)
    {
      m_jobStatus = (int)s;
      emit jobStatusChanged();
    });
    m_exportJob->start();
  });
}

void ExportVectorTiles::cancel()
{
  m_exportJob->cancelJobAsync().then(this, [this](bool succeeded)
  {
    emit jobCancelDone(succeeded);;
  });
  reset();
}

void ExportVectorTiles::reset()
{
  if (m_isUsingOfflineBasemap)
  {
    m_map->setBasemap(new Basemap(BasemapStyle::ArcGISStreetsNight, this));
    m_isUsingOfflineBasemap = false;
  }

  m_exportAreaGraphic->setGeometry(Geometry());
  m_jobStatus = 0; // Override the job status to set the UI button to "Export area" again
  emit jobStatusChanged();
}

int ExportVectorTiles::exportProgress() const
{
  return m_exportProgress;
}

int ExportVectorTiles::jobStatus() const
{
  return m_jobStatus;
}
