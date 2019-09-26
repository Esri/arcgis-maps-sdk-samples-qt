// [WriteFile Name=FindFeaturesUtilityNetwork, Category=Analysis]
// [Legal]
// Copyright 2019 Esri.

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

#include "FindFeaturesUtilityNetwork.h"

#include "Map.h"
#include "MapQuickView.h"
#include "ServiceFeatureTable.h"
#include "FeatureLayer.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleRenderer.h"
#include "UtilityElement.h"
#include "UtilityElementTraceResult.h"
#include "UtilityNetwork.h"
#include "UtilityNetworkDefinition.h"
#include "UtilityNetworkSource.h"
#include "UtilityNetworkTypes.h"
#include "UtilityTraceParameters.h"
#include "UtilityTraceResultListModel.h"
#include "UtilityAssetGroup.h"
#include "UtilityAssetType.h"
#include "UtilityTerminal.h"
#include "UtilityTerminalConfiguration.h"
#include "ArcGISFeatureListModel.h"
#include "GeometryEngine.h"
#include "GeoElement.h"
//#include "Viewpoint.h"

using namespace Esri::ArcGISRuntime;

FindFeaturesUtilityNetwork::FindFeaturesUtilityNetwork(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::streetsNightVector(this), this))
{
  m_map->setInitialViewpoint(Viewpoint(Envelope(-9813547.35557238, 5129980.36635111, -9813185.0602376, 5130215.41254146, SpatialReference(3857))));
  m_lineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("DarkCyan"), 3, this);


  m_deviceFeatureTable = new ServiceFeatureTable(QUrl("https://sampleserver7.arcgisonline.com/arcgis/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer/100"), this);
  m_deviceLayer = new FeatureLayer(m_deviceFeatureTable, this);

  m_lineFeatureTable = new ServiceFeatureTable(QUrl("https://sampleserver7.arcgisonline.com/arcgis/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer/115"), this);
  m_lineLayer = new FeatureLayer(m_lineFeatureTable, this);
  m_lineLayer->setRenderer(new SimpleRenderer(m_lineSymbol, this));

  m_map->operationalLayers()->append(m_lineLayer);
  m_map->operationalLayers()->append(m_deviceLayer);

  connect(m_map, &Map::doneLoading, this, [this](Error e)
  {
    if(!e.isEmpty())
      return;

    m_graphicsOverlay = new GraphicsOverlay(this);
    m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

    m_startingSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Cross, QColor("green"), 20, this);
    m_barrierSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::X, QColor("red"), 20, this);

    m_utilityNetwork = new UtilityNetwork(m_serviceUrl, m_map, this);
    connect(m_utilityNetwork, &UtilityNetwork::traceCompleted, this, [this](QUuid)
    {
      qDebug() << "trace completed";

      if (m_utilityNetwork->traceResult()->isEmpty())
        return;

      UtilityTraceResult* result = m_utilityNetwork->traceResult()->at(0);
      const QList<UtilityElement*> elements = static_cast<UtilityElementTraceResult*>(result)->elements(this);

      // breaking the utility elements down by sources make no difference with selection of features
      QList<UtilityElement*> devices;
      QList<UtilityElement*> junctions;
      QList<UtilityElement*> lines;

      qDebug() << elements.size();
      for(UtilityElement* item : elements)
      {
        if (item->networkSource()->name() == "Electric Distribution Junction")
        {
          junctions.append(item);
        }
        else if (item->networkSource()->name() == "Electric Distribution Device")
        {
          devices.append(item);
        }
        else if (item->networkSource()->name() == "Electric Distribution Line")
        {
          lines.append(item);
        }
        else
        {
          qDebug() << "ERORR";
        }
      }

      m_utilityNetwork->featuresForElements(junctions);
      m_utilityNetwork->featuresForElements(devices);
      m_utilityNetwork->featuresForElements(lines);

      // now convert the UtilityElement to Features for selection
//      m_utilityNetwork->featuresForElements(elements);

    });

    connect(m_utilityNetwork, &UtilityNetwork::featuresForElementsCompleted, this, [this](QUuid)
    {


      if(m_utilityNetwork->featuresForElementsResult()->isEmpty())
        return;

//      ArcGISFeatureListModel* featuresModel = m_utilityNetwork->featuresForElementsResult();
//      const QList<Feature*> allFeatures = featuresModel->features(this);
//      qDebug() << allFeatures.count();
//      m_deviceLayer->selectFeatures(allFeatures);
//      m_lineLayer->selectFeatures(allFeatures);

      // breaking it down by table name makes no difference currently
      ArcGISFeatureListModel* featuresModel = m_utilityNetwork->featuresForElementsResult();
      const QList<Feature*> allFeatures = featuresModel->features(this);
      if (allFeatures[0]->featureTable()->tableName() == m_lineLayer->featureTable()->tableName())
      {
        m_lineLayer->selectFeatures(allFeatures);
      }
      else if (allFeatures[0]->featureTable()->tableName() == m_deviceLayer->featureTable()->tableName())
      {
        qDebug()<<allFeatures.size();
        m_deviceLayer->selectFeatures(allFeatures);
      }


    });
    m_utilityNetwork->load();
    m_traceParams = new UtilityTraceParameters(UtilityTraceType::Connected, {}, this);


    connectSignals();
  });


}

