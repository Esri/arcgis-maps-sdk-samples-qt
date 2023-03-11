// COPYRIGHT 2023 ESRI
// TRADE SECRETS: ESRI PROPRIETARY AND CONFIDENTIAL
// Unpublished material - all rights reserved under the
// Copyright Laws of the United States and applicable international
// laws, treaties, and conventions.
//
// For additional information, contact:
// Environmental Systems Research Institute, Inc.
// Attn: Contracts and Legal Services Department
// 380 New York Street
// Redlands, California, 92373
// USA
//
// email: contracts@esri.com
/// \file GenerateGeodatabaseReplicaFromFeatureService.cpp

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

#include "GenerateGeodatabaseReplicaFromFeatureService.h"

#include "Map.h"
#include "MapQuickView.h"
#include "FeatureLayer.h"
#include "Basemap.h"
#include "SpatialReference.h"
#include "ServiceFeatureTable.h"
#include "ArcGISTiledLayer.h"
#include "ArcGISFeatureServiceInfo.h"
#include "Envelope.h"
#include "GenerateGeodatabaseParameters.h"
#include "GeodatabaseSyncTask.h"
#include "GeometryEngine.h"
#include "GenerateLayerOption.h"
#include "GeodatabaseFeatureTable.h"
#include "MapViewTypes.h"
#include "TileCache.h"
#include "Error.h"
#include "IdInfo.h"
#include "LayerListModel.h"
#include "GenerateGeodatabaseJob.h"
#include "TaskTypes.h"
#include "Geodatabase.h"
#include "TaskWatcher.h"
#include "Viewpoint.h"
#include "Point.h"

#include <QtCore/qglobal.h>
#include <QUrl>
#include <QStandardPaths>

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
  QString defaultDataPath()
  {
    QString dataPath;

  #ifdef Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  #else
    dataPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
  #endif

    return dataPath;
  }
} // namespace

GenerateGeodatabaseReplicaFromFeatureService::GenerateGeodatabaseReplicaFromFeatureService(QQuickItem* parent) :
  QQuickItem(parent),
  m_dataPath(defaultDataPath() + "/ArcGIS/Runtime/Data/")
{
}

GenerateGeodatabaseReplicaFromFeatureService::~GenerateGeodatabaseReplicaFromFeatureService() = default;

void GenerateGeodatabaseReplicaFromFeatureService::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<GenerateGeodatabaseReplicaFromFeatureService>("Esri.Samples", 1, 0, "GenerateGeodatabaseReplicaFromFeatureServiceSample");
}

void GenerateGeodatabaseReplicaFromFeatureService::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  //! [Create a map using a local tile package]
  TileCache* tileCache = new TileCache(m_dataPath + "tpkx/SanFrancisco.tpkx", this);
  ArcGISTiledLayer* tiledLayer = new ArcGISTiledLayer(tileCache, this);
  Basemap* basemap = new Basemap(tiledLayer, this);
  m_map = new Map(basemap, this);
  //! [Create a map using a local tile package]

  // set an initial viewpoint
  Envelope env(-122.50017, 37.74500, -122.43843, 37.81638, SpatialReference(4326));
  Viewpoint viewpoint(env);
  m_map->setInitialViewpoint(viewpoint);

  // Set map to map view
  m_mapView->setMap(m_map);

  //! [Features GenerateGeodatabase Part 1]
  // create the GeodatabaseSyncTask
  m_syncTask = new GeodatabaseSyncTask(QUrl(m_featureServiceUrl), this);
  //! [Features GenerateGeodatabase Part 1]

  // connect to sync task doneLoading signal
  connect(m_syncTask, &GeodatabaseSyncTask::doneLoading, this, [this](const Error& error)
  {
    if (!error.isEmpty())
    {
      emit updateStatus("Generate failed");
      emit hideWindow(5000, false);
      return;
    }

    // add online feature layers to the map, and obtain service IDs
    m_featureServiceInfo = m_syncTask->featureServiceInfo();
    const auto infos = m_featureServiceInfo.layerInfos();
    for (const IdInfo& idInfo : infos)
    {
      // get the layer ID from the idInfo
      QString id = QString::number(idInfo.infoId());

      // add the layer to the map
      QUrl featureLayerUrl(m_featureServiceInfo.url().toString() + "/" + id);
      ServiceFeatureTable* serviceFeatureTable = new ServiceFeatureTable(featureLayerUrl, this);
      FeatureLayer* featureLayer = new FeatureLayer(serviceFeatureTable, this);
      m_map->operationalLayers()->append(featureLayer);

      // add the layer id to the string list
      m_serviceIds << id;
    }

  });

  // connect to map doneLoading signal
  connect(m_map, &Map::doneLoading, this, [this](const Error& error)
  {
    if (error.isEmpty())
    {
      // load the sync task once the map loads
      m_syncTask->load();
    }
  });
}

