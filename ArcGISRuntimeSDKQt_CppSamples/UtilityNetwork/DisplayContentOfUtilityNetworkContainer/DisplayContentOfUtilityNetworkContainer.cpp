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
#include "SymbolImageProvider.h"

#include "ArcGISFeatureListModel.h"
#include "ArcGISFeatureTable.h"
#include "AuthenticationManager.h"
#include "GeometryEngine.h"
#include "Map.h"
#include "MapQuickView.h"
#include "SimpleLineSymbol.h"
#include "SubtypeFeatureLayer.h"
#include "UtilityAssetType.h"
#include "UtilityAssociation.h"
#include "UtilityElement.h"
#include "UtilityNetwork.h"
#include "UtilityNetworkListModel.h"
#include "UtilityNetworkTypes.h"

#include <QImage>
#include <QQmlContext>

using namespace Esri::ArcGISRuntime;

DisplayContentOfUtilityNetworkContainer::DisplayContentOfUtilityNetworkContainer(QObject* parent /* = nullptr */):
  QObject(parent),
  m_cred(new Credential("viewer01", "I68VGU^nMurF", this))
{
  connect(AuthenticationManager::instance(), &AuthenticationManager::authenticationChallenge, this, [this](AuthenticationChallenge* challenge)
  {
    challenge->continueWithCredential(m_cred);
  });

  // Load a web map that includes ArcGIS Pro Subtype Group Layers with only container features visible (i.e. fuse bank, switch bank, transformer bank, hand hole and junction box)
  m_map = new Map(QUrl("https://sampleserver7.arcgisonline.com/portal/home/item.html?id=813eda749a9444e4a9d833a4db19e1c8"), this);

  // Create and load a UtilityNetwork with the same feature service URL as the layers in the Map
  m_utilityNetwork = new UtilityNetwork(QUrl("https://sampleserver7.arcgisonline.com/server/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer"), this);

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

  // Add a GraphicsOverlay for displaying a container view.
  m_containerGraphicsOverlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(m_containerGraphicsOverlay);

  createConnections();
  createLegend();

  emit mapViewChanged();
}

void DisplayContentOfUtilityNetworkContainer::createConnections()
{
  connect(m_mapView, &MapQuickView::mouseClicked, this, &DisplayContentOfUtilityNetworkContainer::identifyFeaturesAtMouseClick);
  connect(m_mapView, &MapQuickView::identifyLayersCompleted, this, &DisplayContentOfUtilityNetworkContainer::getUtilityAssociationsOfFeature);
  connect(m_utilityNetwork, &UtilityNetwork::featuresForElementsCompleted, this, &DisplayContentOfUtilityNetworkContainer::displayFeaturesAndGetAssociations);

  connect(m_utilityNetwork, &UtilityNetwork::associationsCompleted, this, [this](QUuid, QList<UtilityAssociation*> containmentAssociations)
  {
    if (!m_showContainerView)
      getFeaturesForElementsOfUtilityAssociations(containmentAssociations);

    else
      showAttachmentAndConnectivitySymbols(containmentAssociations);
  });

  // Connect error signals to message box
  connect(m_map, &Map::errorOccurred, this, [this](Error e)
  {
    setMessageBoxText("Map error: " + e.message() + " " + e.additionalMessage());
  });

  connect(m_mapView, &MapQuickView::errorOccurred, this, [this](Error e)
  {
    setMessageBoxText("MapView error: " + e.message() + " " + e.additionalMessage());
  });

  connect(m_utilityNetwork, &UtilityNetwork::errorOccurred, this, [this](Error e)
  {
    setMessageBoxText("Utility Network error occured: " + e.message() + " " + e.additionalMessage());
  });
}

void DisplayContentOfUtilityNetworkContainer::identifyFeaturesAtMouseClick(QMouseEvent& mouseEvent)
{
  if (m_map->loadStatus() != LoadStatus::Loaded || m_utilityNetwork->loadStatus() != LoadStatus::Loaded || !m_taskWatcher.isDone())
    return;

  constexpr double tolerance = 5;
  constexpr bool returnPopupsOnly = false;

  m_mapView->identifyLayers(mouseEvent.x(), mouseEvent.y(), tolerance, returnPopupsOnly);
}

