// [WriteFile Name=ShowCallout, Category=DisplayInformation]
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "ShowCallout.h"

#include "Map.h"
#include "MapQuickView.h"
#include "CalloutData.h"

using namespace Esri::ArcGISRuntime;

ShowCallout::ShowCallout(QQuickItem* parent):
  QQuickItem(parent)
{
}

ShowCallout::~ShowCallout() = default;

void ShowCallout::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ShowCallout>("Esri.Samples", 1, 0, "ShowCalloutSample");
  qmlRegisterUncreatableType<CalloutData>("Esri.Samples", 1, 0, "CalloutData", "CalloutData is an uncreatable type");
}

void ShowCallout::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create a map using the topographic basemap
  m_map = new Map(Basemap::topographic(this), this);
  m_map->setInitialViewpoint(Viewpoint(Point(-1.2e7, 5e6, SpatialReference::webMercator()), 1e7));

  // Set map to map view
  m_mapView->setMap(m_map);

  //! [initialize callout]
  m_mapView->calloutData()->setVisible(false);
  m_mapView->calloutData()->setTitle("Location");
  QImage image(":/Samples/DisplayInformation/ShowCallout/RedShinyPin.png");
  m_mapView->calloutData()->setImage(image);
  //! [initialize callout]

  // display callout on mouseClicked
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent){
    if (m_mapView->calloutData()->isVisible())
      m_mapView->calloutData()->setVisible(false);
    else
    {
      // set callout position
      Point mapPoint(m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y()));
      m_mapView->calloutData()->setLocation(mapPoint);

      // set detail as coordinates formatted to decimal numbers with precision 2
      m_mapView->calloutData()->setDetail("x: " + QString::number(mapPoint.x(), 'f', 2) + " y: " + QString::number(mapPoint.y(), 'f', 2));
      m_mapView->calloutData()->setVisible(true);
    }
  });
}
