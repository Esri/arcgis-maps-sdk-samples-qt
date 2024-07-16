// [WriteFile Name=FeatureLayerSelection, Category=Features]
// [Legal]
// Copyright 2022 Esri.
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

#include "FeatureLayerSelection.h"

#include "Envelope.h"
#include "Feature.h"
#include "FeatureLayer.h"
#include "IdentifyLayerResult.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "ServiceFeatureTable.h"
#include "SpatialReference.h"
#include "Viewpoint.h"

#include <QFuture>
#include <QUuid>
#include <memory>

using namespace Esri::ArcGISRuntime;

FeatureLayerSelection::FeatureLayerSelection(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISLightGray, this))
{
  m_map->setInitialViewpoint(Viewpoint(Envelope(-6603299.491810, 1679677.742046, 9002253.947487, 8691318.054732, SpatialReference::webMercator())));

  m_featureTable = new ServiceFeatureTable(QUrl("https://services1.arcgis.com/4yjifSiIG17X0gW4/arcgis/rest/services/GDP_per_capita_1960_2016/FeatureServer/0"), this);
  m_featureLayer = new FeatureLayer(m_featureTable, this);
  m_map->operationalLayers()->append(m_featureLayer);
}

FeatureLayerSelection::~FeatureLayerSelection() = default;

void FeatureLayerSelection::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<FeatureLayerSelection>("Esri.Samples", 1, 0, "FeatureLayerSelectionSample");
}

MapQuickView* FeatureLayerSelection::mapView() const
{
  return m_mapView;
}

QString FeatureLayerSelection::selectedFeatureText() const
{
  return m_selectedFeatureText;
}

// Set the view (created in QML)
void FeatureLayerSelection::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  connectSignals();

  emit mapViewChanged();
}

//! [identify feature layer qml api snippet]
void FeatureLayerSelection::connectSignals()
{
  // lambda expression for the mouse press event on the mapview... do an identify operation
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    constexpr double tolerance = 22.0;
    constexpr bool returnPopupsOnly = false;
    constexpr int maximumResults = 1000;
    m_mapView->identifyLayerAsync(m_featureLayer, mouseEvent.position(), tolerance, returnPopupsOnly, maximumResults).then(this,
    [this](IdentifyLayerResult* result)
    {
      onIdentifyLayerCompleted_(result);
    });
  });
}

void FeatureLayerSelection::onIdentifyLayerCompleted_(IdentifyLayerResult* result)
{
  auto identifyResult = std::unique_ptr<IdentifyLayerResult>(result);

  if (!identifyResult)
    return;

  // clear any existing selection
  m_featureLayer->clearSelection();

  // create a list to store the identified elements
  QList<Feature*> identifiedFeatures;
  for (int i = 0; i < identifyResult->geoElements().size(); i++)
  {
    GeoElement* element = identifyResult->geoElements().at(i);
    if (nullptr != element)
    {
      // add the element to the list and take ownership of it.
      Feature* feature = static_cast<Feature*>(element);
      feature->setParent(this);
      identifiedFeatures.append(feature);
    }
  }

  // select the identified features
  m_featureLayer->selectFeatures(identifiedFeatures);
  // update the member with the number of selected features
  const qsizetype count = identifiedFeatures.length();
  m_selectedFeatureText = count > 1 ? QString::number(count) + " features selected." : QString::number(count) + " feature selected.";
  emit selectedFeatureTextChanged();
}
//! [identify feature layer qml api snippet]
