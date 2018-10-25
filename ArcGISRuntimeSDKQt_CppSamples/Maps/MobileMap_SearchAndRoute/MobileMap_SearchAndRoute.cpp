// [WriteFile Name=MobileMap_SearchAndRoute, Category=Maps]
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

#include "MobileMap_SearchAndRoute.h"

#include "Map.h"
#include "Stop.h"
#include "Graphic.h"
#include "RouteTask.h"
#include "TextSymbol.h"
#include "LocatorTask.h"
#include "RouteResult.h"
#include "MapQuickView.h"
#include "GeocodeResult.h"
#include "SimpleRenderer.h"
#include "GraphicsOverlay.h"
#include "RouteParameters.h"
#include "SimpleLineSymbol.h"
#include "MobileMapPackage.h"
#include "PictureMarkerSymbol.h"
#include "ReverseGeocodeParameters.h"

#include <QDir>
#include <QQmlProperty>
#include <QFileInfoList>
#include <QFile>

using namespace Esri::ArcGISRuntime;

MobileMap_SearchAndRoute::MobileMap_SearchAndRoute(QQuickItem* parent):
    QQuickItem(parent),
    m_selectedMmpkIndex(0),
    m_canRoute(false),
    m_canClear(false),
    m_isGeocodeInProgress(false),
    m_map(nullptr),
    m_mapView(nullptr),
    m_mobileMap(nullptr),
    m_currentLocatorTask(nullptr),
    m_bluePinSymbol(nullptr),
    m_currentRouteTask(nullptr),
    m_stopsGraphicsOverlay(nullptr),
    m_routeGraphicsOverlay(nullptr),
    m_calloutData(nullptr)
{
}

MobileMap_SearchAndRoute::~MobileMap_SearchAndRoute() = default;

void MobileMap_SearchAndRoute::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<MobileMap_SearchAndRoute>("Esri.Samples", 1, 0, "MobileMap_SearchAndRouteSample");
  qmlRegisterUncreatableType<CalloutData>("Esri.Samples", 1, 0, "CalloutData", "CalloutData is an uncreatable type");
}

void MobileMap_SearchAndRoute::componentComplete()
{
    QQuickItem::componentComplete();

    // find QML MapView component
    m_mapView = findChild<MapQuickView*>("mapView");
    m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

    m_dataPath = QQmlProperty::read(this, "dataPath").toString();
    m_fileInfoList = QDir(m_dataPath).entryInfoList();

    // initialize Callout
    m_mapView->calloutData()->setTitle("Address");
    m_calloutData = m_mapView->calloutData();
    emit calloutDataChanged();

    // set reverse geocoding parameters
    m_reverseGeocodeParameters.setMaxResults(1);

    // identify and create MobileMapPackages using mmpk files in datapath
    createMobileMapPackages(0);

    // create graphics overlays to visually display geocoding and routing results
    m_stopsGraphicsOverlay = new GraphicsOverlay(this);
    m_routeGraphicsOverlay = new GraphicsOverlay(this);
    m_routeGraphicsOverlay->setRenderer(new SimpleRenderer(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("#2196F3"), 4, this), this));

    m_mapView->graphicsOverlays()->append(m_routeGraphicsOverlay);
    m_mapView->graphicsOverlays()->append(m_stopsGraphicsOverlay);

    // create a pin symbol
    m_bluePinSymbol = new PictureMarkerSymbol(QUrl("qrc:/Samples/Maps/MobileMap_SearchAndRoute/bluePinSymbol.png"), this);
    m_bluePinSymbol->setHeight(36);
    m_bluePinSymbol->setWidth(36);
    m_bluePinSymbol->setOffsetY(m_bluePinSymbol->height() / 2);

    connectSignals();
}

void MobileMap_SearchAndRoute::createMobileMapPackages(int index)
{
    if (index < m_fileInfoList.length())
    {
        // check if file is a .mmpk file
        if (m_fileInfoList[index].completeSuffix() == "mmpk")
        {
            // create a new MobileMapPackage
            MobileMapPackage* mobileMapPackage = new MobileMapPackage(m_fileInfoList[index].absoluteFilePath(), this);

            // once MMPK is finished loading, add it and its information to lists
            connect(mobileMapPackage, &MobileMapPackage::doneLoading, this, [mobileMapPackage, this](Error error)
            {
                if (error.isEmpty())
                {
                    // QList of MobileMapPackages
                    m_mobileMapPackages.append(mobileMapPackage);

                    // QStringList of MobileMapPackage names. Used as a ListModel in QML
                    m_mobileMapPackageList << mobileMapPackage->item()->title();
                    emit mmpkListChanged();
                }
            });

            // load the new MMPK
            mobileMapPackage->load();
        }

        createMobileMapPackages(++index);
    }
    else
        return;
}

