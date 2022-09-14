// [WriteFile Name=GenerateOfflineMap_Overrides, Category=Maps]
// [Legal]
// Copyright 2018 Esri.

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

#include "GenerateOfflineMap_Overrides.h"

#include "Envelope.h"
#include "FeatureLayer.h"
#include "GeometryEngine.h"
#include "Map.h"
#include "MapQuickView.h"
#include "Portal.h"
#include "PortalItem.h"
#include "OfflineMapTask.h"
#include "Point.h"

using namespace Esri::ArcGISRuntime;

const QString GenerateOfflineMap_Overrides::s_webMapId = QStringLiteral("acc027394bc84c2fb04d1ed317aac674");

QString GenerateOfflineMap_Overrides::webMapId()
{
  return s_webMapId;
}

GenerateOfflineMap_Overrides::GenerateOfflineMap_Overrides(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void GenerateOfflineMap_Overrides::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<GenerateOfflineMap_Overrides>("Esri.Samples", 1, 0, "GenerateOfflineMap_OverridesSample");
}

void GenerateOfflineMap_Overrides::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a Portal Item for use by the Map and OfflineMapTask
  const bool loginRequired = false;
  Portal* portal = new Portal(loginRequired, this);
  m_portalItem = new PortalItem(portal, webMapId(), this);

  // Create a map from the Portal Item
  m_map = new Map(m_portalItem, this);

  // Update property once map is done loading
  connect(m_map, &Map::doneLoading, this, [this](Error e)
  {
    if (!e.isEmpty())
      return;

    m_mapLoaded = true;
    emit mapLoadedChanged();
  });

  // Set map to map view
  m_mapView->setMap(m_map);

  // Create the OfflineMapTask with the PortalItem
  m_offlineMapTask = new OfflineMapTask(m_portalItem, this);

  // connect to the error signal
  connect(m_offlineMapTask, &OfflineMapTask::errorOccurred, this, [](Error e)
  {
    if (e.isEmpty())
      return;

    qDebug() << e.message() << e.additionalMessage();
  });

  // connect to the signal for when the default parameters are generated
  connect(m_offlineMapTask, &OfflineMapTask::createDefaultGenerateOfflineMapParametersCompleted,
          this, [this](QUuid, const GenerateOfflineMapParameters& params)
  {
    // Use the parameters to create a set of overrides.
    m_parameters = params;
    m_offlineMapTask->createGenerateOfflineMapParameterOverrides(params);
  });

  connect(m_offlineMapTask, &OfflineMapTask::createGenerateOfflineMapParameterOverridesCompleted,
          this, [this](QUuid /*id*/, GenerateOfflineMapParameterOverrides* parameterOverrides)
  {
    m_parameterOverrides = parameterOverrides;
    emit overridesReadyChanged();
    setBusy(false);
    emit taskBusyChanged();
  });
}

void GenerateOfflineMap_Overrides::setAreaOfInterest(double xCorner1, double yCorner1, double xCorner2, double yCorner2)
{
  // create an envelope from the QML rectangle corners
  const Point corner1 = m_mapView->screenToLocation(xCorner1, yCorner1);
  const Point corner2 = m_mapView->screenToLocation(xCorner2, yCorner2);
  const Envelope extent(corner1, corner2);
  const Envelope mapExtent = geometry_cast<Envelope>(GeometryEngine::project(extent, SpatialReference::webMercator()));

  // generate parameters
  m_offlineMapTask->createDefaultGenerateOfflineMapParameters(mapExtent);
  setBusy(true);
  emit taskBusyChanged();
}

void GenerateOfflineMap_Overrides::setBasemapLOD(int min, int max)
{
  if (!overridesReady())
    return;

  LayerListModel* layers = m_map->basemap()->baseLayers();
  if (!layers || layers->isEmpty())
    return;

  // Obtain a key for the given basemap-layer.
  OfflineMapParametersKey keyForTiledLayer(layers->at(0));

  // Check that the key is valid.
  if (keyForTiledLayer.isEmpty() || keyForTiledLayer.type() != OfflineMapParametersType::ExportTileCache)
    return;

  // Obtain the dictionary of parameters for taking the basemap offline.
  QMap<OfflineMapParametersKey, ExportTileCacheParameters> dictionary = m_parameterOverrides->exportTileCacheParameters();
  if (!dictionary.contains(keyForTiledLayer))
    return;

  // Create a new sublist of LODs in the range requested by the user.
  QList<int> newLODs;
  for (int i = min; i < max; ++i )
    newLODs.append(i);

  // Apply the sublist as the LOD level in tilecache parameters for the given
  // service.
  ExportTileCacheParameters& exportTileCacheParam = dictionary[keyForTiledLayer];
  exportTileCacheParam.setLevelIds(newLODs);

  m_parameterOverrides->setExportTileCacheParameters(dictionary);
}

