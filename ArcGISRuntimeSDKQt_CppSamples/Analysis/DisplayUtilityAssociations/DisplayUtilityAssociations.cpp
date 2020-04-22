// [WriteFile Name=DisplayUtilityAssociations, Category=Analysis]
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

#include "DisplayUtilityAssociations.h"

#include "ArcGISFeatureTable.h"
#include "AttributeListModel.h"
#include "FeatureLayer.h"
#include "GraphicsOverlay.h"
#include "Map.h"
#include "MapQuickView.h"
#include "SimpleLineSymbol.h"
#include "UniqueValueRenderer.h"
#include "UtilityAssociation.h"
#include "UtilityNetwork.h"
#include "UtilityNetworkDefinition.h"
#include "UtilityNetworkSource.h"
#include "UtilityNetworkTypes.h"

#include <QList>
#include <QVariant>
#include <QMetaEnum>
#include <QImage>
#include <QMap>

using namespace Esri::ArcGISRuntime;

namespace
{
const QString featureServerUrl("https://sampleserver7.arcgisonline.com/arcgis/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer");
const int maxScale = 2000;
}

DisplayUtilityAssociations::DisplayUtilityAssociations(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::topographicVector(this), this)),
  m_associationsOverlay(new GraphicsOverlay(this))
{
  m_utilityNetwork = new UtilityNetwork(featureServerUrl, this);

  // create symbols for the associations
  m_attachmentSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Dot, Qt::green, 5, this);
  m_connectivitySymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Dot, Qt::red, 5, this);

  connect(m_utilityNetwork, &UtilityNetwork::doneLoading, this, [this](const Error& error)
  {
    if (!error.isEmpty())
    {
      qDebug() << error.message() << error.additionalMessage();
      return;
    }

    // get all the edges and junctions in the network
    QList<UtilityNetworkSource*> edges;
    QList<UtilityNetworkSource*> junctions;

    for (UtilityNetworkSource* networkSource : m_utilityNetwork->definition()->networkSources())
    {
      if (networkSource->sourceType() == UtilityNetworkSourceType::Edge)
      {
        edges.append(networkSource);
      }
      else if (networkSource->sourceType() == UtilityNetworkSourceType::Junction)
      {
        junctions.append(networkSource);
      }
    }

    // Add all edges that are not subnet lines to the map.
    for (UtilityNetworkSource* source : edges)
    {
      if (source->sourceUsageType() != UtilityNetworkSourceUsageType::SubnetLine && source->featureTable() != nullptr)
      {
        m_map->operationalLayers()->append(new FeatureLayer(source->featureTable(), this));
      }
    }

    // Add all junctions to the map.
    for (UtilityNetworkSource* source : junctions)
    {
      if (source->featureTable())
      {
        m_map->operationalLayers()->append(new FeatureLayer(source->featureTable(), this));
      }
    }

    // create a renderer for the associations
//    UniqueValue* attachmentValue = new UniqueValue("Attachment", "", QVariantList{static_cast<int>(UtilityAssociationType::Attachment)}, m_attachmentSymbol, this);
//    UniqueValue* connectivityValue = new UniqueValue("Connection", "", QVariantList{static_cast<int>(UtilityAssociationType::Connectivity)}, m_connectivitySymbol, this);
    UniqueValue* attachmentValue = new UniqueValue("Attachment", "", QVariantList{QString::number(static_cast<int>(UtilityAssociationType::Attachment))}, m_attachmentSymbol, this);
    UniqueValue* connectivityValue = new UniqueValue("Connectivity", "", QVariantList{QString::number(static_cast<int>(UtilityAssociationType::Connectivity))}, m_connectivitySymbol, this);
    UniqueValueRenderer* uniqueValueRenderer = new UniqueValueRenderer("", nullptr, QList<QString>{"AssociationType"}, QList<UniqueValue*>{attachmentValue, connectivityValue}, this);
    m_associationsOverlay->setRenderer(uniqueValueRenderer);

    // populate the legend
    m_attachmentSymbol->createSwatch();

    addAssociations();
  });

  connect(m_attachmentSymbol, &Symbol::createSwatchCompleted, this, [this](QUuid, QImage image)
  {
    // save url to file and pass to member variable
  });
}

void DisplayUtilityAssociations::addAssociations()
{
  qDebug("add associations");

  // check if current viewpoint is outside the max scale
  if(m_mapView->currentViewpoint(ViewpointType::CenterAndScale).targetScale() >= maxScale)
  {
    return;
  }

  // check if current viewpoint has a valid extent
  Envelope extent = m_mapView->currentViewpoint(ViewpointType::BoundingGeometry).targetGeometry().extent();
  if (!extent.isValid())
  {
    qDebug("extent not valid");
    return;
  }

  connect(m_utilityNetwork, &UtilityNetwork::associationsCompleted, this, [this](QUuid, const QList<UtilityAssociation*>& associations)
  {
    for (UtilityAssociation* association : associations)
    {
      // check if the graphics overlay already contains the association
      for (Graphic* graphic : *m_associationsOverlay->graphics())
      {
        if (graphic->attributes()->containsAttribute("GlobalId") && graphic->attributes()->operator[]("GlobalId") == association->globalId())
        {
          qDebug("matched");
          continue;
        }
      }

      // add a graphic for the association
      QVariantMap graphicAttributes;
      graphicAttributes["GlobalId"] = association->globalId();
      graphicAttributes["AssociationType"] = QString::number(static_cast<int>(association->associationType()));
      Graphic* graphic = new Graphic(association->geometry(), graphicAttributes, this);
      qDebug() << graphicAttributes["GlobalId"];
      qDebug() << association->globalId();

      if (graphic->attributes()->operator[]("GlobalId") == association->globalId())
      {
        qDebug("worked");
      }
      else {
        qDebug("didn't work");
      }

      m_associationsOverlay->graphics()->append(graphic);
    }
    qDebug("done");
    qDebug() << m_associationsOverlay->graphics()->size();
  });

  // get all the associations in the extent of the viewpoint
  m_utilityNetwork->associations(extent);

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
  //Viewpoint InitialViewpoint = new Viewpoint(new MapPoint(-9812698.37297436, 5131928.33743317, SpatialReferences.WebMercator), 22d);

  m_mapView->setViewpoint(Viewpoint(Point(-9812698.37297436, 5131928.33743317, SpatialReference::webMercator()), 22));
  m_mapView->graphicsOverlays()->append(m_associationsOverlay);

  m_utilityNetwork->load();

  emit mapViewChanged();
}
