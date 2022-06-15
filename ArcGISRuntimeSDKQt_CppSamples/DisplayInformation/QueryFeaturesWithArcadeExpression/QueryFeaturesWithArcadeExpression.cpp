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
#include "ArcGISFeatureListModel.h"
#include "ArcGISFeatureTable.h"

#include <QDebug>
#include <QVariantMap>
#include <QMap>

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

        //        if (current_layer_name == "RPD Beats  - City_Beats_Border_1128-4500")
        //        {
        //          m_beatsLayer = static_cast<FeatureLayer*>(m_map->operationalLayers()->at(i));
        //        }
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
  m_mapView->calloutData()->setTitle("RPD Beats");

  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent){
    if (m_mapView->calloutData()->isVisible())
      m_mapView->calloutData()->setVisible(false);
    else
    {
      // set callout position
      Point mapPoint(m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y()));
      m_mapView->calloutData()->setLocation(mapPoint);

      // once the identify is done
      connect(m_mapView, &MapQuickView::identifyLayersCompleted, this, [this, mapPoint](QUuid, const QList<IdentifyLayerResult*>& results)
      {
        //        auto identifyResult = std::unique_ptr<IdentifyLayerResult>(results);

        qDebug() << results.count();

        if (results.empty())
        {
          qDebug() << "results is empty";
          return;
        }

        //        GeoElement* element = identifyResult->geoElements().at(0);
        QList<GeoElement*> element_list = results.first()->geoElements();
        qDebug() << element_list.count();
        GeoElement* element;

        if (element_list.empty())
        {
          //          element = element_list.at(0);
          qDebug() << "Element list is empty";
          return;
        }
        element = element_list.at(0);
        m_identifiedFeature = dynamic_cast<ArcGISFeature*>(element);

        showEvaluatedArcadeInCallout(m_identifiedFeature, mapPoint);
      });

      // Identify the visible layer
      m_mapView->identifyLayers(mouseEvent.x(), mouseEvent.y(), 12, false);

      m_mapView->calloutData()->setVisible(true);
    }
  });

  emit mapViewChanged();
}

void QueryFeaturesWithArcadeExpression::showEvaluatedArcadeInCallout(Feature* feature, Point mapPoint)
{

  qDebug() << "showEvaluatedArcadeInCallout";
//  const QString& expressionValue =
//      "// Get a feature set of crimes from the map by referencing a layer name\n"
//      "var crimes = FeatureSetByName($map, \"Crime in the last 60 days\")\n"

//      "// Count the number of crimes that intersect the provided police beat polygon\n"
//      "return Count(Intersects($feature, crimes))";
  QVariantMap profileVariables;
  profileVariables["$feature"] = QVariant::fromValue(feature);
  profileVariables["$map"] = QVariant::fromValue(m_map);

  const auto expressionValue =
      "var crimes = FeatureSetByName($map, 'Crime in the last 60 days');\n"
      "return Count(Intersects($feature, crimes));";

  ArcadeExpression expression {expressionValue};
  ArcadeEvaluator* evaluator = new ArcadeEvaluator(&expression, ArcadeProfile::FormCalculation, this);

  qDebug() << "Made it past evaluator";



  qDebug() << "Made it past profileVariables";

  //  QVariant eval_result;
  connect(evaluator, &ArcadeEvaluator::evaluateCompleted, this, [this](QUuid, ArcadeEvaluationResult* arcadeEvaluationResult)
  {
    qDebug() << "Made it into lambda";
    //eval_result =  arcadeEvaluationResult ? arcadeEvaluationResult->result() : QVariant{};
    if (!arcadeEvaluationResult/*->result()*/)
    {
      //      auto eval_result = arcadeEvaluationResult->result();
      qDebug() << "arcadeEvaluationResult is empty";
      return;
    }

    auto eval_result = arcadeEvaluationResult->result();

    qDebug() <<  eval_result;
    qDebug() << "Made it past eval_result";
    int crimeCount = eval_result.toInt();

    qDebug() << crimeCount;
    m_mapView->calloutData()->setDetail("Crimes in the last 60 days: " + QString::number(crimeCount));
  });

  connect(evaluator, &ArcadeEvaluator::errorOccurred, this, [](Error e)
  {
    qDebug() << e.message() + e.additionalMessage();
  });


//  qDebug() << "Made it past eval_result";
//  m_mapView->calloutData()->setDetail("$eval_result crimes in the past two months");
//});

evaluator->evaluate(profileVariables);
}
