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

#include "Basemap.h"
#include "CoordinateFormatter.h"
#include "Graphic.h"
#include "Map.h"
#include "MapQuickView.h"
#include "Point.h"
#include "SimpleMarkerSymbol.h"

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

    // create a graphic
    Esri::ArcGISRuntime::Point geometry(m_startLongitude, m_startLatitude, m_map->spatialReference());
    SimpleMarkerSymbol* symbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::X, QColor(Qt::red), 15.0, this);
    Graphic* graphic = new Graphic(geometry, symbol, this);

    // create a new graphics overlay instance
    // and add insert the graphic
    GraphicsOverlay* go = new GraphicsOverlay(this);
    go->graphics()->append(graphic);

    // set map and graphics overlay on the map view
    m_mapView->setMap(m_map);
    m_mapView->graphicsOverlays()->append(go);

    connectSignals();

    // Set initial position to Building Q
    handleTextUpdate("Decimal Degrees", "34.056195N 117.195723W");
}

void FormatCoordinates::connectSignals()
{
  // connect to the mouse clicked signal on the MapQuickView
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    // get the point from the mouse point
    Esri::ArcGISRuntime::Point mapPoint = m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y());

    // using the point, refresh the graphic and the text
    handleLocationUpdate(mapPoint);
  });
}

// handle case where the user changed one of the text fields
void FormatCoordinates::handleTextUpdate(QString textType, QString text)
{
  Esri::ArcGISRuntime::Point point = createPointFromText(textType, text);
  handleLocationUpdate(point);
}

// handle case where user clicked on the map
void FormatCoordinates::handleLocationUpdate(Esri::ArcGISRuntime::Point point)
{
  if (!point.isEmpty())
  {
    m_mapView->graphicsOverlays()->at(0)->graphics()->at(0)->setGeometry(point);
    setTextFromPoint(point);
  }
}

Esri::ArcGISRuntime::Point FormatCoordinates::createPointFromText(QString textType, QString text)
{
  if ("Decimal Degrees" == textType
   || "Degrees Minutes Seconds" == textType) {
     return CoordinateFormatter::fromLatitudeLongitude(text, m_map->spatialReference());
  }
  else if ("USNG"== textType) {
    return CoordinateFormatter::fromUsng(text, m_map->spatialReference());
  }
  else if ("UTM" == textType) {
   return CoordinateFormatter::fromUtm(text, m_map->spatialReference(), UtmConversionMode::LatitudeBandIndicators);
}
  else {
    return Esri::ArcGISRuntime::Point();
  }
}

void FormatCoordinates::setTextFromPoint(Esri::ArcGISRuntime::Point point)
{
  m_coordinatesInDD = CoordinateFormatter::toLatitudeLongitude(point, LatitudeLongitudeFormat::DecimalDegrees, 6); // last parm = decimal places
  emit coordinatesInDDChanged();

  m_coordinatesInDMS = CoordinateFormatter::toLatitudeLongitude(point, LatitudeLongitudeFormat::DegreesMinutesSeconds, 1); // last parm = decimal places
  emit coordinatesInDMSChanged();

  m_coordinatesInUsng = CoordinateFormatter::toUsng(point, 5, true);
  emit coordinatesInUsngChanged();

  m_coordinatesInUtm = CoordinateFormatter::toUtm(point, UtmConversionMode::LatitudeBandIndicators, true); // last parm = add spaces
  emit coordinatesInUtmChanged();
}

QString FormatCoordinates::coordinatesInDD()
{
  return m_coordinatesInDD;
}

QString FormatCoordinates::coordinatesInDMS()
{
  return m_coordinatesInDMS;
}

QString FormatCoordinates::coordinatesInUsng()
{
  return m_coordinatesInUsng;
}

QString FormatCoordinates::coordinatesInUtm()
{
  return m_coordinatesInUtm;
}
