// [WriteFile Name=PerformValveIsolationTrace, Category=UtilityNetwork]
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
#include "UtilityTerminalConfiguration.h"
#include "UtilityTier.h"
#include "UtilityTraceConfiguration.h"
#include "UtilityTraceFilter.h"
#include "UtilityTraceParameters.h"

#include <QUuid>

using namespace Esri::ArcGISRuntime;

namespace  {
//const QString featureServiceUrl = "https://sampleserver7.arcgisonline.com/arcgis/rest/services/UtilityNetwork/NapervilleGas/FeatureServer";
const QString featureServiceUrl = "https://nice.esri.com/server/rest/services/NapervilleGas/FeatureServer";
const QString domainNetworkName = "Pipeline";
const QString tierName = "Pipe Distribution System";
const QString networkSourceName = "Gas Device";
const QString assetGroupName = "Meter";
const QString assetTypeName = "Customer";
const QString globalId = "{98A06E95-70BE-43E7-91B7-E34C9D3CB9FF}";
}

PerformValveIsolationTrace::PerformValveIsolationTrace(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::streetsNightVector(this), this)),
  m_startingLocationOverlay(new GraphicsOverlay(this)),
  m_filterBarriersOverlay(new GraphicsOverlay(this))
{

  Credential* cred = new Credential{"apptest", "app.test1234", this};
  ServiceFeatureTable* distributionLineFeatureTable = new ServiceFeatureTable(featureServiceUrl + "/3", cred, this);
  FeatureLayer* distributionLineLayer = new FeatureLayer(distributionLineFeatureTable, this);

  ServiceFeatureTable* deviceFeatureTable = new ServiceFeatureTable(featureServiceUrl + "/0", cred, this);
  FeatureLayer* deviceLayer = new FeatureLayer(deviceFeatureTable, this);

  // add the feature layers to the map
  m_map->operationalLayers()->append(distributionLineLayer);
  m_map->operationalLayers()->append(deviceLayer);

  m_utilityNetwork = new UtilityNetwork(featureServiceUrl, m_map, cred, this);
  connectSignals();
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

  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent mouseEvent)
  {
    qDebug() << "MouseClicked";

    if (m_map->loadStatus() != LoadStatus::Loaded)
      return;

    constexpr double tolerance = 10.0;
    constexpr bool returnPopups = false;
    m_clickPoint = m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y());
    m_mapView->identifyLayers(mouseEvent.x(), mouseEvent.y(), tolerance, returnPopups);

  });

  // handle the identify results
  connect(m_mapView, &MapQuickView::identifyLayersCompleted, this, &PerformValveIsolationTrace::onIdentifyLayersCompleted);

  // apply renderers
  SimpleMarkerSymbol* startingPointSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Cross, Qt::green, 25, this);
  m_startingLocationOverlay->setRenderer(new SimpleRenderer(startingPointSymbol, this));

  SimpleMarkerSymbol* filterBarrierSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::X, Qt::red, 25, this);
  m_filterBarriersOverlay->setRenderer(new SimpleRenderer(filterBarrierSymbol, this));

  m_mapView->graphicsOverlays()->append(m_startingLocationOverlay);
  m_mapView->graphicsOverlays()->append(m_filterBarriersOverlay);

  emit mapViewChanged();
}

QStringList PerformValveIsolationTrace::categoriesList() const
{
  if (!m_utilityNetwork)
    return { };

  if (m_utilityNetwork->loadStatus() != LoadStatus::Loaded)
    return { };

  const QList<UtilityCategory*> categories = m_utilityNetwork->definition()->categories();
  QStringList strList;
  for (UtilityCategory* category : categories)
  {
    strList << category->name();
  }
  return strList;
}

