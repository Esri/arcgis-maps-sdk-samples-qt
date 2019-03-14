// [WriteFile Name=MapReferenceScale, Category=Maps]
// [Legal]
// Copyright 2018 Esri.

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
  m_map(new Map(Basemap::imagery(this), this)),
  m_portal(new Portal(this)),
  m_portalItem(new PortalItem(m_portal, "3953413f3bd34e53a42bf70f2937a408", this))
{
  m_map = new Map(m_portalItem, this);

  //once map is loaded set FeatureLayer list model
  connect(m_map, &Map::doneLoading, this, [this](Error loadError)
  {
    if (!loadError.isEmpty())
        return;

    m_layerInfoListModel = m_map->operationalLayers();
    emit layerInfoListModelChanged();
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

double MapReferenceScale::currentMapScale() {
  return m_mapView->mapScale();
}

void MapReferenceScale::changeReferenceScale(const double& scale){
  m_map->setReferenceScale(scale);
}

void MapReferenceScale::setMapScaleToReferenceScale(const double& scale){
  m_mapView->setViewpointScale(scale);
}

void MapReferenceScale::featureLayerScaleSymbols(const QString& layerName, const bool& checkedStatus){
  LayerListModel* model = m_map->operationalLayers();
  for( Layer* layer : *model ) {
    if(layer->name() == layerName){
      FeatureLayer* featureLayer = static_cast<FeatureLayer*>(layer);
      featureLayer->setScaleSymbols(checkedStatus);
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
  {
    return;
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}
