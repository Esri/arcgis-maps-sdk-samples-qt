// [WriteFile Name=OfflineRouting, Category=Analysis]
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

#include "OfflineRouting.h"

#include "ArcGISTiledLayer.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "Map.h"
#include "MapQuickView.h"
#include "Polyline.h"
#include "RouteParameters.h"
#include "RouteResult.h"
#include "RouteTask.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleRenderer.h"
#include "Stop.h"
#include "TextSymbol.h"
#include "TileCache.h"

#include <QDir>
#include <QHoverEvent>

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
QString defaultDataPath()
{
  QString dataPath;

#ifdef Q_OS_ANDROID
  dataPath = "/sdcard";
#elif defined Q_OS_IOS
  dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
  dataPath = QDir::homePath();
#endif

  return dataPath;
}
} // namespace

OfflineRouting::OfflineRouting(QObject* parent /* = nullptr */):
  QObject(parent)
{
  const QString fileLocation = QString("%1/ArcGIS/Runtime/Data/tpk/san_diego/streetmap_SD.tpk").arg(defaultDataPath());
  TileCache* tileCache = new TileCache(fileLocation, this);
  ArcGISTiledLayer* tiledLayer = new ArcGISTiledLayer(tileCache, this);
  Basemap* basemap = new Basemap(tiledLayer, this);
  m_map = new Map(basemap, this);
  m_map->setMinScale(100000);

  m_stopsOverlay = new GraphicsOverlay(this);
  m_routeOverlay = new GraphicsOverlay(this);

  SimpleLineSymbol* lineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::blue, 2, this);
  SimpleRenderer* routeRenderer = new SimpleRenderer(lineSymbol, this);
  m_routeOverlay->setRenderer(routeRenderer);
}

OfflineRouting::~OfflineRouting() = default;

void OfflineRouting::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<OfflineRouting>("Esri.Samples", 1, 0, "OfflineRoutingSample");
}

MapQuickView* OfflineRouting::mapView() const
{
  return m_mapView;
}

QStringList OfflineRouting::travelModeNames()
{
  QList<TravelMode> modesList = m_routeTask->routeTaskInfo().travelModes();
  QStringList strList;
  for (int i = 0; i < modesList.size(); ++i)
  {
    strList << modesList[i].name();
  }
  return strList;
}

void OfflineRouting::setTravelModeIndex(int index)
{
  m_travelModeIndex = index;
  emit travelModeIndexChanged();
  //  qDebug() << m_travelModeIndex;
}

int OfflineRouting::travelModeIndex()
{
  return m_travelModeIndex;
}

void OfflineRouting::findRoute()
{
  int numGraphics = m_stopsOverlay->graphics()->size();
  if (numGraphics > 1)
  {
    QList<Stop> stops;
    for (int i = 0; i < numGraphics; ++i)
    {
      stops.append(Stop(m_stopsOverlay->graphics()->at(i)->geometry()));
    }

    // configure stops and travel mode
    m_routeParameters.setStops(stops);
    m_routeParameters.setTravelMode(m_routeTask->routeTaskInfo().travelModes().at(m_travelModeIndex));

    // solve the route
    m_routeTask->solveRoute(m_routeParameters); // emits solveRouteCompleted

    connect(m_routeTask, &RouteTask::solveRouteCompleted, this, [this](QUuid, const RouteResult routeResult){
      // clear old route
      m_routeOverlay->graphics()->clear();
      Polyline routeGeometry = routeResult.routes().first().routeGeometry();
      Graphic* routeGraphic = new Graphic(routeGeometry, this);

      m_routeOverlay->graphics()->append(routeGraphic);
    });

  }
}

// Set the view (created in QML)
void OfflineRouting::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setAcceptHoverEvents(true);
  m_mapView->setMap(m_map);

  m_mapView->graphicsOverlays()->append(m_stopsOverlay);
  m_mapView->graphicsOverlays()->append(m_routeOverlay);

  const QString geodatabaseLocation = QString("%1/ArcGIS/Runtime/Data/tpk/san_diego/sandiego.geodatabase").arg(defaultDataPath());
  m_routeTask = new RouteTask(geodatabaseLocation, "Streets_ND",this);
  m_routeTask->load();

  connect(m_routeTask, &RouteTask::doneLoading, this, [this](Error loadError){
    if (loadError.isEmpty())
    {
      m_routeTask->createDefaultParameters();
      emit travelModeNamesChanged();
    }
    else
    {
      qDebug() << loadError.message() << loadError.additionalMessage();
      return;
    }
  });

  connect(m_routeTask, &RouteTask::createDefaultParametersCompleted, this, [this](QUuid, const RouteParameters& defaultParameters){
    m_routeParameters = defaultParameters;
  });

  connect(m_mapView, &MapQuickView::identifyGraphicsOverlayCompleted, this,
          [this](QUuid, IdentifyGraphicsOverlayResult* result)
  {
    if (!result->error().isEmpty())
      qDebug() << result->error().message() << result->error().additionalMessage();

    // if didn't click on an existing stop, then add a stop
    if (result->graphics().isEmpty())
    {
      SimpleMarkerSymbol* stopLabel = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, Qt::red, 20, this);
      //TextSymbol* stopLabel = new TextSymbol(QString::number(m_stopsOverlay->graphics()->size() + 1), Qt::red, 30, HorizontalAlignment::Right, VerticalAlignment::Top);
      //stopLabel->setBackgroundColor(Qt::black);
      Graphic* stopGraphic = new Graphic(m_clickedPoint, stopLabel, this);
      m_stopsOverlay->graphics()->append(stopGraphic);
      m_selectedStop = false;
    }

    // otherwise move existing stop
    else
    {
      m_selectedStop = true;
      qDebug() << "reclicked";
    }
    findRoute();
    delete result;
  });


  // get stops from clicked locations
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& e){
    m_clickedPoint = m_mapView->screenToLocation(e.x(), e.y());
    m_mapView->identifyGraphicsOverlay(m_stopsOverlay, e.x(), e.y(), 10, false);
    e.accept();
  });

  // update the route if a stop has been selected, and move the stop
  connect(m_mapView, &MapQuickView::mouseMoved, this, [this](QMouseEvent& e){
    qDebug() << e.x();
  });

  emit mapViewChanged();
}
