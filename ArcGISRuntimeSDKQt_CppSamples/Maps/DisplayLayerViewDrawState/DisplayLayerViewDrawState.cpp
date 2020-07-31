// [WriteFile Name=DisplayLayerViewDrawState, Category=Maps]
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

#include "DisplayLayerViewDrawState.h"

#include "FeatureLayer.h"
#include "ServiceFeatureTable.h"
#include "Map.h"
#include "MapQuickView.h"
#include "PortalItem.h"
#include "Viewpoint.h"
#include "Point.h"


using namespace Esri::ArcGISRuntime;

DisplayLayerViewDrawState::DisplayLayerViewDrawState(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::topographic(this), this))
{

  // create the feature table
//  ServiceFeatureTable* featureTable = new ServiceFeatureTable(QUrl("https://runtime.maps.arcgis.com/home/item.html?id=b8f4033069f141729ffb298b7418b653"), this);
  // create the feature layer using the feature table
//  m_featureLayer = new FeatureLayer(featureTable, this);

  // add the feature layer to the map
//  m_map->operationalLayers()->append(m_featureLayer);
}

DisplayLayerViewDrawState::~DisplayLayerViewDrawState() = default;

void DisplayLayerViewDrawState::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DisplayLayerViewDrawState>("Esri.Samples", 1, 0, "DisplayLayerViewDrawStateSample");
}

MapQuickView* DisplayLayerViewDrawState::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void DisplayLayerViewDrawState::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  connect(m_mapView, &MapQuickView::layerViewStateChanged, this, [this](Layer*, LayerViewState viewState)
  {
    m_viewStatuses.clear();
//    m_viewStatus.clear();

    // use insert to replace values mapped to layer name
    if (viewState.statusFlags() & Esri::ArcGISRuntime::LayerViewStatus::Active)
      m_viewStatuses.append("Active ");
    if (viewState.statusFlags() & Esri::ArcGISRuntime::LayerViewStatus::NotVisible)
      m_viewStatuses.append("NotVisible ");
    if (viewState.statusFlags() & Esri::ArcGISRuntime::LayerViewStatus::OutOfScale)
      m_viewStatuses.append("OutOfScale ");
    if (viewState.statusFlags() & Esri::ArcGISRuntime::LayerViewStatus::Loading)
      m_viewStatuses.append("Loading ");
    if (viewState.statusFlags() & Esri::ArcGISRuntime::LayerViewStatus::Error)
      m_viewStatuses.append("Error ");
    if (viewState.statusFlags() & Esri::ArcGISRuntime::LayerViewStatus::Warning)
      m_viewStatuses.append("Warning ");

    emit viewStatusChanged();
  });

  emit mapViewChanged();
}

void DisplayLayerViewDrawState::loadLayer()
{
  Portal* portal = new Portal(QUrl("https://runtime.maps.arcgis.com"), this);
  PortalItem* pI = new PortalItem(portal, "b8f4033069f141729ffb298b7418b653", this);
  m_featureLayer = new FeatureLayer(pI, 0, this);

  connect(m_featureLayer, &FeatureLayer::loadStatusChanged, this, [this] (LoadStatus loadStatus)
  {
    if (loadStatus == LoadStatus::Loading)
      m_loading = true;
    else
      m_loading = false;
    emit loadingChanged();

  });

  connect(m_featureLayer, &FeatureLayer::doneLoading, this, [this](Error e)
  {
    if (!e.isEmpty())
      return;
    const Point point{-11000000, 4500000, SpatialReference::webMercator()};
    Viewpoint vp{point, 40000000.0};
    m_mapView->setViewpoint(vp);

  });
  m_featureLayer->setMinScale(400000000.0);
  m_featureLayer->setMaxScale(400000000.0 / 10);

  m_map->operationalLayers()->append(m_featureLayer);
}

void DisplayLayerViewDrawState::changeFeatureLayerVisibility(bool visible)
{
  if (m_featureLayer->loadStatus() == LoadStatus::Loaded)
    m_featureLayer->setVisible(visible);
}