void GenerateOfflineMap_Overrides::setBasemapBuffer(int bufferMeters)
{
  if (!overridesReady())
    return;

  LayerListModel* layers = m_map->basemap()->baseLayers();
  if (!layers || layers->isEmpty())
    return;

  OfflineMapParametersKey keyForTiledLayer(layers->at(0));

  if (keyForTiledLayer.isEmpty() || keyForTiledLayer.type() != OfflineMapParametersType::ExportTileCache)
    return;

  // Obtain the dictionary of parameters for taking the basemap offline.
  QMap<OfflineMapParametersKey, ExportTileCacheParameters> dictionary = m_parameterOverrides->exportTileCacheParameters();
  if (!dictionary.contains(keyForTiledLayer))
    return;

  // Create a new geometry around the original area of interest.
  auto bufferGeom = GeometryEngine::buffer(m_parameters.areaOfInterest(), bufferMeters);

  // Apply the geometry to the ExportTileCacheParameters.
  ExportTileCacheParameters& exportTileCacheParam = dictionary[keyForTiledLayer];

  // Set the parameters back into the dictionary.
  exportTileCacheParam.setAreaOfInterest(bufferGeom);

  //  Store the dictionary.
  m_parameterOverrides->setExportTileCacheParameters(dictionary);
}

void GenerateOfflineMap_Overrides::removeSystemValves()
{
  removeFeatureLayer(QStringLiteral("System Valve"));
}

void GenerateOfflineMap_Overrides::removeServiceConnection()
{
  removeFeatureLayer(QStringLiteral("Service Connection"));
}

void GenerateOfflineMap_Overrides::setHydrantWhereClause(const QString& whereClause)
{
  if (!overridesReady())
    return;

  FeatureLayer* targetLayer = getFeatureLayerByName(QStringLiteral("Hydrant"));

  if (!targetLayer)
    return;

  // Get the parameter key for the layer.
  OfflineMapParametersKey keyForTargetLayer(targetLayer);

  if (keyForTargetLayer.isEmpty() || keyForTargetLayer.type() != OfflineMapParametersType::GenerateGeodatabase)
    return;

  // Get the dictionary of GenerateGeoDatabaseParameters.
  QMap<OfflineMapParametersKey, GenerateGeodatabaseParameters> dictionary = m_parameterOverrides->generateGeodatabaseParameters();

  auto keyIt = dictionary.find(keyForTargetLayer);
  if (keyIt == dictionary.end())
    return;

  // Grab the GenerateGeoDatabaseParameters associated with the given key.
  GenerateGeodatabaseParameters& generateGdbParam = keyIt.value();

  ServiceFeatureTable* table = qobject_cast<ServiceFeatureTable*>(targetLayer->featureTable());
  if (!table)
    return;

  // Get the service layer id for the given layer.
  const qint64 targetLayerId = table->layerInfo().serviceLayerId();

  // Set the whereClause on the required layer option.
  QList<GenerateLayerOption> layerOptions = generateGdbParam.layerOptions();
  for (auto& it : layerOptions)
  {
    GenerateLayerOption& option = it;
    if (option.layerId() == targetLayerId)
    {
      option.setWhereClause(whereClause);
      option.setQueryOption(GenerateLayerQueryOption::UseFilter);
      break;
    }
  }

  // Add layer options back to parameters and re-add to the dictionary.
  generateGdbParam.setLayerOptions(layerOptions);

  dictionary[keyForTargetLayer] = generateGdbParam;
  m_parameterOverrides->setGenerateGeodatabaseParameters(dictionary);
}

void GenerateOfflineMap_Overrides::setClipWaterPipesAOI(bool clip)
{
  if (!overridesReady())
    return;

  FeatureLayer* targetLayer = getFeatureLayerByName(QStringLiteral("Main"));

  if (!targetLayer)
    return;

  // Get the parameter key for the layer.
  OfflineMapParametersKey keyForTargetLayer(targetLayer);

  if (keyForTargetLayer.isEmpty() || keyForTargetLayer.type() != OfflineMapParametersType::GenerateGeodatabase)
    return;

  // Get the dictionary of GenerateGeoDatabaseParameters.
  QMap<OfflineMapParametersKey, GenerateGeodatabaseParameters> dictionary = m_parameterOverrides->generateGeodatabaseParameters();

  auto keyIt = dictionary.find(keyForTargetLayer);
  if (keyIt == dictionary.end())
    return;

  // Grab the GenerateGeoDatabaseParameters associated with the given key.
  GenerateGeodatabaseParameters& generateGdbParam = keyIt.value();

  ServiceFeatureTable* table = qobject_cast<ServiceFeatureTable*>(targetLayer->featureTable());
  if (!table)
    return;

  // Get the service layer id for the given layer.
  const qint64 targetLayerId = table->layerInfo().serviceLayerId();

  // Set whether to use the geometry filter to clip the waterpipes.
  // If not then we get all the features.
  QList<GenerateLayerOption> layerOptions = generateGdbParam.layerOptions();
  for (auto& it : layerOptions)
  {
    GenerateLayerOption& option = it;
    if (option.layerId() == targetLayerId)
    {
      option.setUseGeometry(clip);
      break;
    }
  }

  // Add layer options back to parameters and re-add to the dictionary.
  generateGdbParam.setLayerOptions(layerOptions);

  dictionary[keyForTargetLayer] = generateGdbParam;
  m_parameterOverrides->setGenerateGeodatabaseParameters(dictionary);
}

