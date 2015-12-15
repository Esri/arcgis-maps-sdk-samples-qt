// [WriteFile Name=IdentifyGraphics, Category=DisplayInformation]
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

#include "IdentifyGraphics.h"

#include "Map.h"
#include "MapQuickView.h"
#include "SpatialReference.h"
#include "Basemap.h"
#include "GraphicsOverlay.h"
#include "SimpleFillSymbol.h"
#include "SimpleRenderer.h"
#include "PolygonBuilder.h"
#include "Graphic.h"
#include <QMouseEvent>
#include <QList>
#include <QUuid>

using namespace Esri::ArcGISRuntime;

IdentifyGraphics::IdentifyGraphics(QQuickItem* parent) :
    QQuickItem(parent),
    m_map(nullptr),
    m_mapView(nullptr),
    m_graphicsOverlay(nullptr),
    m_identifiedGraphicsCount(0)
{
}

IdentifyGraphics::~IdentifyGraphics()
{
}

void IdentifyGraphics::componentComplete()
{
    QQuickItem::componentComplete();

    // find QML MapView component
    m_mapView = findChild<MapQuickView*>("mapView");

    // Create a map using the topographic basemap
    m_map = new Map(Basemap::topographic(this), this);
    // set map on the map view
    m_mapView->setMap(m_map);

    // create a new graphics overlay
    m_graphicsOverlay = new GraphicsOverlay(this);

    // assign a renderer to the graphics overlay
    SimpleFillSymbol* simpleFillSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor(255, 255, 0, 180), this);
    SimpleRenderer* simpleRenderer = new SimpleRenderer(simpleFillSymbol, this);
    m_graphicsOverlay->setRenderer(simpleRenderer);

    // add the overlay to the mapview
    m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

    // add polygon graphics
    addPolygonGraphic();

    // connect signals
    connectSignals();
}

// create the polygon geometry and graphic
void IdentifyGraphics::addPolygonGraphic()
{
    // create the polygon using the builder class
    PolygonBuilder polygonBuilder(SpatialReference::webMercator());
    polygonBuilder.addPoint(-20e5, 20e5);
    polygonBuilder.addPoint(20e5, 20e5);
    polygonBuilder.addPoint(20e5, -20e5);
    polygonBuilder.addPoint(-20e5, -20e5);

    // create the graphic and add to the graphics overlay
    Graphic* graphic = new Graphic(polygonBuilder.toGeometry(), this);
    m_graphicsOverlay->graphics()->append(graphic);
}

void IdentifyGraphics::connectSignals()
{
    // connect to the mouse press release signal on the MapQuickView
    connect(m_mapView, &MapQuickView::mouseClick, [this](QMouseEvent& mouseEvent)
    {
        // call identify on the map view
        m_mapView->identifyGraphicsOverlay(m_graphicsOverlay, mouseEvent.x(), mouseEvent.y(), 5, 1);
    });

    // connect to the identifyLayerCompleted signal on the map view
    connect(m_mapView, &MapQuickView::identifyGraphicsOverlayCompleted, [this](QUuid, QList<Graphic*> identifyResults)
    {
        m_identifiedGraphicsCount = identifyResults.size();
        emit identifiedGraphicsCountChanged();
    });
}

int IdentifyGraphics::identifiedGraphicsCount()
{
    return m_identifiedGraphicsCount;
}