void DisplayContentOfUtilityNetworkContainer::getUtilityAssociationsOfFeature(QUuid, QList<IdentifyLayerResult*> identifyResults)
{
  if (identifyResults.isEmpty())
    return;

  if (m_containerElement)
  {
    delete m_containerElement;
    m_containerElement = nullptr;
  }

  // Identify a feature and create a UtilityElement from it.
  for (IdentifyLayerResult* layerResult : identifyResults)
  {
    if (!m_containerElement && dynamic_cast<SubtypeFeatureLayer*>(layerResult->layerContent()))
    {
      for (const IdentifyLayerResult* sublayerResult : layerResult->sublayerResults())
      {
        for (GeoElement* geoElement : sublayerResult->geoElements())
        {
          if (ArcGISFeature* feature = dynamic_cast<ArcGISFeature*>(geoElement))
          {
            m_containerElement = m_utilityNetwork->createElementWithArcGISFeature(feature);
            if (m_containerElement)
            {
              // Queries for a list of all UtilityAssociation objects of containment association types present in the geodatabase for the m_containerElement.
              m_taskWatcher = m_utilityNetwork->associations(m_containerElement, UtilityAssociationType::Containment);
              return;
            }
          }
        }
      }
    }
  }
}

void DisplayContentOfUtilityNetworkContainer::getFeaturesForElementsOfUtilityAssociations(QList<UtilityAssociation*> containmentAssociations)
{
  // Create a list of elements representing the participants in the containment associations
  QList<UtilityElement*> contentElements;
  for (UtilityAssociation* association : containmentAssociations)
  {
    UtilityElement* otherElement = association->fromElement()->objectId() == m_containerElement->objectId() ? association->toElement() : association->fromElement();
    contentElements.append(otherElement);
  }

  if (!contentElements.isEmpty())
  {
    // Set visibility of all `OperationalLayers` to `false`
    setShowContainerView(true);

    // Get the features for the UtilityElements
    m_taskWatcher = m_utilityNetwork->featuresForElements(contentElements);
  }
}

void DisplayContentOfUtilityNetworkContainer::displayFeaturesAndGetAssociations(QUuid)
{
  // Display the features on the graphics overlay
  QList<Feature*> contentFeatures = m_utilityNetwork->featuresForElementsResult()->features();
  for (Feature* content : contentFeatures)
  {
    Symbol* symbol = dynamic_cast<ArcGISFeatureTable*>(content->featureTable())->layerInfo().drawingInfo().renderer(this)->symbol(content);
    m_containerGraphicsOverlay->graphics()->append(new Graphic(content->geometry(), symbol, this));
  }

  // Get the associations for each feature within the graphics overlay extent
  m_taskWatcher = m_utilityNetwork->associations(m_containerGraphicsOverlay->extent());
}

void DisplayContentOfUtilityNetworkContainer::showAttachmentAndConnectivitySymbols(QList<UtilityAssociation*> containmentAssociations)
{
  // Display the association lines on the graphics overlay
  for (UtilityAssociation* association : containmentAssociations)
  {
    Symbol* symbol = association->associationType() == UtilityAssociationType::Attachment ? m_attachmentSymbol : m_connectivitySymbol;
    m_containerGraphicsOverlay->graphics()->append(new Graphic(association->geometry(), symbol, this));
  }

  // If there are no associations, create a bounding box graphic using the viewpoint, otherwise use the extent of the graphics overlay
  if (m_containerGraphicsOverlay->graphics()->size() == 1 && m_containerGraphicsOverlay->graphics()->first()->geometry().geometryType() == GeometryType::Point)
  {
    m_mapView->setViewpointAndWait(Viewpoint(Point(m_containerGraphicsOverlay->graphics()->first()->geometry()), m_containerElement->assetType()->containerViewScale()));
    m_boundingBox = m_mapView->currentViewpoint(ViewpointType::BoundingGeometry).targetGeometry();
    m_mapView->setViewpointAndWait(m_previousViewpoint);

    setMessageBoxText("This feature contains no associations");
  }
  else
  {
    m_boundingBox = GeometryEngine::buffer(m_containerGraphicsOverlay->extent(), 0.05);
  }

  m_containerGraphicsOverlay->graphics()->append(new Graphic(m_boundingBox, m_boundingBoxSymbol, this));
  m_taskWatcher = m_mapView->setViewpoint(Viewpoint(GeometryEngine::buffer(m_containerGraphicsOverlay->extent(), 0.5)), .5);
}

