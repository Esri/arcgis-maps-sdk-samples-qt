// [WriteFile Name=FindAddress, Category=Search]
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

#include "FindAddress.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "GraphicsOverlay.h"
#include "PictureMarkerSymbol.h"
#include "SimpleRenderer.h"
#include "LocatorTask.h"
#include "GeocodeParameters.h"
#include "Graphic.h"
#include <QUrl>

using namespace Esri::ArcGISRuntime;

FindAddress::FindAddress(QQuickItem* parent) :
  QQuickItem(parent)
{
}

FindAddress::~FindAddress()
{
}

void FindAddress::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<FindAddress>("Esri.Samples", 1, 0, "FindAddressSample");
}

void FindAddress::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // create a new basemap instance
  Basemap* basemap = Basemap::imageryWithLabels(this);
  // create a new map instance
  m_map = new Map(basemap, this);
  // set map on the map view
  m_mapView->setMap(m_map);
  // create graphics overlay and add to map view
  m_graphicsOverlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  // set a renderer on the graphics overlay
  SimpleRenderer* simpleRenderer = new SimpleRenderer(this);
  PictureMarkerSymbol* pictureMarkerSymbol = new PictureMarkerSymbol(QUrl("qrc:/Samples/Search/FindAddress/pin_circle_red.png"), this);
  pictureMarkerSymbol->setWidth(35);
  pictureMarkerSymbol->setHeight(35);
  pictureMarkerSymbol->setOffsetY(pictureMarkerSymbol->height() / 2);
  simpleRenderer->setSymbol(pictureMarkerSymbol);
  m_graphicsOverlay->setRenderer(simpleRenderer);
  m_graphic = new Graphic(this);
  m_graphicsOverlay->graphics()->append(m_graphic);

  // create locator task and parameters
  //! [FindAddress create LocatorTask]
  m_locatorTask = new LocatorTask(QUrl("http://geocode.arcgis.com/arcgis/rest/services/World/GeocodeServer"), this);
  //! [FindAddress create LocatorTask]
  m_geocodeParameters.setMinScore(75);
  m_geocodeParameters.setResultAttributeNames(QStringList() << "Place_addr" << "Match_addr");

  connectSignals();
}

void FindAddress::connectSignals()
{
  // connect to geocode complete signal on the LocatorTask
  //! [FindAddress geocodeCompleted handler]
  connect(m_locatorTask, &LocatorTask::geocodeCompleted, this, [this](QUuid, QList<GeocodeResult> geocodeResults)
  {
    if (geocodeResults.length() > 0)
    {
      m_graphic->setGeometry(geocodeResults.at(0).displayLocation());
      m_graphic->attributes()->setAttributesMap(geocodeResults.at(0).attributes());
      constexpr double scale = 8000.0;
      m_mapView->setViewpointCenter(geocodeResults.at(0).extent().center(), scale);
    }
  });
  //! [FindAddress geocodeCompleted handler]

  // connect to the viewpoint changed signal on the MapQuickView
  connect(m_mapView, &MapQuickView::viewpointChanged, this, [this]()
  {
    emit hideCallout();
  });

  // connect to the mouse click signal on the MapQuickView
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    // set the properties for qml
    m_screenX = mouseEvent.x() - 110;
    m_screenY = mouseEvent.y() - 60;
    emit hideCallout();

    // call identify on the map view
    m_mapView->identifyGraphicsOverlay(m_graphicsOverlay, mouseEvent.x(), mouseEvent.y(), 5, false, 1);
  });

  // connect to the identifyGraphicsOverlayCompleted signal on the map view
  connect(m_mapView, &MapQuickView::identifyGraphicsOverlayCompleted, this, [this](QUuid, IdentifyGraphicsOverlayResult* identifyResult)
  {
    if (!identifyResult)
      return;

    auto graphics = identifyResult->graphics();
    if (graphics.length() > 0)
    {
      m_calloutText = graphics.at(0)->attributes()->attributeValue("Match_addr").toString();
      m_calloutDetailedText = graphics.at(0)->attributes()->attributeValue("Place_addr").toString();
      emit showCallout(m_screenX, m_screenY, m_calloutText, m_calloutDetailedText);
    }

    identifyResult->deleteLater();
  });
}

void FindAddress::geocodeAddress(const QString& address)
{
  //! [FindAddress geocodeWithParameters]
  m_locatorTask->geocodeWithParameters(address, m_geocodeParameters);
  //! [FindAddress geocodeWithParameters]
}

void FindAddress::clearGraphics()
{  
  m_graphic->setGeometry(Point());
}
