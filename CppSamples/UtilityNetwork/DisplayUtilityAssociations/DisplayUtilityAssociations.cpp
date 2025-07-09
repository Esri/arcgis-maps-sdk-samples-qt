// [WriteFile Name=DisplayUtilityAssociations, Category=UtilityNetwork]
// [Legal]
// Copyright 2020 Esri.
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
#include "DisplayUtilityAssociations.h"
#include "SymbolImageProvider.h"

// ArcGIS Maps SDK headers
#include "ArcGISRuntimeEnvironment.h"
#include "Authentication/AuthenticationManager.h"
#include "Authentication/ArcGISAuthenticationChallenge.h"
#include "Authentication/TokenCredential.h"
#include "ArcGISFeatureTable.h"
#include "AttributeListModel.h"
#include "Envelope.h"
#include "ErrorException.h"
#include "FeatureLayer.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "SimpleLineSymbol.h"
#include "SpatialReference.h"
#include "SymbolTypes.h"
#include "UniqueValue.h"
#include "UniqueValueRenderer.h"
#include "UtilityAssociation.h"
#include "UtilityNetwork.h"
#include "UtilityNetworkDefinition.h"
#include "UtilityNetworkListModel.h"
#include "UtilityNetworkTypes.h"
#include "Viewpoint.h"

// Qt headers
#include <QFuture>
#include <QImage>
#include <QList>
#include <QQmlContext>
#include <QUuid>

using namespace Esri::ArcGISRuntime;
using namespace Esri::ArcGISRuntime::Authentication;

namespace
{
const int maxScale = 2000;
constexpr int targetScale = 50;
}

DisplayUtilityAssociations::DisplayUtilityAssociations(QObject* parent /* = nullptr */):
  ArcGISAuthenticationChallengeHandler(parent),
  m_map(new Map(QUrl("https://sampleserver7.arcgisonline.com/portal/home/item.html?id=be0e4637620a453584118107931f718b"), this)),
  m_associationsOverlay(new GraphicsOverlay(this)),
  m_attachmentSymbol(new SimpleLineSymbol(SimpleLineSymbolStyle::Dot, Qt::green, 5, this)),
  m_connectivitySymbol(new SimpleLineSymbol(SimpleLineSymbolStyle::Dot, Qt::red, 5, this))
{
  ArcGISRuntimeEnvironment::authenticationManager()->setArcGISAuthenticationChallengeHandler(this);

  connect(m_map, &Map::doneLoading, this, [this](const Error& error)
  {
    if (!error.isEmpty() || m_map->utilityNetworks()->isEmpty())
    {
      return;
    }

    m_utilityNetwork = m_map->utilityNetworks()->first();
    m_utilityNetwork->load();
    connectSignals();
  });
}

void DisplayUtilityAssociations::handleArcGISAuthenticationChallenge(ArcGISAuthenticationChallenge* challenge)
{
  TokenCredential::createWithChallengeAsync(challenge, "viewer01", "I68VGU^nMurF", {}, this).then(this, [challenge](TokenCredential* tokenCredential)
  {
    challenge->continueWithCredential(tokenCredential);
  }).onFailed(this, [challenge](const ErrorException& e)
  {
    challenge->continueWithError(e.error());
  });
}

void DisplayUtilityAssociations::addAssociations()
{
  // check if current viewpoint is outside the max scale
  if(m_mapView->currentViewpoint(ViewpointType::CenterAndScale).targetScale() >= maxScale)
    return;

  // check if current viewpoint has a valid extent
  const Envelope extent = m_mapView->currentViewpoint(ViewpointType::BoundingGeometry).targetGeometry().extent();
  if (!extent.isValid())
  {
    qDebug("Extent not valid");
    return;
  }

  // get all the associations in the extent of the viewpoint
  m_utilityNetwork->associationsAsync(extent).then(this, [this](const QList<UtilityAssociation*>& associations)
  {
    const GraphicListModel* graphics = m_associationsOverlay->graphics();

    for (UtilityAssociation* association : associations)
    {
      // check if the graphics overlay already contains the association
      const bool uniqueGraphic = std::none_of(graphics->begin(), graphics->end(), [association](const Graphic* graphic)
                                              {
                                                const AttributeListModel* attributes = graphic->attributes();
                                                return attributes->containsAttribute("GlobalId") && qvariant_cast<QUuid>((*graphic->attributes())["GlobalId"]) == association->globalId();
                                              });

      if (uniqueGraphic)
      {
        // add a graphic for the association
        QVariantMap graphicAttributes;
        graphicAttributes["GlobalId"] = association->globalId();
        graphicAttributes["AssociationType"] = static_cast<int>(association->associationType());
        Graphic* graphic = new Graphic(association->geometry(), graphicAttributes, this);

        m_associationsOverlay->graphics()->append(graphic);
      }
    }
  });
}

