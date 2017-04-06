// [WriteFile Name=DisplayMap, Category=Geometry]
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

#include "FormatCoordinates.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "CoordinateFormatter.h"
#include "SimpleMarkerSymbol.h"
#include "Graphic.h"
#include "CoordinateFormatter.h"
#include "Point.h"

#include <QDebug>

using namespace Esri::ArcGISRuntime;

FormatCoordinates::FormatCoordinates(QQuickItem* parent) :
    QQuickItem(parent)
{
}

FormatCoordinates::~FormatCoordinates()
{
}

void FormatCoordinates::componentComplete()
{
    QQuickItem::componentComplete();

    // find QML MapView component
    m_mapView = findChild<MapQuickView*>("mapView");

    // create a new basemap instance
    Basemap* basemap = Basemap::imagery(this);

    // create a new map instance
    m_map = new Map(basemap, this);


    // create a graphic and place in a graphic overlay
    Point geometry(m_startLongitude, m_startLatitude, m_map->spatialReference());
    SimpleMarkerSymbol* symbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::X, QColor(Qt::red), 15.0, this);
    Graphic* graphic = new Graphic(geometry, symbol, this);

    // create a new graphics overlay instance
    // and add it to the map view
    GraphicsOverlay* go = new GraphicsOverlay(this);
    go->graphics()->append(graphic);
    m_mapView->graphicsOverlays()->append(go);

    // set map on the map view
    m_mapView->setMap(m_map);
}

void FormatCoordinates::setGraphicFromText(QString textType, QString text)
{
  Point point = createPointFromText(textType, text);
  if (!point.isEmpty()) {
    m_mapView->graphicsOverlays()->at(0)->graphics()->at(0)->setGeometry(point);
  }
}

Point FormatCoordinates::createPointFromText(QString textType, QString text)
{
  if ("Decimal Degrees" == textType
   || "Degrees Minutes Seconds" == textType) {
     return CoordinateFormatter::fromLatitudeLongitude(text, m_map->spatialReference());
  }
    else if ("UTM" == textType) {
     return CoordinateFormatter::fromUtm(text, m_map->spatialReference(), UtmConversionMode::LatitudeBandIndicators);

  }
  else if ("USNG"== textType) {
    return CoordinateFormatter::fromUsng(text, m_map->spatialReference());
  }
  else {
    return Point();
  }
}

void FormatCoordinates::setTextFromGraphic(Esri::ArcGISRuntime::Point point)
{
// Use QQuickItem *QQuickView::rootObject() const  ???

// How would this work with the ListView?

}
