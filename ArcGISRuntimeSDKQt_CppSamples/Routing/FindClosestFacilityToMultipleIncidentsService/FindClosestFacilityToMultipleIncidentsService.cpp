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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

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

  connect(m_task, &ClosestFacilityTask::doneLoading, this, [this](Error e)
  {
    if (!e.isEmpty())
    {
      qDebug() << e.message();
      return;
    }
    setupRouting();
  });
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
  m_facilitySymbol = new PictureMarkerSymbol(QUrl("https://static.arcgis.com/images/Symbols/SafetyHealth/FireStation.png"), this);
  m_facilitySymbol->setWidth(30.0f);
  m_facilitySymbol->setHeight(30.0f);

  m_incidentSymbol = new PictureMarkerSymbol(QUrl("https://static.arcgis.com/images/Symbols/SafetyHealth/esriCrimeMarker_56_Gradient.png"), this);
  m_incidentSymbol->setWidth(30.0f);
  m_incidentSymbol->setHeight(30.0f);

  m_routeSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::blue, 2.0f, this);
}

void FindClosestFacilityToMultipleIncidentsService::createFeatureLayers()
{
  m_facilitiesFeatureTable = new ServiceFeatureTable(QUrl("https://services2.arcgis.com/ZQgQTuoyBrtmoGdP/ArcGIS/rest/services/San_Diego_Facilities/FeatureServer/0"), this);
  m_facilitiesFeatureLayer = new FeatureLayer(m_facilitiesFeatureTable, this);
  m_facilitiesFeatureLayer->setRenderer(new SimpleRenderer(m_facilitySymbol, this));

  m_incidentsFeatureTable = new ServiceFeatureTable(QUrl("https://services2.arcgis.com/ZQgQTuoyBrtmoGdP/ArcGIS/rest/services/San_Diego_Incidents/FeatureServer/0"), this);
  m_incidentsFeatureLayer = new FeatureLayer(m_incidentsFeatureTable, this);
  m_incidentsFeatureLayer->setRenderer(new SimpleRenderer(m_incidentSymbol, this));

  // connect to the doneLoading signal which calls the slot to set the view point geometry
  connect(m_facilitiesFeatureTable, &ServiceFeatureTable::doneLoading, this, &FindClosestFacilityToMultipleIncidentsService::setViewpointGeometry);
  connect(m_incidentsFeatureTable, &ServiceFeatureTable::doneLoading, this, &FindClosestFacilityToMultipleIncidentsService::setViewpointGeometry);

  m_facilitiesFeatureTable->load();
  m_incidentsFeatureTable->load();
}

void FindClosestFacilityToMultipleIncidentsService::setupRouting()
{
  connect(m_task, &ClosestFacilityTask::createDefaultParametersCompleted, this, [this](QUuid, const ClosestFacilityParameters& defaultParameters)
  {
    QueryParameters params;
    params.setWhereClause("1=1");
    m_facilityParams = defaultParameters;
    m_facilityParams.setFacilitiesWithFeatureTable(m_facilitiesFeatureTable, params);
    m_facilityParams.setIncidentsWithFeatureTable(m_incidentsFeatureTable, params);

    m_busy = false;
    m_solveButtonEnabled = true;
    emit busyChanged();
    emit solveButtonChanged();
  });

  connect(m_task, &ClosestFacilityTask::solveClosestFacilityCompleted, this, [this](QUuid, const ClosestFacilityResult& closestFacilityResult)
  {
    if (closestFacilityResult.isEmpty())
    {
      qDebug() << "Empty result";
      return;
    }

    // finding the closest facility for each incident to create a route graphic between each pair
    for (int incidentIndex = 0; incidentIndex < m_incidentsFeatureTable->numberOfFeatures(); incidentIndex++)
    {
      const int closestFacilityIndex = closestFacilityResult.rankedFacilityIndexes(incidentIndex).first();
      const ClosestFacilityRoute route = closestFacilityResult.route(closestFacilityIndex, incidentIndex);
      Graphic* m_routeGraphic = new Graphic(route.routeGeometry(), m_routeSymbol, this);

      m_resultsOverlay->graphics()->append(m_routeGraphic);
    }

    m_mapView->graphicsOverlays()->append(m_resultsOverlay);
    m_resetButtonEnabled = true;
    emit resetButtonChanged();
    m_busy = false;
    emit busyChanged();
  });

  m_task->createDefaultParameters();
}

void FindClosestFacilityToMultipleIncidentsService::setViewpointGeometry(const Error& e)
{
  if (!e.isEmpty())
  {
    qDebug() << e.message();
    return;
  }

  // proceed only if both layers have been loaded
  if ((m_facilitiesFeatureTable->loadStatus() != LoadStatus::Loaded) || (m_incidentsFeatureTable->loadStatus() != LoadStatus::Loaded))
    return;

  // return if set viewpoint task watcher has already been created
  if (setViewpointTaskWatcher.isValid())
    return;

  m_mapView->map()->operationalLayers()->append(m_facilitiesFeatureLayer);
  m_mapView->map()->operationalLayers()->append(m_incidentsFeatureLayer);

  setViewpointTaskWatcher = m_mapView->setViewpointGeometry(GeometryEngine::unionOf(m_facilitiesFeatureLayer->fullExtent(), m_incidentsFeatureLayer->fullExtent()), 20);
}

void FindClosestFacilityToMultipleIncidentsService::solveRoute()
{
  m_busy = true;
  m_solveButtonEnabled = false;
  emit busyChanged();
  emit solveButtonChanged();

  m_task->solveClosestFacility(m_facilityParams);
}

void FindClosestFacilityToMultipleIncidentsService::resetGO()
{
  m_mapView->graphicsOverlays()->clear();
  m_resetButtonEnabled = false;
  m_solveButtonEnabled = true;
  emit solveButtonChanged();
  emit resetButtonChanged();
}
