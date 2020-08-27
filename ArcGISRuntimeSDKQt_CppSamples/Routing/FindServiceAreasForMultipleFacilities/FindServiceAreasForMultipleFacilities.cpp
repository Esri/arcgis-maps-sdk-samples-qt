// [WriteFile Name=FindServiceAreasForMultipleFacilities, Category=Routing]
// [Legal]
// Copyright 2020 Esri.

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

#include "FindServiceAreasForMultipleFacilities.h"

#include "FeatureLayer.h"
#include "GraphicsOverlay.h"
#include "Map.h"
#include "MapQuickView.h"
#include "PictureMarkerSymbol.h"
#include "QueryParameters.h"
#include "ServiceAreaParameters.h"
#include "ServiceAreaPolygon.h"
#include "ServiceAreaResult.h"
#include "ServiceAreaTask.h"
#include "ServiceFeatureTable.h"
#include "SimpleFillSymbol.h"
#include "SimpleRenderer.h"
#include "TaskWatcher.h"

#include <QUrl>

using namespace Esri::ArcGISRuntime;

namespace
{
const QUrl url("https://services2.arcgis.com/ZQgQTuoyBrtmoGdP/ArcGIS/rest/services/San_Diego_Facilities/FeatureServer/0");
const QUrl imageUrl("https://static.arcgis.com/images/Symbols/SafetyHealth/Hospital.png");
const QUrl serviceAreaTaskUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/NetworkAnalysis/SanDiego/NAServer/ServiceArea");
}

FindServiceAreasForMultipleFacilities::FindServiceAreasForMultipleFacilities(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::lightGrayCanvas(this), this)),
  m_serviceAreasOverlay(new GraphicsOverlay(this))
{
  // create fill symbols for rendering the results
  m_fillSymbols.append(new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor(255, 166, 0, 66), this)); // translucent orange
  m_fillSymbols.append(new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor(255, 0, 0, 66), this)); // translucent red

  m_facilitiesTable = new ServiceFeatureTable(url, this);
  m_facilitiesFeatureLayer = new FeatureLayer(m_facilitiesTable, this);

  // create a symbol to display the facilities
  PictureMarkerSymbol* facilitiesSymbol = new PictureMarkerSymbol(imageUrl, this);
  facilitiesSymbol->setWidth(25);
  facilitiesSymbol->setHeight(25);
  m_facilitiesFeatureLayer->setRenderer(new SimpleRenderer(facilitiesSymbol, this));

  // add the facilities feature layer to the map
  m_map->operationalLayers()->append(m_facilitiesFeatureLayer);
}

FindServiceAreasForMultipleFacilities::~FindServiceAreasForMultipleFacilities() = default;

void FindServiceAreasForMultipleFacilities::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<FindServiceAreasForMultipleFacilities>("Esri.Samples", 1, 0, "FindServiceAreasForMultipleFacilitiesSample");
}

MapQuickView* FindServiceAreasForMultipleFacilities::mapView() const
{
  return m_mapView;
}

bool FindServiceAreasForMultipleFacilities::taskRunning() const
{
  return !m_taskWatcher.isDone();
}

// Set the view (created in QML)
void FindServiceAreasForMultipleFacilities::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;

  connect(m_facilitiesFeatureLayer, &FeatureLayer::doneLoading, this, [this](Error loadError)
  {
    if (!loadError.isEmpty())
    {
      qDebug() << loadError.message() << loadError.additionalMessage();
      return;
    }

    // zoom to the full extent of the feature layer
    int buffer = 100;
    m_mapView->setViewpointGeometry(m_facilitiesFeatureLayer->fullExtent(), buffer);

  });

  m_mapView->setMap(m_map);

  m_mapView->graphicsOverlays()->append(m_serviceAreasOverlay);



  emit mapViewChanged();
}

void FindServiceAreasForMultipleFacilities::connectServiceAreaTaskSignals()
{
  // once service area task is done loading, create default parameters
  connect(m_serviceAreaTask, &ServiceAreaTask::doneLoading, this, [this](Error loadError)
  {
    if (!loadError.isEmpty())
    {
      qDebug() << loadError.message() << loadError.additionalMessage();
      return;
    }
    m_serviceAreaTask->createDefaultParameters();
  });

  // once default parameters created, set parameters and solve
  connect(m_serviceAreaTask, &ServiceAreaTask::createDefaultParametersCompleted, this, [this](QUuid, ServiceAreaParameters serviceAreaParameters)
  {
    serviceAreaParameters.setPolygonDetail(ServiceAreaPolygonDetail::High);
    serviceAreaParameters.setReturnPolygons(true);

    QList<double> travelTimes = {1.0, 3.0};

    // add service areas of 1 minute and 3 minutes travel time by car
    serviceAreaParameters.setDefaultImpedanceCutoffs(travelTimes);

    // create query parameters used to select all facilities from the feature table
    QueryParameters queryParameters;
    queryParameters.setWhereClause("1=1");

    // add all facilities to the service area parameters
    serviceAreaParameters.setFacilitiesWithFeatureTable(m_facilitiesTable, queryParameters);

    m_taskWatcher = m_serviceAreaTask->solveServiceArea(serviceAreaParameters);
    if (!m_taskWatcher.isValid())
      qWarning() << "Task not valid.";

    emit taskRunningChanged();
  });

  connect(m_serviceAreaTask, &ServiceAreaTask::solveServiceAreaCompleted, this, [this](QUuid, ServiceAreaResult serviceAreaResult)
  {
    emit taskRunningChanged();

    // iterate through the facilities to get the service area polygons
    for (int i = 0; i < serviceAreaResult.facilities().size(); ++i)
    {
      QList<ServiceAreaPolygon> serviceAreaPolygonList = serviceAreaResult.resultPolygons(i);
      // create a graphic for each available polygon
      for (int j = 0; j < serviceAreaPolygonList.size(); ++j)
      {
        m_serviceAreasOverlay->graphics()->append(new Graphic(serviceAreaPolygonList[j].geometry(), m_fillSymbols[j], this));
      }
    }
  });
}

void FindServiceAreasForMultipleFacilities::findServiceAreas()
{
  if (m_serviceAreaTask)
    return;

  m_serviceAreaTask = new ServiceAreaTask(serviceAreaTaskUrl, this);
  connectServiceAreaTaskSignals();
  m_serviceAreaTask->load();
}
