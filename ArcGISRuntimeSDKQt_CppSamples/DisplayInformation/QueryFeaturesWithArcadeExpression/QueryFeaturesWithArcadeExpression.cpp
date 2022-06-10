// [WriteFile Name=QueryFeaturesWithArcadeExpression, Category=DisplayInformation]
// [Legal]
// Copyright 2022 Esri.

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

#include "QueryFeaturesWithArcadeExpression.h"

#include "CalloutData.h"
#include "Map.h"
#include "MapQuickView.h"
#include "Point.h"
#include "PortalItem.h"

#include <QDebug>

using namespace Esri::ArcGISRuntime;

QueryFeaturesWithArcadeExpression::QueryFeaturesWithArcadeExpression(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISTopographic, this))
{
  Portal* portal = new Portal(QUrl("https://www.arcgis.com/"), this);
  PortalItem* portalItem = new PortalItem(portal, "14562fced3474190b52d315bc19127f6", this);

  // Create a map object using the portalItem
  m_map = new Map(portalItem, this);

  //  for (const auto layer : m_map->operationalLayers()) {
  //    qDebug() << layer;
  //  }

  connect(m_map, &Map::doneLoading, this, [this]()
  {
    if (m_map->loadStatus() == LoadStatus::Loaded)
    {
      // Set the visibility of all but the RDT Beats layer to false to avoid cluttering the UI
      for (int i = 0; i < m_map->operationalLayers()->size(); ++i)
      {

        auto current_layer_name = m_map->operationalLayers()->at(i)->name();
        if ((current_layer_name == "Crime in the last 60 days") || (current_layer_name == "Police Stations"))
        {
          m_map->operationalLayers()->at(i)->setVisible(false);
        }
      }
    }
  });

}

QueryFeaturesWithArcadeExpression::~QueryFeaturesWithArcadeExpression() = default;

void QueryFeaturesWithArcadeExpression::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<QueryFeaturesWithArcadeExpression>("Esri.Samples", 1, 0, "QueryFeaturesWithArcadeExpressionSample");
}

MapQuickView* QueryFeaturesWithArcadeExpression::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void QueryFeaturesWithArcadeExpression::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  // identify clicked features
    connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
    {
      m_mapView->identifyLayers(mouseEvent.x(), mouseEvent.y(), 12, false);
      // call showEvaluatedArcadeInCallout with the results from the above call
    });

    m_mapView->calloutData()->setVisible(false);
      m_mapView->calloutData()->setTitle("Location");

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

//    connect(m_mapView, &MapQuickView::identifyLayersCompleted, this, &PerformValveIsolationTrace::onIdentifyLayersCompleted);

  emit mapViewChanged();
}

//void QueryFeaturesWithArcadeExpression::showEvaluatedArcadeInCallout()
//{

//}
