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
#include "Feature.h"
#include "ArcadeEvaluator.h"
#include "ArcadeExpression.h"
#include "ArcadeEvaluationResult.h"
#include "FeatureLayer.h"

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

        if (current_layer_name == "RPD Beats  - City_Beats_Border_1128-4500")
        {
          m_beatsLayer = static_cast<FeatureLayer*>(m_map->operationalLayers()->at(i));
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

  m_mapView->calloutData()->setVisible(false);
  m_mapView->calloutData()->setTitle("Crimes");

  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent){
    if (m_mapView->calloutData()->isVisible())
      m_mapView->calloutData()->setVisible(false);
    else
    {
      // set callout position
      Point mapPoint(m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y()));
      m_mapView->calloutData()->setLocation(mapPoint);

      // Identify the visible layer
      m_mapView->identifyLayers(mouseEvent.x(), mouseEvent.y(), 12, false);

      // once the identify is done
      connect(m_mapView, &MapQuickView::identifyLayerCompleted, this, [this, mapPoint](QUuid, Esri::ArcGISRuntime::IdentifyLayerResult* rawIdentifyResult)
      {
        auto identifyResult = std::unique_ptr<IdentifyLayerResult>(rawIdentifyResult);

        if (!identifyResult)
          return;

        GeoElement* element = identifyResult->geoElements().at(0);
        m_identifiedFeature = static_cast<Feature*>(element);

        showEvaluatedArcadeInCallout(m_identifiedFeature, mapPoint);
      });

      m_mapView->calloutData()->setVisible(true);
    }
  });

  emit mapViewChanged();
}

void QueryFeaturesWithArcadeExpression::showEvaluatedArcadeInCallout(Feature* feature, Point mapPoint)
{
  const QString& expressionValue =
      "// Get a feature set of crimes from the map by referencing a layer name\n"
      "var crimes = FeatureSetByName(${'$'}map, \"Crime in the last 60 days\")\n"

      "// Count the number of crimes that intersect the provided police beat polygon\n"
      "return Count(Intersects(${'$'}feature, crimes))";

  ArcadeExpression expression {expressionValue};
  ArcadeEvaluator* evaluator = new ArcadeEvaluator(&expression, ArcadeProfile::FormCalculation, this);

  // Create a map for key-value pairs
  auto profileVariables = QVariantMap{{"Endangered", "ENDANGERED"}, {"Not endangered", "NOT_ENDANGERED"}, {"N/A", "NA"}};

  connect(evaluator, &ArcadeEvaluator::evaluateCompleted, this, [this](QUuid taskId)
  {
    // TODO: Find a way to get back the result from evaluate here???
    //    ArcadeEvaluationResult* result = evaluator->
  });

  evaluator->evaluate(profileVariables);

  m_mapView->calloutData()->setDetail("Beat: $beat - $section\n $result crimes in the past two months");
}
