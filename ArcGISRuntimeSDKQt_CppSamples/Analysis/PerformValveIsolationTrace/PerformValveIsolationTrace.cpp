// [WriteFile Name=PerformValveIsolationTrace, Category=Analysis]
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

#include "PerformValveIsolationTrace.h"

#include "ArcGISFeatureListModel.h"
#include "FeatureLayer.h"
#include "FeatureQueryResult.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "Map.h"
#include "MapQuickView.h"
#include "Point.h"
#include "QueryParameters.h"
#include "ServiceFeatureTable.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleRenderer.h"
#include "UtilityTraceResultListModel.h"
#include "UtilityAssetGroup.h"
#include "UtilityAssetType.h"
#include "UtilityCategory.h"
#include "UtilityCategoryComparison.h"
#include "UtilityDomainNetwork.h"
#include "UtilityElement.h"
#include "UtilityElementTraceResult.h"
#include "UtilityNetwork.h"
#include "UtilityNetworkDefinition.h"
#include "UtilityNetworkSource.h"
#include "UtilityNetworkTypes.h"
#include "UtilityTier.h"
#include "UtilityTraceConfiguration.h"
#include "UtilityTraceFilter.h"
#include "UtilityTraceParameters.h"

#include <QUuid>
#include <memory>

using namespace Esri::ArcGISRuntime;

namespace  {
const QString featureServiceUrl = "https://sampleserver7.arcgisonline.com/arcgis/rest/services/UtilityNetwork/NapervilleGas/FeatureServer";
}

PerformValveIsolationTrace::PerformValveIsolationTrace(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::streetsNightVector(this), this)),
  m_startingLocationOverlay(new GraphicsOverlay(this))
{
  ServiceFeatureTable* distributionLineFeatureTable = new ServiceFeatureTable(featureServiceUrl + "/3", this);
  FeatureLayer* distributionLineLayer = new FeatureLayer(distributionLineFeatureTable, this);

  ServiceFeatureTable* deviceFeatureTable = new ServiceFeatureTable(featureServiceUrl + "/0", this);
  FeatureLayer* deviceLayer = new FeatureLayer(deviceFeatureTable, this);

  // add the feature layers to the map
  m_map->operationalLayers()->append(distributionLineLayer);
  m_map->operationalLayers()->append(deviceLayer);

  m_utilityNetwork = new UtilityNetwork(featureServiceUrl, m_map, this);
  connect(m_utilityNetwork, &UtilityNetwork::doneLoading, this, [this](const Error& error)
  {
    if (!error.isEmpty())
    {
      qDebug() << error.message() << error.additionalMessage();
      return;
    }

    if (m_utilityNetwork->loadStatus() != LoadStatus::Loaded)
      return;

    // get a trace configuration from a tier
    UtilityNetworkDefinition* networkDefinition = m_utilityNetwork->definition();
    UtilityDomainNetwork* domainNetwork = networkDefinition->domainNetwork("Pipeline");
    UtilityTier* tier = domainNetwork->tier("Pipe Distribution System");
    m_traceConfiguration = tier->traceConfiguration();

    // create a trace filter
    m_traceConfiguration->setFilter(new UtilityTraceFilter(this));

    // get a default starting location
    UtilityNetworkSource* networkSource = networkDefinition->networkSource("Gas Device");
    UtilityAssetGroup* assetGroup = networkSource->assetGroup("Meter");
    UtilityAssetType* assetType = assetGroup->assetType("Customer");
    m_startingLocation = m_utilityNetwork->createElementWithAssetType(assetType, QUuid("98A06E95-70BE-43E7-91B7-E34C9D3CB9FF"), nullptr, this);

    // display starting location
    m_utilityNetwork->featuresForElements(QList<UtilityElement*> {m_startingLocation});

    // populate the combo box choices
    m_categoriesList = categoriesList();
    emit categoriesListChanged();



  });



  connect(m_utilityNetwork, &UtilityNetwork::traceCompleted, this, [this](QUuid)
  {
    qDebug("trace completed");
    m_uiEnabled = true;
    emit uiEnabledChanged();

    UtilityTraceResultListModel* utilityTraceResultList = m_utilityNetwork->traceResult();

    if (utilityTraceResultList->isEmpty())
    {
      qWarning() << "No results found.";
      return;
    }

    if (UtilityElementTraceResult* utilityElementTraceResult = dynamic_cast<UtilityElementTraceResult*>(utilityTraceResultList->at(0)))
    {
      // iterate through the map's features
      for (Layer* layer : *m_map->operationalLayers())
      {
        if (FeatureLayer* featureLayer = dynamic_cast<FeatureLayer*>(layer))
        {
          m_currentFeatureLayer = featureLayer;
          // create query parameters to find features whose network source names match layer's feature table name
          QueryParameters queryParameters;
          QList<qint64> objectIds = {};

          for (UtilityElement* utilityElement : utilityElementTraceResult->elements(this))
          {
            QString networkSourceName = utilityElement->networkSource()->name();
            QString featureTableName = m_currentFeatureLayer->featureTable()->tableName();
            if (networkSourceName == featureTableName)
            {
              objectIds.append(utilityElement->objectId());
            }
          }
          queryParameters.setObjectIds(objectIds);
          qDebug() << queryParameters.objectIds().length();
          m_currentFeatureLayer->selectFeatures(queryParameters, SelectionMode::New);
        }
      }
      qDebug("done iterating");

    }

  });

  m_utilityNetwork->load();
}