void GenerateOfflineMap_Overrides::takeMapOffline()
{
  if (!overridesReady())
    return;

  // create temp data path for offline mmpk
  const QString dataPath = m_tempPath.path() + "/offlinemap_overrides.mmpk";

  // Take the map offline with the original paramaters and the customized overrides.
  GenerateOfflineMapJob* generateJob = m_offlineMapTask->generateOfflineMap(m_parameters, dataPath, m_parameterOverrides);

  // check if there is a valid job
  if (!generateJob)
    return;

  // connect to the job's status changed signal
  connect(generateJob, &GenerateOfflineMapJob::statusChanged, this, [this, generateJob](JobStatus jobStatus)
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
      emit updateStatus("In progress");
      break;
    case JobStatus::Succeeded:
      // show any layer errors
      if (generateJob->result()->hasErrors())
      {
        QString layerErrors = "";
        const QMap<Layer*, Error>& layerErrorsMap = generateJob->result()->layerErrors();
        for (auto it = layerErrorsMap.cbegin(); it != layerErrorsMap.cend(); ++it)
        {
          layerErrors += it.key()->name() + ": " + it.value().message() + "\n";
        }
        emit showLayerErrors(layerErrors);
      }

      // show the map
      emit updateStatus("Complete");
      emit hideWindow(1500, true);
      m_mapView->setMap(generateJob->result()->offlineMap(this));
      break;
    default: // do nothing
      break;
    }
  });

  // connect to progress changed signal
  connect(generateJob, &GenerateOfflineMapJob::progressChanged, this, [this, generateJob]()
  {
    emit updateProgress(generateJob->progress());
  });

  // connect to the error signal
  connect(generateJob, &GenerateOfflineMapJob::errorOccurred, this, [](Error e)
  {
    if (e.isEmpty())
      return;

    qDebug() << e.message() << e.additionalMessage();
  });

  // start the generate job
  generateJob->start();
}

bool GenerateOfflineMap_Overrides::taskBusy() const
{
  return m_taskBusy;
}

bool GenerateOfflineMap_Overrides::overridesReady() const
{
  return m_parameterOverrides;
}


void GenerateOfflineMap_Overrides::removeFeatureLayer(const QString& layerName)
{
  if (!overridesReady())
    return;

  FeatureLayer* targetLayer = getFeatureLayerByName(layerName);
  if (!targetLayer)
    return;

  // Get the parameter key for the layer.
  OfflineMapParametersKey keyForTargetLayer(targetLayer);

  if (keyForTargetLayer.isEmpty() || keyForTargetLayer.type() != OfflineMapParametersType::GenerateGeodatabase)
    return;

  // Get the dictionary of GenerateGeoDatabaseParameters.
  QMap<OfflineMapParametersKey, GenerateGeodatabaseParameters> dictionary = m_parameterOverrides->generateGeodatabaseParameters();

  auto keyIt = dictionary.find(keyForTargetLayer);
  if (keyIt == dictionary.end())
    return;

  // Grab the GenerateGeoDatabaseParameters associated with the given key.
  GenerateGeodatabaseParameters& generateGdbParam = keyIt.value();

  ServiceFeatureTable* table = qobject_cast<ServiceFeatureTable*>(targetLayer->featureTable());
  if (!table)
    return;

  // Get the service layer id for the given layer.
  const qint64 targetLayerId = table->layerInfo().serviceLayerId();

  // Remove the layer option from the list.
  QList<GenerateLayerOption> layerOptions = generateGdbParam.layerOptions();
  for (auto it = layerOptions.begin(); it != layerOptions.end(); ++it)
  {
    if (it->layerId() == targetLayerId)
    {
      layerOptions.erase(it);
      break;
    }
  }

  // Add layer options back to parameters and re-add to the dictionary.
  generateGdbParam.setLayerOptions(layerOptions);

  dictionary[keyForTargetLayer] = generateGdbParam;
  m_parameterOverrides->setGenerateGeodatabaseParameters(dictionary);
}

FeatureLayer* GenerateOfflineMap_Overrides::getFeatureLayerByName(const QString& layerName)
{
  // Find the feature layer with the given name
  LayerListModel* opLayers = m_map->operationalLayers();
  for (int i = 0; i < opLayers->rowCount(); ++i)
  {
    Layer* candidateLayer = opLayers->at(i);
    if (!candidateLayer)
      continue;

    if (candidateLayer->layerType() == LayerType::FeatureLayer && candidateLayer->name().contains(layerName, Qt::CaseInsensitive))
    {
      return qobject_cast<FeatureLayer*>(candidateLayer);
    }
  }
  return nullptr;
}

void GenerateOfflineMap_Overrides::setBusy(bool busy)
{
  m_taskBusy = busy;
  emit taskBusyChanged();
}

bool GenerateOfflineMap_Overrides::mapLoaded() const
{
  return m_mapLoaded;
}
