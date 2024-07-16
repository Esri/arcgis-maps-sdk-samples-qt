// [WriteFile Name=QueryFeaturesWithArcadeExpression, Category=DisplayInformation]
// [Legal]
// Copyright 2022 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "QueryFeaturesWithArcadeExpression.h"

// C++ API headers
#include "ArcGISFeature.h"
#include "ArcadeEvaluationResult.h"
#include "ArcadeEvaluator.h"
#include "ArcadeExpression.h"
#include "CalloutData.h"
#include "ExpressionTypes.h"
#include "IdentifyLayerResult.h"
#include "Layer.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "Portal.h"
#include "PortalItem.h"

// Qt headers
#include <QFuture>
#include <QUuid>
#include <QVariantMap>

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
      m_map->operationalLayers()->at(1)->setVisible(false);
      m_map->operationalLayers()->at(2)->setVisible(false);
      m_map->operationalLayers()->at(3)->setVisible(false);
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
  m_mapView->calloutData()->setTitle("RPD Beats");

  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent){
    if (m_mapView->calloutData()->isVisible())
      m_mapView->calloutData()->setVisible(false);
    m_mapView->calloutData()->setDetail("");
    // Set callout position
    const Point mapPoint(m_mapView->screenToLocation(mouseEvent.position().x(), mouseEvent.position().y()));
    m_mapView->calloutData()->setLocation(mapPoint);

    m_mapView->identifyLayersAsync(mouseEvent.position(), 12, false).then(this,
    [this](const QList<IdentifyLayerResult*>& results)
    {
      if (results.empty())
        return;

      QList<GeoElement*> element_list = results.first()->geoElements();
      if (element_list.empty())
        return;

      GeoElement* element = element_list.at(0);
      ArcGISFeature* identifiedFeature = dynamic_cast<ArcGISFeature*>(element);
      m_mapView->calloutData()->setVisible(true);

      showEvaluatedArcadeInCallout(identifiedFeature);
    });

  });

  emit mapViewChanged();
}

void QueryFeaturesWithArcadeExpression::showEvaluatedArcadeInCallout(Feature* feature)
{
  QVariantMap profileVariables;
  profileVariables["$feature"] = QVariant::fromValue(feature);
  profileVariables["$map"] = QVariant::fromValue(m_map);

  const QString expressionValue =
      "var crimes = FeatureSetByName($map, 'Crime in the last 60 days');\n"
      "return Count(Intersects($feature, crimes));";

  ArcadeExpression expression {expressionValue};
  ArcadeEvaluator* evaluator = new ArcadeEvaluator(&expression, ArcadeProfile::FormCalculation, this);

  evaluator->evaluateAsync(profileVariables).then(this,
  [this](ArcadeEvaluationResult* arcadeEvaluationResult)
  {
    if (!arcadeEvaluationResult)
      return;

    QVariant evalResult = arcadeEvaluationResult->result();
    const int crimeCount = evalResult.toInt();
    m_mapView->calloutData()->setDetail("Crimes in the last 60 days: " + QString::number(crimeCount));
  });
}
