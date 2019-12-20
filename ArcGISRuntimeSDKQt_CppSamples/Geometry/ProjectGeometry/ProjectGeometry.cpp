// [WriteFile Name=ProjectGeometry, Category=Geometry]
// [Legal]
// Copyright 2018 Esri.

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

#include "ProjectGeometry.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "Viewpoint.h"
#include "Envelope.h"
#include "GeometryEngine.h"
#include "Point.h"
#include "SimpleMarkerSymbol.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "CalloutData.h"

using namespace Esri::ArcGISRuntime;

ProjectGeometry::ProjectGeometry(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void ProjectGeometry::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ProjectGeometry>("Esri.Samples", 1, 0, "ProjectGeometrySample");
  qmlRegisterUncreatableType<CalloutData>("Esri.Samples", 1, 0, "CalloutData", "CalloutData is an uncreatable type");
}

void ProjectGeometry::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // connect to mouse clicked signal
  connect(m_mapView, &MapQuickView::mouseClicked, this, &ProjectGeometry::onMouseClicked);

  // Create a map using the National Geographic basemap
  m_map = new Map(Basemap::nationalGeographic(this), this);

  // Set initial viewpoint to Minneapolis
  Viewpoint initialViewpoint(Envelope(-10995912.335747, 5267868.874421, -9880363.974046, 5960699.183877, SpatialReference::webMercator()));
  m_map->setInitialViewpoint(initialViewpoint);

  // Add a GraphicsOverlay
  GraphicsOverlay* overlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(overlay);

  // Add a Point Graphic
  SimpleMarkerSymbol* sms = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("red"), 5.0f /*size*/, this);
  m_inputGraphic = new Graphic(this);
  m_inputGraphic->setSymbol(sms);
  overlay->graphics()->append(m_inputGraphic);

  // Set map to map view
  m_mapView->setMap(m_map);

  // Set up callout data
  m_calloutData = m_mapView->calloutData();
  m_calloutData->setTitle(QStringLiteral("Coordinates"));
}

void ProjectGeometry::onMouseClicked(QMouseEvent& event)
{
  // get the mouse click as a point
  const Point originalPoint = m_mapView->screenToLocation(event.x(), event.y());

  // show the clicked location on the map with a graphic
  m_inputGraphic->setGeometry(originalPoint);

  // create the output SpatialReference by specifying a well known ID (WKID)
  const SpatialReference spatialReference(4326);

  // project the web mercator point to WGS84
  const Point projectedPoint = GeometryEngine::project(originalPoint, spatialReference);

  // update callout data
  m_calloutData->setLocation(originalPoint);
  const QString ox = QString::number(originalPoint.x(), 'f', 5);
  const QString oy = QString::number(originalPoint.y(), 'f', 5);
  const QString px = QString::number(projectedPoint.x(), 'f', 5);
  const QString py = QString::number(projectedPoint.y(), 'f', 5);
  m_calloutData->setDetail(QString("Original: %1, %2\nProjected: %3, %4").arg(ox, oy, px, py));
  m_calloutData->setVisible(true);
}
