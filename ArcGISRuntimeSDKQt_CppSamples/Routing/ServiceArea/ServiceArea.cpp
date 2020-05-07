// [WriteFile Name=ServiceArea, Category=Routing]
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

#include "ServiceArea.h"

#include "Map.h"
#include "MapQuickView.h"
#include "PictureMarkerSymbol.h"
#include "PolylineBuilder.h"
#include "ServiceAreaTask.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleRenderer.h"

using namespace Esri::ArcGISRuntime;

ServiceArea::ServiceArea(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_task(new ServiceAreaTask(
           QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/NetworkAnalysis/SanDiego/NAServer/ServiceArea"), this))
{
}

ServiceArea::~ServiceArea() = default;

void ServiceArea::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ServiceArea>("Esri.Samples", 1, 0, "ServiceAreaSample");
}

void ServiceArea::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // set view to be over San Diego
  m_map = new Map(Basemap::streets(this), this);
  m_map->setInitialViewpoint(Viewpoint(Point(-13041154, 3858170, SpatialReference(3857)), 1e5));

  // Set map to map view
  m_mapView->setMap(m_map);

  connect(m_task, &ServiceAreaTask::doneLoading, this, [this](Esri::ArcGISRuntime::Error loadError)
  {
    if (!loadError.isEmpty())
      return;

    if (m_task->loadStatus() != LoadStatus::Loaded)
      return;

    setupRouting();
  });
  m_task->load();

  setupGraphics();
}

void ServiceArea::setFacilityMode()
{
  m_mode = SampleMode::Facility;
}

void ServiceArea::setBarrierMode()
{
  m_mode = SampleMode::Barrier;

  if (m_barrierBuilder)
    return;

  m_barrierBuilder = new PolylineBuilder(SpatialReference::webMercator(), this);
}

void ServiceArea::solveServiceArea()
{
  setBusy(true);

  m_parameters.clearFacilities();
  m_parameters.clearPolylineBarriers();

  GraphicListModel* facilitiesGraphics = m_facilitiesOverlay->graphics();
  if (!facilitiesGraphics || facilitiesGraphics->rowCount() == 0)
  {
    setBusy(false);
    m_message = "At least 1 Facility is required.";
    emit messageChanged();
    return;
  }

  QList<ServiceAreaFacility> facilities;
  facilities.reserve(facilitiesGraphics->rowCount());
  for (int f = 0; f < facilitiesGraphics->rowCount(); ++f)
  {
    Graphic* g = facilitiesGraphics->at(f);
    if (!g)
      continue;

    facilities.append(ServiceAreaFacility(g->geometry()));
  }
  m_parameters.setFacilities(facilities);

  GraphicListModel* barrierGraphics = m_barrierOverlay->graphics();
  QList<PolylineBarrier> barriers;
  barriers.reserve(barrierGraphics->rowCount());
  for (int b = 0; b < barrierGraphics->rowCount(); ++b)
  {
    Graphic* g = barrierGraphics->at(b);
    if (!g)
      continue;

    barriers.append(PolylineBarrier(g->geometry()));
  }

  if (!barriers.isEmpty())
    m_parameters.setPolylineBarriers(barriers);

  m_task->solveServiceArea(m_parameters);
}

void ServiceArea::reset()
{
  m_facilitiesOverlay->graphics()->clear();
  m_barrierOverlay->graphics()->clear();
  if (m_barrierBuilder)
  {
    delete m_barrierBuilder;
    m_barrierBuilder = new PolylineBuilder(SpatialReference::webMercator(), this);
  }
  m_areasOverlay->graphics()->clear();
  if (m_graphicParent)
  {
    delete m_graphicParent;
    m_graphicParent = nullptr;
  }
}

void ServiceArea::newBarrier()
{
  if (m_barrierBuilder)
  {
    delete m_barrierBuilder;
    m_barrierBuilder = nullptr;
  }

  setBarrierMode();
  if (!m_graphicParent)
    m_graphicParent = new QObject(this);
  m_barrierOverlay->graphics()->append(new Graphic(m_barrierBuilder->toPolyline(), m_graphicParent));
}

bool ServiceArea::busy() const
{
  return m_busy;
}

QString ServiceArea::message() const
{
  return m_message;
}

void ServiceArea::setBusy(bool val)
{
  if (m_busy == val)
    return;

  m_message.clear();
  m_busy = val;
  emit busyChanged();
  emit messageChanged();
}

void ServiceArea::setupGraphics()
{
  // create a symbol for the incidents
  PictureMarkerSymbol* facilitySymbol = new PictureMarkerSymbol(
        QUrl(QStringLiteral("https://static.arcgis.com/images/Symbols/SafetyHealth/Hospital.png")), this);
  facilitySymbol->setHeight(30);
  facilitySymbol->setWidth(30);

  // create a graphics overlay for the facilities and add a renderer for the symbol
  m_facilitiesOverlay = new GraphicsOverlay(this);
  SimpleRenderer* renderer = new SimpleRenderer(facilitySymbol, this);
  m_facilitiesOverlay->setRenderer(renderer);

  m_mapView->graphicsOverlays()->append(m_facilitiesOverlay);

  // create a symbol for the barriers
  float lineWidth = 3.0f;
  SimpleLineSymbol* lineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::black, lineWidth, this);
  m_barrierOverlay = new GraphicsOverlay(this);
  SimpleRenderer* lineRenderer = new SimpleRenderer(lineSymbol, this);
  m_barrierOverlay->setRenderer(lineRenderer);

  m_mapView->graphicsOverlays()->append(m_barrierOverlay);

  //create a symbol for the service areas
  SimpleFillSymbol* fillSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, Qt::green, lineSymbol, this);
  m_areasOverlay = new GraphicsOverlay(this);
  SimpleRenderer* areaRenderer = new SimpleRenderer(fillSymbol, this);
  m_areasOverlay->setRenderer(areaRenderer);
  m_areasOverlay->setOpacity(0.5f);

  m_mapView->graphicsOverlays()->append(m_areasOverlay);
}

