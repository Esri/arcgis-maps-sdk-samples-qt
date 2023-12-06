// [WriteFile Name=AddClusteringFeatureReductionToAPointFeatureLayer, Category=Layers]
// [Legal]
// Copyright 2023 Esri.

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
#include "pch.hpp" // IWYU pragma: keep
#endif // PCH_BUILD

#include "AddClusteringFeatureReductionToAPointFeatureLayer.h"

#include "ClassBreaksRenderer.h"
#include "SimpleMarkerSymbol.h"
#include "SymbolTypes.h"
#include "ClusteringFeatureReduction.h"
#include "AggregateField.h"
#include "AggregateFieldListModel.h"
#include "ReductionTypes.h"
#include "FeatureLayer.h"
#include "PopupDefinition.h"
#include "Error.h"
#include "LayerListModel.h"
#include "SimpleLabelExpression.h"
#include "TextSymbol.h"
#include "LabelDefinitionListModel.h"
#include "LabelDefinition.h"
#include "ServiceTypes.h"
#include "IdentifyLayerResult.h"
#include "Popup.h"
#include "PopupManager.h"

#include <QColor>

using namespace Esri::ArcGISRuntime;

AddClusteringFeatureReductionToAPointFeatureLayer::AddClusteringFeatureReductionToAPointFeatureLayer(QObject* parent /* = nullptr */):
  QObject(parent)
{
  // Create a map from a web map PortalItem.
  m_map = new Map(new PortalItem("aa44e79a4836413c89908e1afdace2ea", this), this);

  // Get the Zurich buildings feature layer once the map has finished loading.
  connect(m_map, &Map::doneLoading, this, [this](const Error& error)
          {
            if (!error.isEmpty())
            {
              qDebug() << "Map loading error:" << error.message() << error.additionalMessage();
              return;
            }

            m_layer = static_cast<FeatureLayer*>(m_map->operationalLayers()->first());
          });
}

AddClusteringFeatureReductionToAPointFeatureLayer::~AddClusteringFeatureReductionToAPointFeatureLayer() = default;

void AddClusteringFeatureReductionToAPointFeatureLayer::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<AddClusteringFeatureReductionToAPointFeatureLayer>("Esri.Samples", 1, 0, "AddClusteringFeatureReductionToAPointFeatureLayerSample");
}

MapQuickView* AddClusteringFeatureReductionToAPointFeatureLayer::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void AddClusteringFeatureReductionToAPointFeatureLayer::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);
  connect(m_mapView, &MapQuickView::mapScaleChanged, this, &AddClusteringFeatureReductionToAPointFeatureLayer::mapScaleChanged);
  connect(m_mapView, &MapQuickView::mouseClicked, this, &AddClusteringFeatureReductionToAPointFeatureLayer::mouseClicked);

  // Set the initial viewpoint to Zurich, Switzerland.
  m_mapView->setViewpointAsync(Viewpoint(47.38, 8.53, 8e4));

  emit mapViewChanged();
}

void AddClusteringFeatureReductionToAPointFeatureLayer::setClusterRadius(qreal clusterRadius)
{
  if (!m_clusteringFeatureReduction)
    return;

  m_clusteringFeatureReduction->setRadius(clusterRadius);
}

void AddClusteringFeatureReductionToAPointFeatureLayer::setMaxScale(qreal maxScale)
{
  if (!m_clusteringFeatureReduction)
    return;

  m_clusteringFeatureReduction->setMaxScale(maxScale);
}

qreal AddClusteringFeatureReductionToAPointFeatureLayer::mapScale() const
{
  if (!m_mapView)
    return 0.0;

  return m_mapView->mapScale();
}

