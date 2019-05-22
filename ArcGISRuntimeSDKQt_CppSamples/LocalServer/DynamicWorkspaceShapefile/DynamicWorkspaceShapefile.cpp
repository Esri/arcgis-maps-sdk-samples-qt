// [WriteFile Name=DynamicWorkspaceShapefile, Category=LocalServer]
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

#include "DynamicWorkspaceShapefile.h"

#include "Map.h"
#include "MapQuickView.h"
#include "LocalServer.h"
#include "DynamicWorkspace.h"
#include "LocalMapService.h"
#include "ShapefileWorkspace.h"
#include "TableSublayerSource.h"
#include "ArcGISMapImageSublayer.h"
#include "ArcGISMapImageLayer.h"
#include "SimpleRenderer.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

using namespace Esri::ArcGISRuntime;

DynamicWorkspaceShapefile::DynamicWorkspaceShapefile(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_dataPath(QDir::homePath() + "/ArcGIS/Runtime/Data/shapefile")
{
  emit dataPathChanged();
}

void DynamicWorkspaceShapefile::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DynamicWorkspaceShapefile>("Esri.Samples", 1, 0, "DynamicWorkspaceShapefileSample");
}

void DynamicWorkspaceShapefile::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the topographic basemap
  m_map = new Map(Basemap::topographic(this), this);

  // Set map to map view
  m_mapView->setMap(m_map);

  // Start the Local Server
  if (LocalServer::instance()->isInstallValid())
  {
    if (LocalServer::instance()->status() == LocalServerStatus::Started)
    {
      emit localServerInitializationComplete(true);
      startLocalService(m_dataPath + "/mjrroads.shp");
    }
    else
    {
      connect(LocalServer::instance(), &LocalServer::statusChanged, this, [this]()
      {
        if (LocalServer::instance()->status() == LocalServerStatus::Failed)
          emit localServerInitializationComplete(false);
        else if (LocalServer::instance()->status() == LocalServerStatus::Started)
        {
          emit localServerInitializationComplete(true);
          startLocalService(m_dataPath + "/mjrroads.shp");

          // create temp path
          const QString tempPath = QDir::homePath() + "/EsriQtTemp";

          // create the directory
          if (!QDir(tempPath).exists())
            QDir().mkdir(tempPath);

          // set the temp data path for the local server
          LocalServer::instance()->setTempDataPath(tempPath);
        }
      });
      LocalServer::start();
    }
  }
}

// Start a service based on a file and folder path
void DynamicWorkspaceShapefile::startLocalService(const QString& filePath)
{
  if (!LocalServer::instance()->isInstallValid())
    return;

  // Setup file and folder variables
  QFile f(filePath);
  QFileInfo fileInfo(f.fileName());
  QString workspacePath = m_dataPath;

  // create a service from the blank MPK
  QString mapPackagePath = QDir::homePath() + "/ArcGIS/Runtime/Data/mpk/mpk_blank.mpk";
  if (m_localMapService)
    m_localMapService->deleteLater();

  m_localMapService = new LocalMapService(mapPackagePath, this);

  // Create a shapefile workspace
  ShapefileWorkspace* shapefileWorkspace = new ShapefileWorkspace("shp_wkspc", workspacePath, m_localMapService);
  TableSublayerSource* source = new TableSublayerSource(shapefileWorkspace->id(), fileInfo.fileName(), m_localMapService);

  // Add the dynamic workspace to the local map service
  // This must be added before the service is started
  QList<DynamicWorkspace*> dynamicWorkspaces{shapefileWorkspace};
  m_localMapService->setDynamicWorkspaces(dynamicWorkspaces);

  // Connect to the status change signal of the local map service
  connect(m_localMapService, &LocalMapService::statusChanged, this, [this, source]()
  {
    if (m_localMapService->status() == LocalServerStatus::Started)
    {
      // Create the sublayer
      ArcGISMapImageSublayer* imageSublayer = new ArcGISMapImageSublayer(0, source, m_localMapService);

      // create the image layer
      ArcGISMapImageLayer* imageLayer = new ArcGISMapImageLayer(m_localMapService->url(), m_localMapService);

      // Add the sub layer to the image layer
      imageLayer->mapImageSublayers()->append(imageSublayer);

      // Apply default symbology and zoom to the layer's extent
      connect(imageSublayer, &ArcGISMapImageSublayer::doneLoading, this, [this, imageSublayer](Error e)
      {
        if (!e.isEmpty())
          return;

        // apply symbology depending on geometry
        Symbol* symbol = nullptr;
        GeometryType geomType = imageSublayer->mapServiceSublayerInfo().geometryType();

        if (geomType == GeometryType::Point || geomType == GeometryType::Multipoint)
          symbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("red"), 10.0, imageSublayer);
        else if (geomType == GeometryType::Polygon)
          symbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("red"), imageSublayer);
        else
          symbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("red"), 3.0, imageSublayer);

        SimpleRenderer* simpleRenderer = new SimpleRenderer(symbol, imageSublayer);
        imageSublayer->setRenderer(simpleRenderer);

        // zoom to extent
        Envelope fullExtent = imageSublayer->mapServiceSublayerInfo().extent();
        m_mapView->setViewpointGeometry(fullExtent, 25);
      });

      // Add the layer to the map
      m_map->operationalLayers()->clear();
      m_map->operationalLayers()->append(imageLayer);
      imageSublayer->load();
    }
  });

  // start the local service
  m_localMapService->start();
}