void GenerateGeodatabaseReplicaFromFeatureService::addFeatureLayers(const QString& serviceUrl, const QStringList& serviceIds)
{
  for (const QString& id : serviceIds)
  {
    ServiceFeatureTable* serviceFeatureTable = new ServiceFeatureTable(QUrl(serviceUrl + id), this);
    FeatureLayer* featureLayer = new FeatureLayer(serviceFeatureTable, this);
    m_map->operationalLayers()->append(featureLayer);
  }
}

//! [Features GenerateGeodatabase Part 2]
GenerateGeodatabaseParameters GenerateGeodatabaseReplicaFromFeatureService::getUpdatedParameters(Envelope gdbExtent)
{
  // create the parameters
  GenerateGeodatabaseParameters params;
  params.setReturnAttachments(false);
  params.setOutSpatialReference(SpatialReference::webMercator());
  params.setExtent(gdbExtent);

  // set the layer options for all of the service IDs
  QList<GenerateLayerOption> layerOptions;
  for (const QString& id : qAsConst(m_serviceIds))
  {
    GenerateLayerOption generateLayerOption(id.toInt());
    layerOptions << generateLayerOption;
  }
  params.setLayerOptions(layerOptions);

  return params;
}

void GenerateGeodatabaseReplicaFromFeatureService::generateGeodatabaseFromCorners(double xCorner1, double yCorner1, double xCorner2, double yCorner2)
{
  // create an envelope from the QML rectangle corners
  const Point corner1 = m_mapView->screenToLocation(xCorner1, yCorner1);
  const Point corner2 = m_mapView->screenToLocation(xCorner2, yCorner2);
  const Envelope extent(corner1, corner2);
  const Envelope geodatabaseExtent = geometry_cast<Envelope>(GeometryEngine::project(extent, SpatialReference::webMercator()));

  // get the updated parameters
  GenerateGeodatabaseParameters params = getUpdatedParameters(geodatabaseExtent);

  // execute the task and obtain the job
  const QString outputGdb = m_tempPath.path() + "/wildfire.geodatabase";
  GenerateGeodatabaseJob* generateJob = m_syncTask->generateGeodatabase(params, outputGdb);

  // connect to the job's status changed signal
  if (generateJob)
  {
    connect(generateJob, &GenerateGeodatabaseJob::statusChanged, this, [this, generateJob](JobStatus jobStatus)
    {
      // connect to the job's status changed signal to know once it is done
      switch (jobStatus) {
      case JobStatus::Failed:
        emit updateStatus("Generate failed");
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
        emit updateStatus("Complete");
        emit hideWindow(1500, true);
        addOfflineData(generateJob->result());
        break;
      default:
        break;
      }
    });

    // start the generate job
    generateJob->start();
  }
  else
  {
    emit updateStatus("Generate failed");
    emit hideWindow(5000, false);
  }
}
//! [Features GenerateGeodatabase Part 2]

void GenerateGeodatabaseReplicaFromFeatureService::addOfflineData(Geodatabase* gdb)
{
  // remove the original online feature layers
  m_map->operationalLayers()->clear();

  // load the geodatabase
  connect(gdb, &Geodatabase::doneLoading, this, [this, gdb](const Error&)
  {
    // create a feature layer from each feature table, and add to the map
    const auto tables = gdb->geodatabaseFeatureTables();
    for (GeodatabaseFeatureTable* featureTable : tables)
    {
      FeatureLayer* featureLayer = new FeatureLayer(featureTable, this);
      m_map->operationalLayers()->append(featureLayer);
    }

    // unregister geodatabase since there will be no edits uploaded
    m_syncTask->unregisterGeodatabase(gdb);
  });
  gdb->load();
}
