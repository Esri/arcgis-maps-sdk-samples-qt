// [WriteFile Name=ReverseGeocodeOnline, Category=Search]
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

#include "ReverseGeocodeOnline.h"

#include "CalloutData.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "LocatorTask.h"
#include "Map.h"
#include "MapQuickView.h"
#include "PictureMarkerSymbol.h"
#include "SimpleRenderer.h"

#include <QUrl>

using namespace Esri::ArcGISRuntime;

ReverseGeocodeOnline::ReverseGeocodeOnline(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::imageryWithLabels(this), this))
{
  const QUrl url("http://geocode.arcgis.com/arcgis/rest/services/World/GeocodeServer");
  LocatorTask* locatorTask = new LocatorTask(url, this);

//  m_mapView->graphicsOverlays()->append(graphicsOverlay);
}

ReverseGeocodeOnline::~ReverseGeocodeOnline() = default;

void ReverseGeocodeOnline::configureGraphic()
{
  m_graphicsOverlay = new GraphicsOverlay(this);

  const QUrl pinUrl("qrc:/Samples/Search/ReverseGeocodeOnline/pin.png");
  PictureMarkerSymbol* pinSymbol = new PictureMarkerSymbol(pinUrl, this);
  SimpleRenderer* simpleRenderer = new SimpleRenderer(pinSymbol, this);

  m_graphicsOverlay->setRenderer(simpleRenderer);
  m_graphicsOverlay->graphics()->append(new Graphic(this));
}

void ReverseGeocodeOnline::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ReverseGeocodeOnline>("Esri.Samples", 1, 0, "ReverseGeocodeOnlineSample");
  qmlRegisterUncreatableType<CalloutData>("Esri.Samples", 1, 0, "CalloutData", "CalloutData is an uncreatable type");
}

MapQuickView* ReverseGeocodeOnline::mapView() const
{
  return m_mapView;
}

CalloutData* ReverseGeocodeOnline::calloutData() const
{
  return m_calloutData;
}

// Set the view (created in QML)
void ReverseGeocodeOnline::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  configureGraphic();
  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& e)
  {
    Point clickedPoint(e.x(),e.y());
    qDebug() << clickedPoint.x();
  });

  emit mapViewChanged();
}