void AddClusteringFeatureReductionToAPointFeatureLayer::createCustomFeatureReduction()
{
  // Add a class break for each intended value range and define a symbol to display for features in that range.
  // In this case, the average building height ranges from 0 to 8 storeys.
  // For each cluster of features with a given average building height, a symbol is defined with a specified color.
  const QList<ClassBreak*> classBreaks =
      {
          new ClassBreak("0", "0", 0.0, 1.0, new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor(4, 251, 255), 8.0, this), this),
          new ClassBreak("1", "1", 1.0, 2.0, new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor(44, 211, 255), 8.0, this), this),
          new ClassBreak("2", "2", 2.0, 3.0, new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor(74, 181, 255), 8.0, this), this),
          new ClassBreak("3", "3", 3.0, 4.0, new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor(120, 135, 255), 8.0, this), this),
          new ClassBreak("4", "4", 4.0, 5.0, new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor(165, 90, 255), 8.0, this), this),
          new ClassBreak("5", "5", 5.0, 6.0, new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor(194, 61, 255), 8.0, this), this),
          new ClassBreak("6", "6", 6.0, 7.0, new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor(224, 31, 255), 8.0, this), this),
          new ClassBreak("7", "7", 7.0, 8.0, new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor(254, 1, 255), 8.0, this), this)
      };

  // Create a class breaks renderer to apply to the custom feature reduction.
  // Define the field to use for the class breaks renderer.
  // Note that this field name must match the name of an aggregate field contained in the clustering feature reduction's aggregate fields property.
  ClassBreaksRenderer* classBreaksRenderer = new ClassBreaksRenderer("Average Building Height", classBreaks, this);

  // Define a default symbol to use for features that do not fall within any of the ranges defined by the class breaks.
  classBreaksRenderer->setDefaultSymbol(new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColorConstants::Svg::pink, 8, this));

  // Create a new clustering feature reduction using the class breaks renderer.
  m_clusteringFeatureReduction = new ClusteringFeatureReduction(classBreaksRenderer, this);

  // Set the feature reduction's aggregate fields. Note that the field names must match the names of fields in the feature layer's dataset.
  // The aggregate fields summarize values based on the defined aggregate statistic type.
  m_clusteringFeatureReduction->aggregateFields()->append(
      new AggregateField("Total Residential Buildings", "Residential_Buildings", AggregateStatisticType::Sum, this));

  m_clusteringFeatureReduction->aggregateFields()->append(
      new AggregateField("Average Building Height", "Most_common_number_of_storeys", AggregateStatisticType::Mode, this));

  // Enable the feature reduction.
  m_clusteringFeatureReduction->setEnabled(true);

  // Set the popup definition for the custom feature reduction.
  m_clusteringFeatureReduction->setPopupDefinition(new PopupDefinition(m_clusteringFeatureReduction, this)); // use the constructor with popup source

  // Set values for the feature reduction's cluster minimum and maximum symbol sizes.
  // Note that the default values for Max and Min symbol size are 70 and 12 respectively.
  m_clusteringFeatureReduction->setMinSymbolSize(5.0);
  m_clusteringFeatureReduction->setMaxSymbolSize(90.0);

  // Set the feature reduction for the layer.
  Q_ASSERT(m_layer);
  m_layer->setFeatureReduction(m_clusteringFeatureReduction);

  // Set initial slider values.
  // Note that the default value for cluster radius is 60.
  // Increasing the cluster radius increases the number of features that are grouped together into a cluster.
  m_clusteringFeatureReduction->setRadius(60.0);

  // Note that the default value for max scale is 0.
  // The max scale value is the maximum scale at which clustering is applied.
  m_clusteringFeatureReduction->setMaxScale(0.0);
}

void AddClusteringFeatureReductionToAPointFeatureLayer::drawClusters()
{
  // If the layer is not yet loaded, do nothing.
  if (!m_layer)
    return;

  // Create a new clustering feature reduction.
  createCustomFeatureReduction();
}

void AddClusteringFeatureReductionToAPointFeatureLayer::displayLabels(bool checked)
{
  if (!m_clusteringFeatureReduction)
    return;

  if (checked)
  {
    // Create a label definition with a simple label expression.
    SimpleLabelExpression* simpleLabelExpression = new SimpleLabelExpression("[cluster_count]", this);
    TextSymbol* textSymbol = new TextSymbol("", QColorConstants::Svg::black, 15.0, HorizontalAlignment::Center, VerticalAlignment::Middle, this);
    textSymbol->setFontWeight(FontWeight::Bold);
    LabelDefinition* labelDefinition = new LabelDefinition(simpleLabelExpression, textSymbol);
    labelDefinition->setPlacement(LabelingPlacement::PointCenterCenter);

    // Add the label definition to the feature reduction.
    m_clusteringFeatureReduction->labelDefinitions()->append(labelDefinition);
  }
  else
  {
    m_clusteringFeatureReduction->labelDefinitions()->clear();
  }
}

void AddClusteringFeatureReductionToAPointFeatureLayer::mouseClicked(QMouseEvent& mouseEvent)
{
  if (!m_layer)
    return;

  // Identify the tapped observation.
  m_mapView->identifyLayerAsync(m_layer, mouseEvent.position(), 3.0, true)
      .then(this, [this](IdentifyLayerResult* result)
            {
              // clear the list of PopupManagers
              m_popupManagers.clear();

              for (Popup* popup: result->popups())
              {
                // create a popup manager
                PopupManager* popupManager = new PopupManager(popup, this);

                // append popup manager to list
                m_popupManagers.append(popupManager);
              }

              // notify QML that m_popupManagers has changed and to display the popup(s).
              emit popupManagersChanged();
            });
}

QQmlListProperty<PopupManager> AddClusteringFeatureReductionToAPointFeatureLayer::popupManagers()
{
  return QQmlListProperty<PopupManager>(this, &m_popupManagers);
}
