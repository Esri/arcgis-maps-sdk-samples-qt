// [WriteFile Name=Picture_Marker_Symbol, Category=DisplayInformation]
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

#include "Picture_Marker_Symbol.h"

#include "Map.h"
#include "Point.h"
#include "Graphic.h"
#include "Basemap.h"
#include "Viewpoint.h"
#include "MapQuickView.h"
#include "GraphicsOverlay.h"
#include "SpatialReference.h"
#include "PictureMarkerSymbol.h"

#include <QUrl>
#include <QString>
#include <QQmlProperty>

using namespace Esri::ArcGISRuntime;

Picture_Marker_Symbol::Picture_Marker_Symbol(QQuickItem* parent) :
    QQuickItem(parent),
    m_map(nullptr),
    m_mapView(nullptr),
    m_graphicsOverlay(nullptr)
{
}

Picture_Marker_Symbol::~Picture_Marker_Symbol()
{
}

void Picture_Marker_Symbol::componentComplete()
{
    QQuickItem::componentComplete();

    // find QML MapView component
    m_mapView = findChild<MapQuickView*>("mapView");
    m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

    m_dataPath = QQmlProperty::read(this, "dataPath").toString();

    // Create a map using the topographic basemap
    m_map = new Map(Basemap::topographic(this), this);

    // Set map to map view
    m_mapView->setMap(m_map);

    // set initial viewpoint
    m_mapView->setViewpointGeometry(Envelope(-228835, 6550763, -223560, 6552021, SpatialReference::webMercator()), 200);

    // create graphics overlay
    m_graphicsOverlay = new GraphicsOverlay(this);

    // create a campsite symbol from a URL
    PictureMarkerSymbol* campSymbol = new PictureMarkerSymbol(QUrl("http://sampleserver6.arcgisonline.com/arcgis/rest/services/Recreation/FeatureServer/0/images/e82f744ebb069bb35b234b3fea46deae"), this);
    addGraphic(Point(-228835, 6550763, SpatialReference::webMercator()), campSymbol);

    // create a blue symbol from a local resource
    PictureMarkerSymbol* blueSymbol = new PictureMarkerSymbol(QUrl("qrc:/Samples/Display Information/Picture_Marker_Symbol/blue_symbol.png"), this);
    addGraphic(Point(-223560, 6552021, SpatialReference::webMercator()), blueSymbol);

    // create an orange symbol from a file path
    PictureMarkerSymbol* orangeSymbol = new PictureMarkerSymbol(QUrl(m_dataPath + "/symbol/orange_symbol.png"));
    addGraphic(Point(-226773, 6550477, SpatialReference::webMercator()), orangeSymbol);

    // add GraphicsOverlay to MapView
    m_mapView->graphicsOverlays()->append(m_graphicsOverlay);
}

void Picture_Marker_Symbol::addGraphic(Point &point, PictureMarkerSymbol* symbol)
{
    // create graphic
    Graphic* graphic = new Graphic(point, symbol);
    // append to graphicsoverlay
    m_graphicsOverlay->graphics()->append(graphic);
}