void MobileMap_SearchAndRoute::connectSignals()
{
    connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
    {
        if (m_currentLocatorTask)
        {
            m_clickedPoint = Point(m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y()));

            // determine if user clicked on a graphic
            m_mapView->identifyGraphicsOverlay(m_stopsGraphicsOverlay, mouseEvent.x(), mouseEvent.y(), 5, false, 2);
        }
    });

    connect(m_mapView, &MapQuickView::identifyGraphicsOverlayCompleted, this, [this](QUuid, IdentifyGraphicsOverlayResult* identifyResult)
    {
        if (!identifyResult)
          return;

        // get graphics list
        QList<Graphic*> graphics = identifyResult->graphics();
        if (graphics.count() > 0)
        {
            // use the blue pin graphic instead of text graphic to as calloutData's geoElement
            Graphic* graphic = graphics[0];
            if (graphic->symbol()->symbolType() != SymbolType::PictureMarkerSymbol && graphics.count() > 1)
                graphic = graphics[1];

            m_mapView->calloutData()->setGeoElement(graphic);
            m_mapView->calloutData()->setDetail(graphic->attributes()->attributeValue("AddressLabel").toString());
            m_mapView->calloutData()->setVisible(true);
        }

        // if clicked a point with no graphic on it, reverse geocode
        else
        {
            m_currentLocatorTask->reverseGeocodeWithParameters(m_clickedPoint, m_reverseGeocodeParameters);
            m_isGeocodeInProgress = true;
            emit isGeocodeInProgressChanged();
        }
    });
}

void MobileMap_SearchAndRoute::resetMapView()
{
    // dismiss mapView controls
    m_canClear = false;
    emit canClearChanged();
    m_canRoute = false;
    emit canRouteChanged();

    // dimiss callout
    m_calloutData->setVisible(false);

    // clear the graphics overlays and stops
    m_stopsGraphicsOverlay->graphics()->clear();
    if (m_stopGraphicParent)
    {
      delete m_stopGraphicParent;
      m_stopGraphicParent = nullptr;
    }
    m_routeGraphicsOverlay->graphics()->clear();
    if (m_routeGraphicParent)
    {
      delete m_routeGraphicParent;
      m_routeGraphicParent = nullptr;
    }
    m_stops.clear();
}

void MobileMap_SearchAndRoute::createMapList(int index)
{
    m_mapList.clear();
    m_selectedMmpkIndex = index;

    int counter = 1;

    for (const Map* map : m_mobileMapPackages[index]->maps())
    {
        QVariantMap mapList;
        mapList["name"] = map->item()->title() + " " + QString::number(counter);
        mapList["geocoding"] = m_mobileMapPackages[index]->locatorTask() != nullptr;
        mapList["routing"] = map->transportationNetworks().count() > 0;

        m_mapList << mapList;
        ++counter;
    }

    emit mapListChanged();
}

