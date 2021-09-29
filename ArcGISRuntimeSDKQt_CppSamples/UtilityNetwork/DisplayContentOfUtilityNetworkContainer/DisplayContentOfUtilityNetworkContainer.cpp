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
#include "UtilityNetworkTypes.h"

using namespace Esri::ArcGISRuntime;

namespace {
Symbol* attachmentSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Dot, Qt::blue, 3);
Symbol* connectivitySymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Dot, Qt::red, 3);
Symbol* boundingBoxSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Dot, Qt::yellow, 3);
}

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

bool DisplayContentOfUtilityNetworkContainer::showContainerView() const
{
  return m_showContainerView;
}

void DisplayContentOfUtilityNetworkContainer::setShowContainerView(bool showContainerView)
{
  m_showContainerView = showContainerView;
  emit showContainerViewChanged();

  if (m_showContainerView)
  {
    m_previousViewpoint = m_mapView->currentViewpoint(ViewpointType::BoundingGeometry);
    for (Layer* layer : *m_mapView->map()->operationalLayers())
    {
      layer->setVisible(false);
    }
  }
  else
  {
    m_mapView->setViewpoint(m_previousViewpoint, 0.5);
    m_containerGraphicsOverlay->graphics()->clear();

    for (Layer* layer : *m_mapView->map()->operationalLayers())
    {
      layer->setVisible(true);
    }
  }
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

  m_containerGraphicsOverlay = new GraphicsOverlay;
  m_mapView->graphicsOverlays()->append(m_containerGraphicsOverlay);

  createConnections();

  emit mapViewChanged();
}

void DisplayContentOfUtilityNetworkContainer::createConnections()
{
  connect(m_mapView, &MapQuickView::mouseClicked, this, &DisplayContentOfUtilityNetworkContainer::onMouseClicked);
  connect(m_mapView, &MapQuickView::identifyLayersCompleted, this, &DisplayContentOfUtilityNetworkContainer::onIdentifyLayersCompleted);
  connect(m_utilityNetwork, &UtilityNetwork::featuresForElementsCompleted, this, &DisplayContentOfUtilityNetworkContainer::onFeaturesForElementsCompleted);

  connect(m_map, &Map::errorOccurred, this, [](Error e)
  {
    qDebug() << "map error:" << e.message() << e.additionalMessage();
  });

  connect(m_mapView, &MapQuickView::errorOccurred, this, [](Error e)
  {
    qDebug() << "map view error:" << e.message() << e.additionalMessage();
  });

  connect(m_utilityNetwork, &UtilityNetwork::errorOccurred, this, [](Error error)
  {
    qWarning() << "Utility Network error occured: " << error.message() << error.additionalMessage();
  });

  connect(m_utilityNetwork, &UtilityNetwork::associationsCompleted, this, [this](QUuid, QList<UtilityAssociation*> containmentAssociations)
  {
    if (!m_showContainerView)
      getContainmentAssociations(containmentAssociations);

    else
      showAttachmentAndConnectivitySymbols(containmentAssociations);
  });
}

void DisplayContentOfUtilityNetworkContainer::onMouseClicked(QMouseEvent& mouseEvent)
{
  if (m_map->loadStatus() != LoadStatus::Loaded || m_utilityNetwork->loadStatus() != LoadStatus::Loaded || !m_taskWatcher.isDone())
    return;

  constexpr double tolerance = 5;
  constexpr bool returnPopupsOnly = false;

  m_mapView->identifyLayers(mouseEvent.x(), mouseEvent.y(), tolerance, returnPopupsOnly);
}

void DisplayContentOfUtilityNetworkContainer::onIdentifyLayersCompleted(QUuid, QList<Esri::ArcGISRuntime::IdentifyLayerResult*> identifyResults)
{
  if (m_containerElement)
  {
    delete m_containerElement;
    m_containerElement = nullptr;
  }

  for (IdentifyLayerResult* layerResult : identifyResults)
  {
    if (!m_containerElement && dynamic_cast<SubtypeFeatureLayer*>(layerResult->layerContent()))
    {
      for (IdentifyLayerResult* sublayerResult : layerResult->sublayerResults())
      {
        for (GeoElement* geoElement : sublayerResult->geoElements())
        {
          if (ArcGISFeature* feature = dynamic_cast<ArcGISFeature*>(geoElement))
          {
            m_containerElement = m_utilityNetwork->createElementWithArcGISFeature(feature);
            if (m_containerElement)
            {
              m_taskWatcher = m_utilityNetwork->associations(m_containerElement, UtilityAssociationType::Containment);
              break;
            }
          }
        }
        if (m_containerElement)
          break;
      }
    }
  }
}

void DisplayContentOfUtilityNetworkContainer::onFeaturesForElementsCompleted(QUuid)
{
  QList<Feature*> contentFeatures = m_utilityNetwork->featuresForElementsResult()->features();
  for (Feature* content : contentFeatures)
  {
    Symbol* symbol = dynamic_cast<ArcGISFeatureTable*>(content->featureTable())->layerInfo().drawingInfo().renderer(this)->symbol(content);
    m_containerGraphicsOverlay->graphics()->append(new Graphic(content->geometry(), symbol, this));
  }
  m_taskWatcher = m_utilityNetwork->associations(m_containerGraphicsOverlay->extent());
}

void DisplayContentOfUtilityNetworkContainer::getContainmentAssociations(QList<Esri::ArcGISRuntime::UtilityAssociation*> containmentAssociations)
{
  if (m_showContainerView)
    return;

  QList<UtilityElement*> contentElements;
  for (UtilityAssociation* association : containmentAssociations)
  {
    UtilityElement* otherElement = association->fromElement()->objectId() == m_containerElement->objectId() ? association->toElement() : association->fromElement();
    contentElements.append(otherElement);
  }

  if (!contentElements.isEmpty())
  {
    setShowContainerView(true);
    m_taskWatcher = m_utilityNetwork->featuresForElements(contentElements);
  }
}

void DisplayContentOfUtilityNetworkContainer::showAttachmentAndConnectivitySymbols(QList<Esri::ArcGISRuntime::UtilityAssociation*> containmentAssociations)
{
  for (UtilityAssociation* association : containmentAssociations)
  {
    Symbol* symbol = association->associationType() == UtilityAssociationType::Attachment ? attachmentSymbol : connectivitySymbol;
    m_containerGraphicsOverlay->graphics()->append(new Graphic(association->geometry(), symbol, this));
  }
  if (m_containerGraphicsOverlay->graphics()->size() == 1 && m_containerGraphicsOverlay->graphics()->first()->geometry().geometryType() == GeometryType::Point)
  {
    m_mapView->setViewpointAndWait(Viewpoint(Point(m_containerGraphicsOverlay->graphics()->first()->geometry()), m_containerElement->assetType()->containerViewScale()));
    m_boundingBox = m_mapView->currentViewpoint(ViewpointType::BoundingGeometry).targetGeometry();
    m_mapView->setViewpointAndWait(m_previousViewpoint);
  }
  else
  {
    m_boundingBox = GeometryEngine::buffer(m_containerGraphicsOverlay->extent(), 0.05);
  }
  m_containerGraphicsOverlay->graphics()->append(new Graphic(m_boundingBox, boundingBoxSymbol, this));
  m_taskWatcher = m_mapView->setViewpoint(Viewpoint(GeometryEngine::buffer(m_containerGraphicsOverlay->extent(), 0.2)), .5);
}
