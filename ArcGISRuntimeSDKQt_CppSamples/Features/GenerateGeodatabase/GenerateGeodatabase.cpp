// [WriteFile Name=GenerateGeodatabase, Category=Features]
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

#include "GenerateGeodatabase.h"

#include "Map.h"
#include "MapQuickView.h"
#include "FeatureLayer.h"
#include "Basemap.h"
#include "SpatialReference.h"
#include "ServiceFeatureTable.h"
#include "ArcGISTiledLayer.h"
#include "Envelope.h"
#include "GenerateGeodatabaseParameters.h"
#include "GeodatabaseSyncTask.h"
#include "GeometryEngine.h"
#include "GenerateLayerOption.h"
#include "GeodatabaseFeatureTable.h"
#include <QUrl>
#include <QQmlProperty>

using namespace Esri::ArcGISRuntime;

GenerateGeodatabase::GenerateGeodatabase(QQuickItem* parent) :
    QQuickItem(parent),
    m_map(nullptr),
    m_mapView(nullptr),
    m_syncTask(nullptr),
    m_dataPath(""),
    m_featureServiceUrl("http://sampleserver6.arcgisonline.com/arcgis/rest/services/Sync/WildfireSync/FeatureServer/"),
    m_serviceIds(QStringList() << "0" << "1" << "2")
{
}

GenerateGeodatabase::~GenerateGeodatabase()
{
}

void GenerateGeodatabase::componentComplete()
{
    QQuickItem::componentComplete();

    // find QML MapView component
    m_mapView = findChild<MapQuickView*>("mapView");
    m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

    // Create a map using a local tile package
    m_dataPath = QQmlProperty::read(m_mapView, "dataPath").toString();
    TileCache* tileCache = new TileCache(m_dataPath + "tpk/SanFrancisco.tpk", this);
    ArcGISTiledLayer* tiledLayer = new ArcGISTiledLayer(tileCache, this);
    Basemap* basemap = new Basemap(tiledLayer, this);
    m_map = new Map(basemap, this);

    // Set map to map view
    m_mapView->setMap(m_map);

    // add feature layers to map
    addFeatureLayers(m_featureServiceUrl, m_serviceIds);

    // create the GeodatabaseSyncTask
    m_syncTask = new GeodatabaseSyncTask(QUrl(m_featureServiceUrl), this);
}

void GenerateGeodatabase::addFeatureLayers(QString serviceUrl, QStringList serviceIds)
{
    foreach (auto id, serviceIds)
    {
        ServiceFeatureTable* serviceFeatureTable = new ServiceFeatureTable(QUrl(serviceUrl + id));
        FeatureLayer* featureLayer = new FeatureLayer(serviceFeatureTable, this);
        m_map->operationalLayers()->append(featureLayer);
    }
}

GenerateGeodatabaseParameters GenerateGeodatabase::getUpdatedParameters(Envelope gdbExtent)
{
    // create the parameters
    GenerateGeodatabaseParameters params;
    params.setReturnAttachments(false);
    params.setOutSpatialReference(SpatialReference::webMercator());
    params.setExtent(gdbExtent);

    // set the layer options for all of the service IDs
    QList<GenerateLayerOption> layerOptions;
    foreach (auto id, m_serviceIds)
    {
        GenerateLayerOption generateLayerOption(id.toInt());
        layerOptions << generateLayerOption;
    }
    params.setLayerOptions(layerOptions);

    return params;
}

void GenerateGeodatabase::generateGeodatabaseFromCorners(double xCorner1, double yCorner1, double xCorner2, double yCorner2)
{
    // create an envelope from the QML rectangle corners
    auto corner1 = m_mapView->screenToLocation(xCorner1, yCorner1);
    auto corner2 = m_mapView->screenToLocation(xCorner2, yCorner2);
    auto extent = Envelope(corner1, corner2);
    auto geodatabaseExtent = GeometryEngine::project(extent, SpatialReference::webMercator());

    // get the updated parameters
    auto params = getUpdatedParameters(geodatabaseExtent);

    // execute the task and obtain the job
    auto generateJob = m_syncTask->generateGeodatabase(params, m_dataPath + "Wildfire.geodatabase");

    // connect to the job's status changed signal
    connect(generateJob, &GenerateGeodatabaseJob::jobStatusChanged, [this, generateJob]()
    {
        // connect to the job's status changed signal to know once it is done
        switch (generateJob->jobStatus()) {
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

void GenerateGeodatabase::addOfflineData(Geodatabase* gdb)
{
    // remove the original online feature layers
    m_map->operationalLayers()->clear();

    // load the geodatabase
    connect(gdb, &Geodatabase::doneLoading, [this, gdb](Error)
    {
        // create a feature layer from each feature table, and add to the map
        foreach (auto featureTable, gdb->geodatabaseFeatureTables())
        {
            FeatureLayer* featureLayer = new FeatureLayer(featureTable, this);
            m_map->operationalLayers()->append(featureLayer);
        }

        // unregister geodatabase sincethere will be no edits uploaded
        m_syncTask->unregisterGeodatabase(gdb);
    });
    gdb->load();
}
