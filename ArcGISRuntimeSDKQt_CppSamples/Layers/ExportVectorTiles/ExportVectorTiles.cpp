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
#include "GraphicsOverlay.h"
#include "Map.h"
#include "MapQuickView.h"
#include "Symbol.h"
#include "SimpleLineSymbol.h"

#include <QTemporaryDir>

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
  m_mapView->setViewpoint(Viewpoint(34.049, -117.181, 1e4));

  Graphic* downloadArea = new Graphic(this);
  downloadArea->setSymbol(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::red, 2, this));
  downloadArea->setGeometry(Viewpoint(34.049, -117.181, 1e4).targetGeometry());

  GraphicsOverlay* graphicsOverlay = new GraphicsOverlay(this);
  graphicsOverlay->graphics()->append(downloadArea);

  m_mapView->graphicsOverlays()->append(graphicsOverlay);

  emit mapViewChanged();
}

void ExportVectorTiles::startExport()
{
  if (m_map->basemap()->baseLayers()->first()->layerType() != LayerType::ArcGISVectorTiledLayer)
    return;

  ArcGISVectorTiledLayer* vectorTiledLayer = static_cast<ArcGISVectorTiledLayer*>(m_map->basemap()->baseLayers()->first());
  ExportVectorTilesTask* exportTask = new ExportVectorTilesTask(vectorTiledLayer->sourceInfo().url(), this);
  ExportVectorTilesParameters exportParameters(m_mapView->graphicsOverlays()->first()->graphics()->first()->geometry());
  exportParameters.setMaxLevel(m_mapView->mapScale());
  exportParameters.setEsriVectorTilesDownloadOption(EsriVectorTilesDownloadOption::UseReducedFontsService);
  QTemporaryDir dir;
  QString vectorTileCachePath = dir.path();
  QString itemResourcePath;
  exportTask->exportVectorTiles(exportParameters, vectorTileCachePath, itemResourcePath);

}