void FindFeaturesUtilityNetwork::connectSignals()
{
      // identify layers on mouse click
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    if(m_map->loadStatus() != LoadStatus::Loaded)
      return;

    const double tolerance = 10.0;
    const bool returnPopups = false;
    m_clickPoint = m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y());
    m_mapView->identifyLayers(mouseEvent.x(), mouseEvent.y(), tolerance, returnPopups);
  });

  // handle the identify results
  connect(m_mapView, &MapQuickView::identifyLayersCompleted, this, [this](QUuid, const QList<IdentifyLayerResult*>& results)
  {
    if (results.isEmpty())
      return;

    IdentifyLayerResult* result = results[0];
    m_feature = dynamic_cast<ArcGISFeature*>(result->geoElements()[0]);
    UtilityElement* element;

    UtilityNetworkSource* networkSource = m_utilityNetwork->definition()->networkSource(m_feature->featureTable()->tableName());
    networkSource->featureTable()->load();

    if (networkSource->sourceType() == UtilityNetworkSourceType::Junction)
    {
      QString assetGroupFieldName = dynamic_cast<ArcGISFeatureTable*>(m_feature->featureTable())->subtypeField();
      int assetGroupCode = m_feature->attributes()->attributeValue(assetGroupFieldName).toInt();
      UtilityAssetGroup* assetGroup;

      for (UtilityAssetGroup* group : networkSource->assetGroups())
      {
        if ( group->code() == assetGroupCode )
        {
          assetGroup = group;
          break;
        }
      }

      int assetTypeCode = m_feature->attributes()->attributeValue("assettype").toInt();

      UtilityAssetType* assetType;
      for (UtilityAssetType* type : assetGroup->assetTypes())
      {
        if (type->code() == assetTypeCode)
        {
          assetType = type;
          break;
        }
      }


//      QList<UtilityTerminal*> terminals = assetType->terminalConfiguration()->terminals();
      m_terminals = assetType->terminalConfiguration()->terminals();

      if ( m_terminals.size() > 1)
      {
        terminalDialogVisisble = true;
        emit terminalDialogVisisbleChanged();
        return;
      }
      else if ( m_terminals.size() == 1 )
      {
        element = m_utilityNetwork->createElementWithArcGISFeature(m_feature, m_terminals[0]);
      }

    }
    else if (networkSource->sourceType() == UtilityNetworkSourceType::Edge)
    {
      element = m_utilityNetwork->createElementWithArcGISFeature(m_feature, nullptr ,this);

      if (m_feature->geometry().geometryType() == GeometryType::Polyline)
      {
        Polyline line = GeometryEngine::removeZ(m_feature->geometry());
        element->setFractionAlongEdge(GeometryEngine::fractionAlong(line, m_clickPoint, -1));
      }
    }

    updateTraceParams(element);
  });
}

FindFeaturesUtilityNetwork::~FindFeaturesUtilityNetwork() = default;

void FindFeaturesUtilityNetwork::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<FindFeaturesUtilityNetwork>("Esri.Samples", 1, 0, "FindFeaturesUtilityNetworkSample");
}

MapQuickView* FindFeaturesUtilityNetwork::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void FindFeaturesUtilityNetwork::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}

void FindFeaturesUtilityNetwork::multiTerminalIndex(const int &index)
{
  if (m_terminals.size() < 2)
    return;
  if (!m_feature)
    return;

  UtilityElement* element;
  element = m_utilityNetwork->createElementWithArcGISFeature(m_feature, m_terminals[index]);

  qDebug() << "hoorah";
  updateTraceParams(element);

}

void FindFeaturesUtilityNetwork::updateTraceParams(UtilityElement* element)
{
  if (startingLocationsEnabled)
  {
    m_startingLocations.append(element);
    m_traceParams->setStartingLocations(m_startingLocations);
    Graphic* traceLocation = new Graphic(m_clickPoint, m_startingSymbol, this);
    m_graphicsOverlay->graphics()->append(traceLocation);
  }
  else
  {
    m_barriers.append(element);
    m_traceParams->setBarriers(m_barriers);
    Graphic* traceLocation = new Graphic(m_clickPoint, m_barrierSymbol, this);
    m_graphicsOverlay->graphics()->append(traceLocation);
  }

}

void FindFeaturesUtilityNetwork::trace()
{
  m_utilityNetwork->trace(m_traceParams);
}

void FindFeaturesUtilityNetwork::reset()
{
  m_startingLocations.clear();
  m_traceParams->setStartingLocations(m_startingLocations);
  m_barriers.clear();
  m_traceParams->setBarriers(m_barriers);
  m_graphicsOverlay->graphics()->clear();

  for(Layer* layer : *m_map->operationalLayers())
  {
    FeatureLayer* featureLayer = static_cast<FeatureLayer*>(layer);
    featureLayer->clearSelection();
  }
}
