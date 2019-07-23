// [WriteFile Name=FindClosestFacilityToMultipleIncidentsService, Category=Routing]
// [Legal]
// Copyright 2019 Esri.

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

#include "FindClosestFacilityToMultipleIncidentsService.h"

#include "Map.h"
#include "MapQuickView.h"
#include "PictureMarkerSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleRenderer.h"
#include "ServiceFeatureTable.h"
#include "FeatureLayer.h"
#include "GraphicsOverlay.h"
#include "Graphic.h"
#include "TaskWatcher.h"
#include "GeometryEngine.h"
#include "ClosestFacilityTask.h"
#include "ClosestFacilityParameters.h"
#include "ClosestFacilityResult.h"
#include "ClosestFacilityRoute.h"

using namespace Esri::ArcGISRuntime;

FindClosestFacilityToMultipleIncidentsService::FindClosestFacilityToMultipleIncidentsService(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::streetsWithReliefVector(this), this)),
  m_task(new ClosestFacilityTask(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/NetworkAnalysis/SanDiego/NAServer/ClosestFacility"), this)),
  m_resultsOverlay(new GraphicsOverlay(this))
{
  // enable busy indicator while loading
  m_busy = true;
  emit busyChanged();

  createSymbols();

  createFeatureLayers();

  // connect to doneLoading signal of closest facility task
  connect(m_task, &ClosestFacilityTask::doneLoading, this, [this](Error e)
  {
    if (!e.isEmpty())
    {
      qDebug() << e.message();
      return;
    }
    setupRouting();
  });
  // load closest facility task
  m_task->load();
}

FindClosestFacilityToMultipleIncidentsService::~FindClosestFacilityToMultipleIncidentsService() = default;

void FindClosestFacilityToMultipleIncidentsService::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<FindClosestFacilityToMultipleIncidentsService>("Esri.Samples", 1, 0, "FindClosestFacilityToMultipleIncidentsServiceSample");
}

MapQuickView* FindClosestFacilityToMultipleIncidentsService::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void FindClosestFacilityToMultipleIncidentsService::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}

void FindClosestFacilityToMultipleIncidentsService::createSymbols()
{
  // create picture marker symbol for facilities
  m_facilitySymbol = new PictureMarkerSymbol(QUrl("https://static.arcgis.com/images/Symbols/SafetyHealth/FireStation.png"), this);
  m_facilitySymbol->setWidth(30.0f);
  m_facilitySymbol->setHeight(30.0f);

  // create picture marker symbol for incidents
  m_incidentSymbol = new PictureMarkerSymbol(QUrl("https://static.arcgis.com/images/Symbols/SafetyHealth/esriCrimeMarker_56_Gradient.png"), this);
  m_incidentSymbol->setWidth(30.0f);
  m_incidentSymbol->setHeight(30.0f);

  // create simple line symbol for the route
  m_routeSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::blue, 2.0f, this);
}

void FindClosestFacilityToMultipleIncidentsService::createFeatureLayers()
{
  // create service feature table from feature server
  m_facilitiesFeatureTable = new ServiceFeatureTable(QUrl("https://services2.arcgis.com/ZQgQTuoyBrtmoGdP/ArcGIS/rest/services/San_Diego_Facilities/FeatureServer/0"), this);
  // create feature layer from feature service table
  m_facilitiesFeatureLayer = new FeatureLayer(m_facilitiesFeatureTable, this);
  // apply the facilities symbol as the renderer for this layer
  m_facilitiesFeatureLayer->setRenderer(new SimpleRenderer(m_facilitySymbol, this));

  // create service feature table from feature server
  m_incidentsFeatureTable = new ServiceFeatureTable(QUrl("https://services2.arcgis.com/ZQgQTuoyBrtmoGdP/ArcGIS/rest/services/San_Diego_Incidents/FeatureServer/0"), this);
  // create feature layer from feature service table
  m_incidentsFeatureLayer = new FeatureLayer(m_incidentsFeatureTable, this);
  // apply the incidents symbol as the renderer for this layer
  m_incidentsFeatureLayer->setRenderer(new SimpleRenderer(m_incidentSymbol, this));

  // connect to the doneLoading signal which calls the slot to set the view point geometry
  connect(m_facilitiesFeatureTable, &ServiceFeatureTable::doneLoading, this, &FindClosestFacilityToMultipleIncidentsService::setViewpointGeometry);
  connect(m_incidentsFeatureTable, &ServiceFeatureTable::doneLoading, this, &FindClosestFacilityToMultipleIncidentsService::setViewpointGeometry);

  // load both service feature tables
  m_facilitiesFeatureTable->load();
  m_incidentsFeatureTable->load();
}

