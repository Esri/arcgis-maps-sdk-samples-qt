// [WriteFile Name=ClosestFacilitySample, Category=Routing]
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

#include "ClosestFacilitySample.h"

#include "ClosestFacilityParameters.h"
#include "ClosestFacilityTask.h"
#include "Facility.h"
#include "Map.h"
#include "MapQuickView.h"
#include "PictureMarkerSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleRenderer.h"

using namespace Esri::ArcGISRuntime;

const QUrl ClosestFacilitySample::facilityImageUrl(
        QStringLiteral("http://static.arcgis.com/images/Symbols/SafetyHealth/Hospital.png"));

const QUrl ClosestFacilitySample::sanDiegoRegion(
        QStringLiteral("http://sampleserver6.arcgisonline.com/arcgis/rest/services/NetworkAnalysis/SanDiego/NAServer/ClosestFacility"));

ClosestFacilitySample::ClosestFacilitySample(QQuickItem* parent /* = nullptr */):
    QQuickItem(parent),
    m_task(new ClosestFacilityTask(sanDiegoRegion, this))
{
}

ClosestFacilitySample::~ClosestFacilitySample()
{
}

void ClosestFacilitySample::componentComplete()
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

bool ClosestFacilitySample::busy() const
{
    return m_busy;
}

QString ClosestFacilitySample::message() const
{
    return m_message;
}

void ClosestFacilitySample::setBusy(bool val)
{
    if (m_busy == val)
        return;

    m_message.clear();
    m_busy = val;
    emit busyChanged();
    emit messageChanged();
}

void ClosestFacilitySample::createFacilities()
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

void ClosestFacilitySample::createGraphics()
{
    // create a symbol for the graphics
    PictureMarkerSymbol* facilitySymbol = new PictureMarkerSymbol(facilityImageUrl, this);
    facilitySymbol->setHeight(30);
    facilitySymbol->setWidth(30);

    // create a graphics overlay for the facilities and add a renderer for the symbol
    GraphicsOverlay* facilityOverlay = new GraphicsOverlay(this);
    SimpleRenderer* renderer = new SimpleRenderer(facilitySymbol, this);
    facilityOverlay->setRenderer(renderer);

    // add a graphic at the location of each of the facilities
    for (const Facility& facility : m_facilities)
    {
        Graphic* facilityGraphic = new Graphic(facility.geometry(), this);
        facilityOverlay->graphics()->append(facilityGraphic);
    }

    m_mapView->graphicsOverlays()->append(facilityOverlay);

    m_resultsOverlay = new GraphicsOverlay(this);
    m_mapView->graphicsOverlays()->append(m_resultsOverlay);
}

void ClosestFacilitySample::setupRouting()
{
    connect(m_task, &ClosestFacilityTask::createDefaultParametersCompleted, this, [this](
            QUuid taskId, Esri::ArcGISRuntime::ClosestFacilityParameters defaultParameters)
    {
        Q_UNUSED(taskId);
        setBusy(false);

        m_facilityParams = defaultParameters;
        m_facilityParams.setFacilities(m_facilities);
    });

    float lineWidth = 2.0f;
    SimpleLineSymbol* routeSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::blue, lineWidth, this);
    connect(m_task, &ClosestFacilityTask::solveClosestFacilityCompleted, this, [this, routeSymbol]
            (QUuid taskId, ClosestFacilityResult closestFacilityResult)
    {
        Q_UNUSED(taskId);
        setBusy(false);

        if (closestFacilityResult.isEmpty())
        {
            m_message = "Incident not within San Diego Area!";
            emit messageChanged();
        }

        QList<int> rankedList = closestFacilityResult.rankedFacilities(0);
        int closestFacilityIdx = rankedList.first();

        int incidentIndex = 0; // 0 since there is just 1 incident at a time
        ClosestFacilityRoute route = closestFacilityResult.route(closestFacilityIdx, incidentIndex);

        Graphic* routeGraphic = new Graphic(route.routeGeometry(), routeSymbol, this);
        m_resultsOverlay->graphics()->append(routeGraphic);
    });

    SimpleMarkerSymbol* incidentSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Cross, Qt::black, 20, this);
    connect(m_mapView, &MapQuickView::mouseClicked, this, [this, incidentSymbol](QMouseEvent& mouseEvent)
    {
        if (busy())
            return;

        m_resultsOverlay->graphics()->clear();

        Point mapPoint = m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y());
        Point incidentPoint(mapPoint.x(), mapPoint.y(), SpatialReference::webMercator());

        Graphic* incidentGraphic = new Graphic(incidentPoint, incidentSymbol, this);
        m_resultsOverlay->graphics()->append(incidentGraphic);

        solveRoute(incidentPoint);
    });

    setBusy(true);
    m_task->createDefaultParameters();
}

void ClosestFacilitySample::solveRoute(const Esri::ArcGISRuntime::Point& incidentPoint)
{
    m_facilityParams.clearIncidents();
    m_facilityParams.setIncidents(QList<Incident> {Incident(incidentPoint)});

    setBusy(true);
    // find the closest facility to the incident
    m_task->solveClosestFacility(m_facilityParams);
}