void PerformValveIsolationTrace::performTrace()
{
  if (m_selectedIndex < 0)
    return;

  // disable UI while trace is run
  m_tasksRunning = true;
  emit tasksRunningChanged();

  for (Layer* layer : *m_map->operationalLayers())
  {
    // clear previous selection from the feature layers
   if (FeatureLayer* featureLayer = dynamic_cast<FeatureLayer*>(layer))
     featureLayer->clearSelection();
  }

  const QList<UtilityCategory*> categories = m_utilityNetwork->definition()->categories();

  // get the selected utility category
  if (categories[m_selectedIndex] != nullptr)
  {
    UtilityCategory* selectedCategory = categories[m_selectedIndex];
    UtilityCategoryComparison* categoryComparison = new UtilityCategoryComparison(selectedCategory, UtilityCategoryComparisonOperator::Exists, this);

    // needs removing
    // set the category comparison to the barriers of the configuration's trace filter
//    m_traceConfiguration->filter()->setBarriers(categoryComparison);

    // set whether to include isolated features
    m_traceConfiguration->setIncludeIsolatedFeatures(m_isolateFeatures);

    // build parameters for the isolation trace
    delete m_traceParams;

    m_traceParams = new UtilityTraceParameters(UtilityTraceType::Isolation, QList<UtilityElement*> {m_startingLocation}, this);

//    UtilityTraceParameters* traceParameters = new UtilityTraceParameters(UtilityTraceType::Isolation, QList<UtilityElement*> {m_startingLocation}, this);
//    traceParameters->setTraceConfiguration(m_traceConfiguration);
    m_traceParams->setTraceConfiguration(m_traceConfiguration);

    connect(m_traceParams, &UtilityTraceParameters::errorOccurred, this, [](Error e)
    {
      qDebug() << e.message();
    });
    // set the user selected filter barries otherwise
    // set the category comparison to the barriers of the configuration's trace filter

    // reset trace configuration filter barriers
    m_traceConfiguration->filter()->setBarriers(nullptr);

    if (!m_filterBarriers.empty())
    {

//      m_traceConfiguration->setFilter(nullptr);
//      auto filterbarriers = traceParameters->filterBarriers();

      auto filterbarriers = m_traceParams->filterBarriers();

      qDebug() << "before traceParameters->filterBarriers(): " << filterbarriers.size();

      for (auto barrier : m_filterBarriers)
      {
        filterbarriers.append(barrier);
      }

      qDebug() << "after traceParameters->filterBarriers(): " << filterbarriers.size();


//      m_traceParams->setFilterBarriers(filterbarriers);
//      m_traceParams->setBarriers(filterbarriers);

      m_traceParams->setFilterBarriers(m_filterBarriers);

//      qDebug() << "m_filterBarriers.size()" <<  m_filterBarriers.size();
//      traceParameters->setFilterBarriers(m_filterBarriers);
//      qDebug() << traceParameters->filterBarriers().size();


      qDebug() << "m_traceParams->filterBarriers().size(): " << m_traceParams->filterBarriers().size();
//      traceParameters->traceConfiguration()->setIncludeIsolatedFeatures(true);

      m_traceParams->traceConfiguration()->setIncludeIsolatedFeatures(true);
    }
    else
    {
//      m_traceConfiguration->filter()->setBarriers(categoryComparison);
//      traceParameters->traceConfiguration()->filter()->setBarriers(categoryComparison);
      m_traceParams->traceConfiguration()->filter()->setBarriers(categoryComparison);
    }
//    m_utilityNetwork->trace(traceParameters);
    m_utilityNetwork->trace(m_traceParams);
  }
}

void PerformValveIsolationTrace::performReset() {
  qDebug() << "reset";
  m_filterBarriersOverlay->graphics()->clear();
  m_filterBarriers.clear();

  for (Layer* layer : *m_map->operationalLayers())
  {
    // clear previous selection from the feature layers
   if (FeatureLayer* featureLayer = dynamic_cast<FeatureLayer*>(layer))
     featureLayer->clearSelection();
  }
}

