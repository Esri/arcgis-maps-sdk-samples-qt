// [WriteFile Name=DisplayContentOfUtilityNetworkContainer, Category=UtilityNetwork]
// [Legal]
// Copyright 2021 Esri.

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

#include "DisplayContentOfUtilityNetworkContainer.h"

#include "ArcGISFeature.h"
#include "ArcGISFeatureListModel.h"
#include "ArcGISFeatureTable.h"
#include "AuthenticationManager.h"
#include "GeometryEngine.h"
#include "SimpleLineSymbol.h"
#include "SubtypeFeatureLayer.h"
#include "Symbol.h"
#include "Map.h"
#include "MapQuickView.h"
#include "UtilityAssetType.h"
#include "UtilityAssociation.h"
#include "UtilityElement.h"
#include "UtilityNetwork.h"
#include "UtilityNetworkListModel.h"

using namespace Esri::ArcGISRuntime;

DisplayContentOfUtilityNetworkContainer::DisplayContentOfUtilityNetworkContainer(QObject* parent /* = nullptr */):
  QObject(parent),
  m_cred(new Credential("viewer01", "I68VGU^nMurF", this))
{
  connect(AuthenticationManager::instance(), &AuthenticationManager::authenticationChallenge, this, [this](AuthenticationChallenge* challenge)
  {
    challenge->continueWithCredential(m_cred);
  });

  m_map = new Map(QUrl("https://sampleserver7.arcgisonline.com/portal/home/item.html?id=813eda749a9444e4a9d833a4db19e1c8"), this);
  m_utilityNetwork = new UtilityNetwork(QUrl("https://sampleserver7.arcgisonline.com/server/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer"), m_cred, this);

  m_map->utilityNetworks()->append(m_utilityNetwork);
  m_utilityNetwork->load();
}

DisplayContentOfUtilityNetworkContainer::~DisplayContentOfUtilityNetworkContainer() = default;

void DisplayContentOfUtilityNetworkContainer::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DisplayContentOfUtilityNetworkContainer>("Esri.Samples", 1, 0, "DisplayContentOfUtilityNetworkContainerSample");
}

MapQuickView* DisplayContentOfUtilityNetworkContainer::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void DisplayContentOfUtilityNetworkContainer::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  m_graphicsOverlay = new GraphicsOverlay;
  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  // once map is set, connect to MapQuickView mouse clicked signal
  connect(m_mapView, &MapQuickView::mouseClicked, this, &DisplayContentOfUtilityNetworkContainer::onMouseClicked);

  // connect to MapQuickView::identifyLayerCompleted signal
  connect(m_mapView, &MapQuickView::identifyLayerCompleted, this, &DisplayContentOfUtilityNetworkContainer::onIdentifyLayerCompleted);

  connect(m_utilityNetwork, &UtilityNetwork::errorOccurred, this, [](Error error)
  {
    qWarning() << "Utility Network error occured: " << error.message() << error.additionalMessage();
  });

  connect(m_utilityNetwork, &UtilityNetwork::associationsCompleted, this, [this](QUuid, QList<UtilityAssociation*> containmentAssociations)
  {
    if (!m_containerView)
      getContainmentAssociations(containmentAssociations);

    else
      showAttachmentAndConnectivitySymbols(containmentAssociations);
  });

  connect(m_utilityNetwork, &UtilityNetwork::featuresForElementsCompleted, this, &DisplayContentOfUtilityNetworkContainer::onFeaturesForElementsCompleted);

  emit mapViewChanged();
}

void DisplayContentOfUtilityNetworkContainer::onMouseClicked(QMouseEvent& mouseEvent)
{
  if (m_map->loadStatus() != LoadStatus::Loaded || m_utilityNetwork->loadStatus() != LoadStatus::Loaded)
    return;

  qDebug() << "mouse clicked";
  Layer* layer = m_map->operationalLayers()->at(0);
  if (layer->layerType() == LayerType::SubtypeFeatureLayer)
  {
    m_subtypeFeatureLayer = static_cast<SubtypeFeatureLayer*>(layer);
  }

  constexpr double tolerance = 12;
  constexpr bool returnPopupsOnly = false;

  m_mapView->identifyLayer(m_subtypeFeatureLayer, mouseEvent.x(), mouseEvent.y(), tolerance, returnPopupsOnly);
}