void FindClosestFacilityToMultipleIncidentsService::setupRouting()
{
  // connect to createDefaultParametersCompleted signal of the closest facility task
  connect(m_task, &ClosestFacilityTask::createDefaultParametersCompleted, this, [this](QUuid, const ClosestFacilityParameters& defaultParameters)
  {
    // create query parameters to query features from feature table
    QueryParameters params;
    params.setWhereClause("1=1");
    m_facilityParams = defaultParameters;
    // set closest facilities parameters facilities
    m_facilityParams.setFacilitiesWithFeatureTable(m_facilitiesFeatureTable, params);
    // set closest facilitites parameters incidents
    m_facilityParams.setIncidentsWithFeatureTable(m_incidentsFeatureTable, params);

    // disable busy indicator
    m_busy = false;
    // enable solve button
    m_solveButtonEnabled = true;
    emit busyChanged();
    emit solveButtonChanged();
  });

  // connect to solveClosestFacilityCompleted signal of the closest facility task
  connect(m_task, &ClosestFacilityTask::solveClosestFacilityCompleted, this, [this](QUuid, const ClosestFacilityResult& closestFacilityResult)
  {
    if (closestFacilityResult.isEmpty())
    {
      qDebug() << "Empty result";
      return;
    }

    // loop through all incidents
    for (int incidentIndex = 0; incidentIndex < m_incidentsFeatureTable->numberOfFeatures(); incidentIndex++)
    {
      // get the index of the facility closest to the incident
      const int closestFacilityIndex = closestFacilityResult.rankedFacilityIndexes(incidentIndex).first();
      // get the route between the incident and the facility
      const ClosestFacilityRoute route = closestFacilityResult.route(closestFacilityIndex, incidentIndex);
      // create a graphic from the routes geometry using the route symbol
      Graphic* m_routeGraphic = new Graphic(route.routeGeometry(), m_routeSymbol, this);
      // append the graphic to the graphics overlay
      m_resultsOverlay->graphics()->append(m_routeGraphic);
    }

    // append the graphics overlay to the mapview
    m_mapView->graphicsOverlays()->append(m_resultsOverlay);
    // enable the reset button
    m_resetButtonEnabled = true;
    emit resetButtonChanged();
    // disable the busy indicator
    m_busy = false;
    emit busyChanged();
  });

  // create default parameters for the closest facility task
  m_task->createDefaultParameters();
}

void FindClosestFacilityToMultipleIncidentsService::setViewpointGeometry(const Error& e)
{
  if (!e.isEmpty())
  {
    qDebug() << e.message();
    return;
  }

  // return if either or both layers are not loaded
  if ((m_facilitiesFeatureTable->loadStatus() != LoadStatus::Loaded) || (m_incidentsFeatureTable->loadStatus() != LoadStatus::Loaded))
    return;

  // return if set viewpoint task watcher has been created
  if (setViewpointTaskWatcher.isValid())
    return;

  // append both feature layers to the map
  m_mapView->map()->operationalLayers()->append(m_facilitiesFeatureLayer);
  m_mapView->map()->operationalLayers()->append(m_incidentsFeatureLayer);

  // set viewpoint geometry to the combined extents of the facilities and incidents feature layers
  setViewpointTaskWatcher = m_mapView->setViewpointGeometry(GeometryEngine::unionOf(m_facilitiesFeatureLayer->fullExtent(), m_incidentsFeatureLayer->fullExtent()), 20);
}

void FindClosestFacilityToMultipleIncidentsService::solveRoute()
{
  // enable the busy indicator
  m_busy = true;
  // disable the solve button
  m_solveButtonEnabled = false;
  emit busyChanged();
  emit solveButtonChanged();

  // solve the closest facility task
  m_task->solveClosestFacility(m_facilityParams);
}

void FindClosestFacilityToMultipleIncidentsService::resetGO()
{
  // clear the graphics overlay
  m_mapView->graphicsOverlays()->clear();
  // disable the reset button
  m_resetButtonEnabled = false;
  // enable the solve button
  m_solveButtonEnabled = true;
  emit solveButtonChanged();
  emit resetButtonChanged();
}
