// [WriteFile Name=FormatCoordinates, Category=Geometry]
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

#include "FormatCoordinates.h"

#include "Basemap.h"
#include "CoordinateFormatter.h"
#include "Graphic.h"
#include "Map.h"
#include "MapQuickView.h"
#include "Point.h"
#include "SimpleMarkerSymbol.h"

using namespace Esri::ArcGISRuntime;

namespace
{
  // Initial point marker 'X' symbol appears.
  const Point startPoint(-117.195723, 34.056195, SpatialReference::wgs84());
}

FormatCoordinates::FormatCoordinates(QObject* parent) :
  QObject(parent),
  m_map(new Map(Basemap::imagery(this), this)),
  m_graphicsOverlay(new GraphicsOverlay(this))
{
  // create a graphic
  SimpleMarkerSymbol* symbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::X, QColor(Qt::red), 15.0, this);

  Graphic* graphic = new Graphic(startPoint, symbol, this);

  // and add insert the graphic
  m_graphicsOverlay->graphics()->append(graphic);
}

FormatCoordinates::~FormatCoordinates() = default;

void FormatCoordinates::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<FormatCoordinates>("Esri.Samples", 1, 0, "FormatCoordinatesSample");
}

// handle case where the user changed one of the text fields
void FormatCoordinates::handleTextUpdate(QString textType, QString text)
{
  Point point = createPointFromText(textType, text);
  handleLocationUpdate(std::move(point));
}

// handle case where user clicked on the map
void FormatCoordinates::handleLocationUpdate(Point point)
{
  if (!point.isEmpty())
  {
    m_mapView->graphicsOverlays()->at(0)->graphics()->at(0)->setGeometry(point);
    setTextFromPoint(point);
  }
}

Point FormatCoordinates::createPointFromText(const QString& textType, const QString& text) const
{
  //! [FormatCoordinates CoordinateFormatter various text to point]
  if (strDecimalDegrees() == textType
      || strDegreesMinutesSeconds() == textType) {
    return CoordinateFormatter::fromLatitudeLongitude(text, m_map->spatialReference());
  }
  if (strUsng() == textType) {
    return CoordinateFormatter::fromUsng(text, m_map->spatialReference());
  }
  if (strUtm() == textType) {
    return CoordinateFormatter::fromUtm(text, m_map->spatialReference(), UtmConversionMode::LatitudeBandIndicators);
  }
  return Point();
  //! [FormatCoordinates CoordinateFormatter various text to point]
}

void FormatCoordinates::setTextFromPoint(Point point)
{
  // last parm = decimal places
  m_coordinatesInDD = CoordinateFormatter::toLatitudeLongitude(point, LatitudeLongitudeFormat::DecimalDegrees, 6);

  // last parm = decimal places
  m_coordinatesInDMS = CoordinateFormatter::toLatitudeLongitude(point, LatitudeLongitudeFormat::DegreesMinutesSeconds, 1);

  //! [FormatCoordinates CoordinateFormatter point to USNG]
  int decimalPlaces = 5;
  bool addSpaces = true;
  m_coordinatesInUsng = CoordinateFormatter::toUsng(point, decimalPlaces, addSpaces);
  //! [FormatCoordinates CoordinateFormatter point to USNG]

  // last parm = add spaces
  m_coordinatesInUtm = CoordinateFormatter::toUtm(point, UtmConversionMode::LatitudeBandIndicators, true);

  emit coordinatesChanged();
}

QString FormatCoordinates::coordinatesInDD() const
{
  return m_coordinatesInDD;
}

QString FormatCoordinates::coordinatesInDMS() const
{
  return m_coordinatesInDMS;
}

QString FormatCoordinates::coordinatesInUsng() const
{
  return m_coordinatesInUsng;
}

QString FormatCoordinates::coordinatesInUtm() const
{
  return m_coordinatesInUtm;
}

QString FormatCoordinates::strDecimalDegrees() const
{
  return tr("Degrees");
}

QString FormatCoordinates::strDegreesMinutesSeconds() const
{
  return tr("DMS");
}

QString FormatCoordinates::strUsng() const
{
  return tr("Usng");
}

QString FormatCoordinates::strUtm() const
{
  return tr("Utm");
}

void FormatCoordinates::setMapView(MapQuickView* mapView)
{
  if (m_mapView)
  {
    m_mapView->setMap(nullptr); // Remove map from old mapView.
    m_mapView->graphicsOverlays()->clear(); // Remove Graphics overlays from old mapView.
  }

  m_mapView = mapView;

  if (!m_mapView)
  {
    return;
  }

  // set map and graphics overlay on the map view
  m_mapView->setMap(m_map);
  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  // connect to the mouse clicked signal on the MapQuickView
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    // get the point from the mouse point
    Point mapPoint = m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y());

    // using the point, refresh the graphic and the text
    handleLocationUpdate(std::move(mapPoint));
  },
  Qt::UniqueConnection);

  handleLocationUpdate(startPoint);
}