void DisplayContentOfUtilityNetworkContainer::onIdentifyLayerCompleted(QUuid, IdentifyLayerResult* rawIdentifyResult)
{
  qDebug() << "id layer completed";
  auto identifyResult = std::unique_ptr<IdentifyLayerResult>(rawIdentifyResult);

  if (!identifyResult)
  {
    qDebug() << "Identify error occured: " << identifyResult->error().message() << identifyResult->error().additionalMessage();
    return;
  }

  QList<IdentifyLayerResult*> sublayerResults = identifyResult->sublayerResults();

  if (m_utilityElement)
  {
    delete m_utilityElement;
    m_utilityElement = nullptr;
  }

  for (IdentifyLayerResult* sublayerResult : identifyResult->sublayerResults())
  {
    for (GeoElement* geoElement : sublayerResult->geoElements())
    {
      if (ArcGISFeature* feature = dynamic_cast<ArcGISFeature*>(geoElement))
      {
        m_utilityElement = m_utilityNetwork->createElementWithArcGISFeature(feature);
        if (m_utilityElement)
        {
          m_utilityNetwork->associations(m_utilityElement, static_cast<UtilityAssociationType>(2)); //UtilityAssociationType::Containment);
          break;
        }
      }
    }
    if (m_utilityElement)
      break;
  }
}

void DisplayContentOfUtilityNetworkContainer::onFeaturesForElementsCompleted(QUuid)
{
  qDebug() << "features for elements completed";
  QList<Feature*> contentFeatures = m_utilityNetwork->featuresForElementsResult()->features();
  for (Feature* content : contentFeatures)
  {
    Symbol* symbol = dynamic_cast<ArcGISFeatureTable*>(content->featureTable())->layerInfo().drawingInfo().renderer(this)->symbol(content);
    m_graphicsOverlay->graphics()->append(new Graphic(content->geometry(), symbol, this));
  }

  m_utilityNetwork->associations(m_graphicsOverlay->extent());
}

void DisplayContentOfUtilityNetworkContainer::getContainmentAssociations(QList<Esri::ArcGISRuntime::UtilityAssociation*> containmentAssociations)
{
  if (m_containerView)
    return;

  QList<UtilityElement*> contentElements;
  for (UtilityAssociation* association : containmentAssociations)
  {
    UtilityElement* otherElement = association->fromElement()->objectId() == m_utilityElement->objectId() ? association->toElement() : association->fromElement();
    contentElements.append(otherElement);
  }
  qDebug() << contentElements.size();

  if (!contentElements.isEmpty())
  {
    Viewpoint m_previousViewpoint = m_mapView->currentViewpoint(ViewpointType::BoundingGeometry);
    for (Layer* layer : *m_mapView->map()->operationalLayers())
    {
      //layer->setVisible(false);
    }
    m_containerView = true;
    m_utilityNetwork->featuresForElements(contentElements);
  }
}

void DisplayContentOfUtilityNetworkContainer::showAttachmentAndConnectivitySymbols(QList<Esri::ArcGISRuntime::UtilityAssociation*> containmentAssociations)
{
  Symbol* attachmentSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Dot, Qt::blue, 3);
  Symbol* connectivitySymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Dot, Qt::red, 3);
  Symbol* boundingBoxSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Dot, Qt::yellow, 3);
  Geometry boundingBox;

  for (UtilityAssociation* association : containmentAssociations)
  {
    Symbol* symbol = association->associationType() == static_cast<UtilityAssociationType>(3) ? attachmentSymbol : connectivitySymbol;
    m_graphicsOverlay->graphics()->append(new Graphic(association->geometry(), symbol, this));
  }
  if (m_graphicsOverlay->graphics()->size() == 1 && m_graphicsOverlay->graphics()->first()->geometry().geometryType() == GeometryType::Point)
  {
    m_mapView->setViewpoint(Viewpoint(m_graphicsOverlay->graphics()->first()->geometry(), m_utilityElement->assetType()->containerViewScale()));
    boundingBox = m_mapView->currentViewpoint(ViewpointType::BoundingGeometry).targetGeometry();
  }
  else
  {
    boundingBox = GeometryEngine::buffer(m_graphicsOverlay->extent(), 0.05);
  }
  m_graphicsOverlay->graphics()->append(new Graphic(boundingBox, boundingBoxSymbol, this));
  m_mapView->setViewpoint(Viewpoint(GeometryEngine::buffer(m_graphicsOverlay->extent(), 0.1)), .5);
}