void MobileMap_SearchAndRoute::selectMap(int index)
{
    resetMapView();

    // set the locatorTask
    //! [MobileMap_SearchAndRoute create LocatorTask]
    m_currentLocatorTask = m_mobileMapPackages[m_selectedMmpkIndex]->locatorTask();
    //! [MobileMap_SearchAndRoute create LocatorTask]

    // set the MapView
    m_mapView->setMap(m_mobileMapPackages[m_selectedMmpkIndex]->maps().at(index));

    if (m_currentLocatorTask)
    {
        // prevent connecting same signal to multiple slots
        m_currentLocatorTask->disconnect();

        connect(m_currentLocatorTask, &LocatorTask::geocodeCompleted, this, [this](QUuid, const QList<GeocodeResult>& geocodeResults)
        {
            // make busy indicator invisible
            m_isGeocodeInProgress = false;
            emit isGeocodeInProgressChanged();

            if (geocodeResults.count() > 0)
            {
                // create parent for the graphic
                if (!m_stopGraphicParent)
                  m_stopGraphicParent = new QObject(this);

                // create a blue pin graphic to display location
                Graphic* bluePinGraphic = new Graphic(geocodeResults[0].displayLocation(), m_bluePinSymbol, m_stopGraphicParent);
                bluePinGraphic->attributes()->insertAttribute("AddressLabel", geocodeResults[0].label());

                m_stopsGraphicsOverlay->graphics()->append(bluePinGraphic);

                // make clear graphics overlay button visible
                m_canClear = true;
                emit canClearChanged();

                // if routing is enabled in map, set up the Stops
                if (m_currentRouteTask)
                {
                    // create a stop based on added graphic
                    m_stops << Stop(bluePinGraphic->geometry());

                    // create a Text Symbol to display stop number
                    TextSymbol* textSymbol = new TextSymbol(m_stopGraphicParent);
                    textSymbol->setText(QString::number(m_stops.count()));
                    textSymbol->setColor(QColor("white"));
                    textSymbol->setSize(18);
                    textSymbol->setOffsetY(m_bluePinSymbol->height() / 2);

                    // create a Graphic using the textSymbol
                    Graphic* stopNumberGraphic = new Graphic(bluePinGraphic->geometry(), textSymbol, m_stopGraphicParent);
                    stopNumberGraphic->setZIndex(bluePinGraphic->zIndex() + 1);
                    m_stopsGraphicsOverlay->graphics()->append(stopNumberGraphic);

                    if (m_stops.count() > 1)
                    {
                        m_canRoute = true;
                        emit canRouteChanged();
                    }
                }
            }
        });
    }

    // create a RouteTask with selected map's transportation network if available
    if (m_mobileMapPackages[m_selectedMmpkIndex]->maps().at(index)->transportationNetworks().count() > 0)
    {
        m_currentRouteTask = new RouteTask(m_mobileMapPackages[m_selectedMmpkIndex]->maps().at(index)->transportationNetworks().at(0), this);
        m_currentRouteTask->load();

        // create default parameters after the RouteTask is loaded
        connect(m_currentRouteTask, &RouteTask::loadStatusChanged, this, [this](LoadStatus loadStatus)
        {
            if (loadStatus == LoadStatus::Loaded)
                m_currentRouteTask->createDefaultParameters();
        });

        // store the created route parameters
        connect(m_currentRouteTask, &RouteTask::createDefaultParametersCompleted, this, [this](QUuid, RouteParameters routeParameters)
        {
            m_currentRouteParameters = routeParameters;
        });

        // create a graphic using the routeResult
        connect(m_currentRouteTask, &RouteTask::solveRouteCompleted, this, [this](QUuid, RouteResult routeResult)
        {
            if (!m_routeGraphicParent)
              m_routeGraphicParent = new QObject(this);

            if (!routeResult.isEmpty())
            {
              Graphic* routeGraphic = new Graphic(routeResult.routes()[0].routeGeometry(), m_routeGraphicParent);
              m_routeGraphicsOverlay->graphics()->append(routeGraphic);
            }
        });
    }
    else
        m_currentRouteTask = nullptr;
}

void MobileMap_SearchAndRoute::solveRoute()
{
    // clear previously displayed routes
    m_routeGraphicsOverlay->graphics()->clear();
    if (m_routeGraphicParent)
    {
      delete m_routeGraphicParent;
      m_routeGraphicParent = nullptr;
    }

    // set stops and solve route
    m_currentRouteParameters.setStops(m_stops);
    m_currentRouteTask->solveRoute(m_currentRouteParameters);
}

QStringList MobileMap_SearchAndRoute::mmpkList() const
{
    return m_mobileMapPackageList;
}

QVariantList MobileMap_SearchAndRoute::mapList() const
{
    return m_mapList;
}

CalloutData *MobileMap_SearchAndRoute::calloutData() const
{
    return m_calloutData;
}

bool MobileMap_SearchAndRoute::isGeocodeInProgress() const
{
    return m_isGeocodeInProgress;
}

bool MobileMap_SearchAndRoute::canRoute() const
{
    return m_canRoute;
}

bool MobileMap_SearchAndRoute::canClear() const
{
    return m_canClear;
}