void PerformValveIsolationTrace::connectSignals()
{
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
    if (UtilityDomainNetwork* domainNetwork = networkDefinition->domainNetwork(domainNetworkName))
    {
      if (UtilityTier* tier = domainNetwork->tier(tierName))
      {
        m_traceConfiguration = tier->traceConfiguration();
      }
    }

    if (!m_traceConfiguration)
      return;

    // create a trace filter
    m_traceConfiguration->setFilter(new UtilityTraceFilter(this));

    // get a default starting location
    if (UtilityNetworkSource* networkSource = networkDefinition->networkSource(networkSourceName))
    {
      if (UtilityAssetGroup* assetGroup = networkSource->assetGroup(assetGroupName))
      {
        if (UtilityAssetType* assetType = assetGroup->assetType(assetTypeName))
        {
          m_startingLocation = m_utilityNetwork->createElementWithAssetType(assetType, QUuid(globalId), nullptr, this);
        }
      }
    }

    if (!m_startingLocation)
      return;

    // display starting location
    m_utilityNetwork->featuresForElements(QList<UtilityElement*> {m_startingLocation});

    // populate the combo box choices
    m_categoriesList = categoriesList();
    emit categoriesListChanged();
  });

  connect(m_utilityNetwork, &UtilityNetwork::traceCompleted, this, [this](QUuid)
  {
    m_tasksRunning = false;
    emit tasksRunningChanged();

    UtilityTraceResultListModel* utilityTraceResultList = m_utilityNetwork->traceResult();

    if (utilityTraceResultList->isEmpty())
    {
      m_noResults = true;
      emit noResultsChanged();
      return;
    }

    if (UtilityElementTraceResult* utilityElementTraceResult = dynamic_cast<UtilityElementTraceResult*>(utilityTraceResultList->at(0)))
    {
      QList<UtilityElement*> utilityElementList = utilityElementTraceResult->elements(this);
      if (utilityElementList.empty())
      {
        m_noResults = true;
        emit noResultsChanged();
        return;
      }

      // iterate through the map's features
      for (Layer* layer : *m_map->operationalLayers())
      {
        if (FeatureLayer* featureLayer = dynamic_cast<FeatureLayer*>(layer))
        {
          // create query parameters to find features whose network source names match layer's feature table name
          QueryParameters queryParameters;
          QList<qint64> objectIds = {};

          for (UtilityElement* utilityElement : utilityElementList)
          {
            const QString networkSourceName = utilityElement->networkSource()->name();
            const QString featureTableName = featureLayer->featureTable()->tableName();
            if (networkSourceName == featureTableName)
            {
              objectIds.append(utilityElement->objectId());
            }
          }
          queryParameters.setObjectIds(objectIds);
          featureLayer->selectFeatures(queryParameters, SelectionMode::New);
        }
      }
    }
  });

  connect(m_utilityNetwork, &UtilityNetwork::featuresForElementsCompleted, this, [this](QUuid)
  {
    // display starting location
    ArcGISFeatureListModel* elementFeaturesList = m_utilityNetwork->featuresForElementsResult();
    const Point startingLocationGeometry = elementFeaturesList->first()->geometry();
    Graphic* graphic = new Graphic(startingLocationGeometry, this);
    m_startingLocationOverlay->graphics()->append(graphic);

    m_mapView->setViewpointCenter(startingLocationGeometry, 3000);
    m_tasksRunning = false;
    emit tasksRunningChanged();
  });
}

bool PerformValveIsolationTrace::noResults() const
{
  return m_noResults;
}

bool PerformValveIsolationTrace::tasksRunning() const
{
  return m_tasksRunning;
}

void PerformValveIsolationTrace::onIdentifyLayersCompleted(QUuid, const QList<IdentifyLayerResult*>& results)
{
  if (results.isEmpty())
    return;

  const IdentifyLayerResult* result = results[0];
  ArcGISFeature* feature = static_cast<ArcGISFeature*>(result->geoElements()[0]);
  // why would feature be null? should remove otherwise
  if (!feature)
    return;
  UtilityElement* element = m_utilityNetwork->createElementWithArcGISFeature(feature);


  const UtilityNetworkSourceType elementSourceType = element->networkSource()->sourceType();

  if (elementSourceType == UtilityNetworkSourceType::Junction)
  {
    qDebug() << "Junction";
    QList<Esri::ArcGISRuntime::UtilityTerminal* > terminals = element->assetType()->terminalConfiguration()->terminals();
    // normally check for multiple terminals but sample doesn't seem to have that occurance.
    if ( terminals.size() > 1)
    {
      qDebug() << "Choose Terminal";
      return;
    }
  }
  else if (elementSourceType == UtilityNetworkSourceType::Edge)
  {
    qDebug() << "Edge";

    if (feature->geometry().geometryType() == GeometryType::Polyline)
    {
      const Polyline line = GeometryEngine::removeZ(feature->geometry());
      // Set how far the element is along the edge.
      const double fraction = GeometryEngine::fractionAlong(line, m_clickPoint, -1);
      qDebug() << fraction;
      element->setFractionAlongEdge(fraction);
    }
  }
  m_filterBarriersOverlay->graphics()->append(new Graphic(m_clickPoint, this));

  m_filterBarriers.append(element);
}
