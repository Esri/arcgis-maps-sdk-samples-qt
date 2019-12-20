// [WriteFile Name=MapReferenceScale, Category=Maps]
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "MapReferenceScale.h"

#include "Portal.h"
#include "PortalItem.h"
#include "Map.h"
#include "Layer.h"
#include "FeatureLayer.h"
#include "LayerListModel.h"
#include "MapQuickView.h"


using namespace Esri::ArcGISRuntime;


MapReferenceScale::MapReferenceScale(QObject* parent /* = nullptr */):
  QObject(parent),
  m_portal(new Portal(this)),
  m_portalItem(new PortalItem(m_portal, "3953413f3bd34e53a42bf70f2937a408", this))
{
  m_map = new Map(m_portalItem, this);

  // Once map is loaded set FeatureLayer list model
  connect(m_map, &Map::doneLoading, this, [this](Error loadError)
  {
    if (!loadError.isEmpty())
      return;

    m_layerInfoListModel = m_map->operationalLayers();
    emit layerInfoListModelChanged();
    emit currentMapScaleChanged();

    connect(m_mapView, &MapQuickView::mapScaleChanged, this, [this]()
    {
      emit currentMapScaleChanged();
    });
  });
}

MapReferenceScale::~MapReferenceScale() = default;

void MapReferenceScale::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<MapReferenceScale>("Esri.Samples", 1, 0, "MapReferenceScaleSample");
  qmlRegisterUncreatableType<QAbstractListModel>("Esri.Samples", 1, 0, "AbstractListModel", "AbstractListModel is uncreateable");
}


double MapReferenceScale::currentMapScale() const
{
  if (!m_mapView)
    return 0.0;

  return m_mapView->mapScale();
}

void MapReferenceScale::setCurrentMapScale(double scale)
{
  if(!m_map)
    return;

  m_map->setReferenceScale(scale);
}

void MapReferenceScale::setMapScaleToReferenceScale(double scale)
{
  if(m_mapView)
    m_mapView->setViewpointScale(scale);
}

void MapReferenceScale::featureLayerScaleSymbols(const QString& layerName, bool checkedStatus)
{
  if(m_layerInfoListModel)
  {
    for(Layer* layer : *static_cast<LayerListModel*>(m_layerInfoListModel))
    {
      if(layer->name() == layerName)
      {
        FeatureLayer* featureLayer = static_cast<FeatureLayer*>(layer);
        if(featureLayer)
          featureLayer->setScaleSymbols(checkedStatus);
      }
    }
  }
}

MapQuickView* MapReferenceScale::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void MapReferenceScale::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}