DisplayUtilityAssociations::~DisplayUtilityAssociations() = default;

void DisplayUtilityAssociations::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DisplayUtilityAssociations>("Esri.Samples", 1, 0, "DisplayUtilityAssociationsSample");
}

MapQuickView* DisplayUtilityAssociations::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void DisplayUtilityAssociations::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  m_mapView->graphicsOverlays()->append(m_associationsOverlay);

  // Get the image provider from the QML Engine
  QQmlEngine* engine = QQmlEngine::contextForObject(this)->engine();
  m_symbolImageProvider = new SymbolImageProvider();
  engine->addImageProvider(SymbolImageProvider::imageProviderId(), m_symbolImageProvider);

  connect(m_mapView, &MapQuickView::navigatingChanged, this, [this]()
  {
    if (!m_mapView->isNavigating())
      addAssociations();
  });

  emit mapViewChanged();
}

void DisplayUtilityAssociations::connectSignals()
{
  connect(m_utilityNetwork, &UtilityNetwork::doneLoading, this, [this](const Error& error)
  {
    if (!error.isEmpty())
    {
      qDebug() << error.message() << error.additionalMessage();
      return;
    }

    m_mapView->setViewpointAsync(Viewpoint(Point(-9812698.37297436, 5131928.33743317, SpatialReference::webMercator()), targetScale)).then(this, [this](bool succeeded)
    {
      if (!succeeded)
        return;

      addAssociations();
    });

    // create a renderer for the associations
    UniqueValue* attachmentValue = new UniqueValue("Attachment", "",
                                                   QVariantList{static_cast<int>(UtilityAssociationType::Attachment)}, m_attachmentSymbol, this);
    UniqueValue* connectivityValue = new UniqueValue("Connectivity", "",
                                                     QVariantList{static_cast<int>(UtilityAssociationType::Connectivity)}, m_connectivitySymbol, this);
    UniqueValueRenderer* uniqueValueRenderer = new UniqueValueRenderer("", nullptr,
                                                                       QStringList{"AssociationType"}, QList<UniqueValue*>{attachmentValue, connectivityValue}, this);
    m_associationsOverlay->setRenderer(uniqueValueRenderer);

    // populate the legend
    m_attachmentSymbol->createSwatchAsync().then(this, [this](const QImage& image)
    {
      if (!m_symbolImageProvider)
        return;

      const QString imageId = QUuid().createUuid().toString(QUuid::WithoutBraces);

      // add the image to the provider
      m_symbolImageProvider->addImage(imageId, image);

      // update the URL with the unique id
      m_attachmentSymbolUrl = QString("image://%1/%2").arg(SymbolImageProvider::imageProviderId(), imageId);

      // emit the signal to trigger the QML Image to update
      emit attachmentSymbolUrlChanged();
    });

    m_connectivitySymbol->createSwatchAsync().then(this, [this](const QImage& image)
    {
      if (!m_symbolImageProvider)
        return;

      const QString imageId = QUuid().createUuid().toString(QUuid::WithoutBraces);

      // add the image to the provider
      m_symbolImageProvider->addImage(imageId, image);

      // update the URL with the unique id
      m_connectivitySymbolUrl = QString("image://%1/%2").arg(SymbolImageProvider::imageProviderId(), imageId);

      // emit the signal to trigger the QML Image to update
      emit connectivitySymbolUrlChanged();
    });
  });
}

QString DisplayUtilityAssociations::attachmentSymbolUrl() const
{
  return m_attachmentSymbolUrl;
}

QString DisplayUtilityAssociations::connectivitySymbolUrl() const
{
  return m_connectivitySymbolUrl;
}