PerformValveIsolationTrace::~PerformValveIsolationTrace() = default;

void PerformValveIsolationTrace::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<PerformValveIsolationTrace>("Esri.Samples", 1, 0, "PerformValveIsolationTraceSample");
}

MapQuickView* PerformValveIsolationTrace::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void PerformValveIsolationTrace::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  // apply renderers
  SimpleMarkerSymbol* startingPointSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Cross, Qt::green, 25, this);
  m_startingLocationOverlay->setRenderer(new SimpleRenderer(startingPointSymbol, this));
  m_mapView->graphicsOverlays()->append(m_startingLocationOverlay);

  connect(m_utilityNetwork, &UtilityNetwork::featuresForElementsCompleted, this, [this](QUuid)
  {
    // display starting location
    ArcGISFeatureListModel* elementFeaturesList = m_utilityNetwork->featuresForElementsResult();
    const Point startingLocationGeometry = elementFeaturesList->first()->geometry();
    Graphic* graphic = new Graphic(startingLocationGeometry, this);
    m_startingLocationOverlay->graphics()->append(graphic);

    m_mapView->setViewpointCenter(startingLocationGeometry, 3000);
    m_uiEnabled = true;
    emit uiEnabledChanged();
  });

  emit mapViewChanged();
}

QStringList PerformValveIsolationTrace::categoriesList() const
{
  if (!m_utilityNetwork)
    return { };

  if (m_utilityNetwork->loadStatus() != LoadStatus::Loaded)
    return { };

  QList<UtilityCategory*> categories = m_utilityNetwork->definition()->categories();
  QStringList strList;
  for (UtilityCategory* category : categories)
  {
    strList << category->name();
  }
  return strList;
}

void PerformValveIsolationTrace::performTrace()
{
  qDebug("clicked");

  // disable UI while trace is run
  m_uiEnabled = false;
  emit uiEnabledChanged();

  for (Layer* layer : *m_map->operationalLayers())
  {
     // clear previous selection from the feature layers
    if (FeatureLayer* featureLayer = dynamic_cast<FeatureLayer*>(layer))
    {
      featureLayer->clearSelection();
    }
  }

  if (m_selectedIndex < 0)
    return;

  QList<UtilityCategory*> categories = m_utilityNetwork->definition()->categories();

  // get the selected utility category
  if (categories[m_selectedIndex] != nullptr)
  {
    UtilityCategory* selectedCategory = categories[m_selectedIndex];
    UtilityCategoryComparison* categoryComparison = new UtilityCategoryComparison(selectedCategory, UtilityCategoryComparisonOperator::Exists, this);

    // set the category comparison to the barriers of the configuration's trace filter
    m_traceConfiguration->filter()->setBarriers(categoryComparison);

    // set whether to include isolated features
    m_traceConfiguration->setIncludeIsolatedFeatures(m_isolateFeatures);

    // build parameters for the isolation trace
    UtilityTraceParameters* traceParameters = new UtilityTraceParameters(UtilityTraceType::Isolation, QList<UtilityElement*> {m_startingLocation}, this);
    traceParameters->setTraceConfiguration(m_traceConfiguration);

    m_utilityNetwork->trace(traceParameters);
  }

//    connect(m_currentFeatureLayer, &FeatureLayer::selectFeaturesCompleted, this, [this](QUuid, FeatureQueryResult* rawFeatureQueryResult)
//    {
//      qDebug("select features completed");
//    });
}

bool PerformValveIsolationTrace::uiEnabled() const
{
  return m_uiEnabled;
}
