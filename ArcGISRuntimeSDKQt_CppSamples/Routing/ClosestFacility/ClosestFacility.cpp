// [WriteFile Name=FindClosestFacilityToAnIncidentInteractive, Category=Routing]
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "ClosestFacility.h"

#include "ClosestFacilityParameters.h"
#include "ClosestFacilityTask.h"
#include "Facility.h"
#include "Map.h"
#include "MapQuickView.h"
#include "PictureMarkerSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleRenderer.h"

using namespace Esri::ArcGISRuntime;

const QUrl ClosestFacility::facilityImageUrl(
    QStringLiteral("https://static.arcgis.com/images/Symbols/SafetyHealth/Hospital.png"));

const QUrl ClosestFacility::sanDiegoRegion(
    QStringLiteral("https://sampleserver6.arcgisonline.com/arcgis/rest/services/NetworkAnalysis/SanDiego/NAServer/ClosestFacility"));

ClosestFacility::ClosestFacility(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_task(new ClosestFacilityTask(sanDiegoRegion, this))
{
}

ClosestFacility::~ClosestFacility() = default;

void ClosestFacility::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ClosestFacility>("Esri.Samples", 1, 0, "ClosestFacilitySample");
}


void ClosestFacility::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create a map using the streets basemap
  m_map = new Map(Basemap::streets(this), this);

  // set view to be over San Diego
  m_map->setInitialViewpoint(Viewpoint(Point(-13041154, 3858170, SpatialReference(3857)), 1e5));

  // Set map to map view
  m_mapView->setMap(m_map);

  createFacilities();
  createGraphics();

  connect(m_task, &ClosestFacilityTask::doneLoading, this, [this] (Error loadError)
  {
    if (!loadError.isEmpty())
      return;

    if (m_task->loadStatus() != LoadStatus::Loaded)
      return;

    setupRouting();
  });

  m_task->load();
}

bool ClosestFacility::busy() const
{
  return m_busy;
}

QString ClosestFacility::message() const
{
  return m_message;
}

void ClosestFacility::setBusy(bool val)
{
  if (m_busy == val)
    return;

  m_message.clear();
  m_busy = val;
  emit busyChanged();
  emit messageChanged();
}

void ClosestFacility::createFacilities()
{
  // List of facilities to be placed around San Diego area
  m_facilities.append(Facility(Point(-1.3042129900625112E7, 3860127.9479775648, SpatialReference::webMercator())));
  m_facilities.append(Facility(Point(-1.3042193400557665E7, 3862448.873041752, SpatialReference::webMercator())));
  m_facilities.append(Facility(Point(-1.3046882875518233E7, 3862704.9896770366, SpatialReference::webMercator())));
  m_facilities.append(Facility(Point(-1.3040539754780494E7, 3862924.5938606677, SpatialReference::webMercator())));
  m_facilities.append(Facility(Point(-1.3042571225655518E7, 3858981.773018156, SpatialReference::webMercator())));
  m_facilities.append(Facility(Point(-1.3039784633928463E7, 3856692.5980474586, SpatialReference::webMercator())));
  m_facilities.append(Facility(Point(-1.3049023883956768E7, 3861993.789732541, SpatialReference::webMercator())));
}

void ClosestFacility::createGraphics()
{
  // create a symbol for the graphics
  PictureMarkerSymbol* facilitySymbol = new PictureMarkerSymbol(facilityImageUrl, this);
  facilitySymbol->setHeight(30);
  facilitySymbol->setWidth(30);

  // create a graphics overlay for the facilities and add a renderer for the symbol
  m_facilityOverlay = new GraphicsOverlay(this);
  SimpleRenderer* facilityRenderer = new SimpleRenderer(facilitySymbol, this);
  m_facilityOverlay->setRenderer(facilityRenderer);

  // add a graphic at the location of each of the facilities
  for (const Facility& facility : m_facilities)
  {
    Graphic* facilityGraphic = new Graphic(facility.geometry(), this);
    m_facilityOverlay->graphics()->append(facilityGraphic);
  }

  // add the overlay to the map view
  m_mapView->graphicsOverlays()->append(m_facilityOverlay);

  // create a symbol for the route graphic
  float lineWidth = 2.0f;
  SimpleLineSymbol* routeSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::blue, lineWidth, this);

  // create a graphics overlay for the routes and add a renderer for the symbol
  m_resultsOverlay = new GraphicsOverlay(this);
  SimpleRenderer* routeRenderer = new SimpleRenderer(routeSymbol, this);
  m_resultsOverlay->setRenderer(routeRenderer);

  // add the overlay to the map view
  m_mapView->graphicsOverlays()->append(m_resultsOverlay);

  // add a graphic to the route overlay
  m_routeGraphic = new Graphic(this);
  m_resultsOverlay->graphics()->append(m_routeGraphic);

  // create a symbol for the incident graphics
  SimpleMarkerSymbol* incidentSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Cross, Qt::black, 20, this);

  // create a graphics overlay for the incidents and add a renderer for the symbol
  m_incidentsOverlay = new GraphicsOverlay(this);
  SimpleRenderer* incidentRenderer = new SimpleRenderer(incidentSymbol, this);
  m_incidentsOverlay->setRenderer(incidentRenderer);

  // add the overlay to the map view
  m_mapView->graphicsOverlays()->append(m_incidentsOverlay);

  // add a graphic to the incident overlay
  m_incidentGraphic = new Graphic(this);
  m_incidentsOverlay->graphics()->append(m_incidentGraphic);
}

void ClosestFacility::setupRouting()
{
  connect(m_task, &ClosestFacilityTask::createDefaultParametersCompleted, this, [this](
          QUuid, Esri::ArcGISRuntime::ClosestFacilityParameters defaultParameters)
  {
    setBusy(false);

    m_facilityParams = defaultParameters;
    m_facilityParams.setFacilities(m_facilities);
  });

  connect(m_task, &ClosestFacilityTask::solveClosestFacilityCompleted, this, [this]
          (QUuid, ClosestFacilityResult closestFacilityResult)
  {
    setBusy(false);

    if (closestFacilityResult.isEmpty())
    {
      m_message = "Incident not within San Diego Area!";
      emit messageChanged();
    }

    QList<int> rankedList = closestFacilityResult.rankedFacilityIndexes(0);
    int closestFacilityIdx = rankedList.first();

    int incidentIndex = 0; // 0 since there is just 1 incident at a time
    ClosestFacilityRoute route = closestFacilityResult.route(closestFacilityIdx, incidentIndex);

    m_routeGraphic->setGeometry(route.routeGeometry());
  });

  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    if (busy())
      return;    

    Point mapPoint = m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y());
    Point incidentPoint(mapPoint.x(), mapPoint.y(), SpatialReference::webMercator());

    m_incidentGraphic->setGeometry(incidentPoint);

    solveRoute(incidentPoint);
  });

  setBusy(true);
  m_task->createDefaultParameters();
}

void ClosestFacility::solveRoute(const Esri::ArcGISRuntime::Point& incidentPoint)
{
  m_facilityParams.clearIncidents();
  m_facilityParams.setIncidents(QList<Incident> {Incident(incidentPoint)});

  setBusy(true);
  // find the closest facility to the incident
  m_task->solveClosestFacility(m_facilityParams);
}