bool DisplayContentOfUtilityNetworkContainer::showContainerView() const
{
  return m_showContainerView;
}

void DisplayContentOfUtilityNetworkContainer::setShowContainerView(bool showContainerView)
{
  m_showContainerView = showContainerView;

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

  emit showContainerViewChanged();
}

QString DisplayContentOfUtilityNetworkContainer::messageBoxText() const
{
  return m_messageBoxText;
}

void DisplayContentOfUtilityNetworkContainer::setMessageBoxText(const QString& message)
{
  m_messageBoxText = message;
  emit messageBoxTextChanged();
}

// Create attachment, connectivity, and bounding box legend
void DisplayContentOfUtilityNetworkContainer::createLegend()
{
  QQmlEngine* engine = QQmlEngine::contextForObject(this)->engine();
  m_symbolImageProvider = new SymbolImageProvider();
  engine->addImageProvider(SymbolImageProvider::imageProviderId(), m_symbolImageProvider);

  m_attachmentSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Dot, Qt::blue, 3, this);
  m_connectivitySymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Dot, Qt::red, 3, this);
  m_boundingBoxSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Dot, Qt::yellow, 3, this);

  connect(m_attachmentSymbol, &Symbol::createSwatchCompleted, this, [this](QUuid id, QImage image)
  {
    if (!m_symbolImageProvider)
      return;

    // convert the QUuid into a QString
    const QString imageId = id.toString().remove("{").remove("}");

    // add the image to the provider
    m_symbolImageProvider->addImage(imageId, image);

    // update the URL with the unique id
    m_attachmentSymbolUrl = QString("image://%1/%2").arg(SymbolImageProvider::imageProviderId(), imageId);

    // emit the signal to trigger the QML Image to update
    emit attachmentSymbolUrlChanged();
  });

  connect(m_connectivitySymbol, &Symbol::createSwatchCompleted, this, [this](QUuid id, QImage image)
  {
    if (!m_symbolImageProvider)
      return;

    // convert the QUuid into a QString
    const QString imageId = id.toString().remove("{").remove("}");

    // add the image to the provider
    m_symbolImageProvider->addImage(imageId, image);

    // update the URL with the unique id
    m_connectivitySymbolUrl = QString("image://%1/%2").arg(SymbolImageProvider::imageProviderId(), imageId);

    // emit the signal to trigger the QML Image to update
    emit connectivitySymbolUrlChanged();
  });

  connect(m_boundingBoxSymbol, &Symbol::createSwatchCompleted, this, [this](QUuid id, QImage image)
  {
    if (!m_symbolImageProvider)
      return;

    // convert the QUuid into a QString
    const QString imageId = id.toString().remove("{").remove("}");

    // add the image to the provider
    m_symbolImageProvider->addImage(imageId, image);

    // update the URL with the unique id
    m_boundingBoxSymbolUrl = QString("image://%1/%2").arg(SymbolImageProvider::imageProviderId(), imageId);

    // emit the signal to trigger the QML Image to update
    emit boundingBoxSymbolUrlChanged();
  });

  m_attachmentSymbol->createSwatch();
  m_connectivitySymbol->createSwatch();
  m_boundingBoxSymbol->createSwatch();
}

QString DisplayContentOfUtilityNetworkContainer::attachmentSymbolUrl() const
{
  return m_attachmentSymbolUrl;
}

QString DisplayContentOfUtilityNetworkContainer::connectivitySymbolUrl() const
{
  return m_connectivitySymbolUrl;
}

QString DisplayContentOfUtilityNetworkContainer::boundingBoxSymbolUrl() const
{
  return m_boundingBoxSymbolUrl;
}