void ServiceArea::setupRouting()
{
  connect(m_task, &ServiceAreaTask::createDefaultParametersCompleted, this, [this]
          (QUuid, Esri::ArcGISRuntime::ServiceAreaParameters defaultParameters)
  {
    m_parameters = defaultParameters;
    m_parameters.setOutputSpatialReference(SpatialReference::webMercator());
    m_parameters.setReturnPolygons(true);
    m_parameters.setPolygonDetail(ServiceAreaPolygonDetail::High);
    setBusy(false);
  });

  connect(m_task, &ServiceAreaTask::solveServiceAreaCompleted, this, [this]
          (QUuid, Esri::ArcGISRuntime::ServiceAreaResult serviceAreaResult)
  {
    setBusy(false);
    if (serviceAreaResult.isEmpty())
    {
      m_message = "No Serice Areas calculated!";
      emit messageChanged();
    }

    int numFacilities = m_facilitiesOverlay->graphics()->size();
    for (int i = 0; i < numFacilities; ++i)
    {
      QList<ServiceAreaPolygon> results = serviceAreaResult.resultPolygons(i);
      if (!m_graphicParent)
        m_graphicParent = new QObject(this);
      for (const ServiceAreaPolygon& poly : results)
        m_areasOverlay->graphics()->append(new Graphic(poly.geometry(), m_graphicParent));
    }
  });

  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    if (busy())
      return;

    switch (m_mode) {
    case SampleMode::Barrier:
    case SampleMode::Facility:
      break;
    default:
      return;
    }

    setBusy(true);

    Point mapPoint = m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y());
    Point projectedPoint(mapPoint.x(), mapPoint.y(), SpatialReference::webMercator());

    if (m_mode == SampleMode::Barrier)
      handleBarrierPoint(projectedPoint);
    else if(m_mode == SampleMode::Facility)
      handleFacilityPoint(projectedPoint);

    setBusy(false);
  });

  m_task->createDefaultParameters();
}

void ServiceArea::handleFacilityPoint(const Point &p)
{
  if (!m_graphicParent)
    m_graphicParent = new QObject(this);
  m_facilitiesOverlay->graphics()->append(new Graphic(p, m_graphicParent));
}

void ServiceArea::handleBarrierPoint(const Point &p)
{
  if (!m_graphicParent)
    m_graphicParent = new QObject(this);
  m_barrierBuilder->addPoint(p);
  // update the geometry for the current barrier - or create 1 if it does not exist
  Graphic* barrier = m_barrierOverlay->graphics()->isEmpty() ?
                       nullptr :
                       m_barrierOverlay->graphics()->last();

  if (barrier)
    barrier->setGeometry(m_barrierBuilder->toPolyline());
  else
    m_barrierOverlay->graphics()->append(new Graphic(m_barrierBuilder->toPolyline(), m_graphicParent));
}
