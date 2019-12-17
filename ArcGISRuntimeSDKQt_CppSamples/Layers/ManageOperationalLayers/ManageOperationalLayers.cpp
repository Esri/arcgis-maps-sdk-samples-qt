// [WriteFile Name=ManageOperationalLayers, Category=Layers]
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

#include "ManageOperationalLayers.h"

#include "Map.h"
#include "MapQuickView.h"
#include "ArcGISMapImageLayer.h"
#include "DrawOrderLayerListModel.h"

using namespace Esri::ArcGISRuntime;

ManageOperationalLayers::ManageOperationalLayers(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::topographic(this), this)),
  m_elevationLayer(new ArcGISMapImageLayer(QUrl("https://sampleserver5.arcgisonline.com/arcgis/rest/services/Elevation/WorldElevations/MapServer"), this)),
  m_censusLayer(new ArcGISMapImageLayer(QUrl("https://sampleserver5.arcgisonline.com/arcgis/rest/services/Census/MapServer"), this)),
  m_damageLayer(new ArcGISMapImageLayer(QUrl("https://sampleserver5.arcgisonline.com/arcgis/rest/services/DamageAssessment/MapServer"), this)),
  m_drawOrderListModel(new DrawOrderLayerListModel(this))
{
  // add layers to the map
  m_map->operationalLayers()->append(m_elevationLayer);
  m_map->operationalLayers()->append(m_censusLayer);
  m_map->operationalLayers()->append(m_damageLayer);

  // Create a custom DrawOrderListModel. This is a QSortFilterProxyModel used to
  // "reverse" the order appearance so the top level layer displays at the top
  // of the list view instead of the bottom, which is the default behavior. This
  // will closer match the widely accepted standard table of contents UX where
  // the top level layer displays at the top of the view.
  m_drawOrderListModel->setLayerListModel(m_map->operationalLayers());
}

ManageOperationalLayers::~ManageOperationalLayers() = default;

void ManageOperationalLayers::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ManageOperationalLayers>("Esri.Samples", 1, 0, "ManageOperationalLayersSample");
  qmlRegisterUncreatableType<QAbstractItemModel>("Esri.Samples", 1, 0, "AbstractItemModel", "AbstractItemModel is uncreateable");
}

MapQuickView* ManageOperationalLayers::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ManageOperationalLayers::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}

QAbstractItemModel* ManageOperationalLayers::layerListModel() const
{
  return m_drawOrderListModel;
}

void ManageOperationalLayers::moveLayerUp(int index)
{
  if (!m_drawOrderListModel || !m_map)
    return;

  // get the index and move the layer up
  int modelIndex = m_drawOrderListModel->mappedIndex(index);
  m_map->operationalLayers()->move(modelIndex, modelIndex + 1);
}

void ManageOperationalLayers::moveLayerDown(int index)
{
  if (!m_drawOrderListModel || !m_map)
    return;

  // get the index and move the layer down
  int modelIndex = m_drawOrderListModel->mappedIndex(index);
  m_map->operationalLayers()->move(modelIndex, modelIndex - 1);
}

void ManageOperationalLayers::removeLayer(int index)
{
  if (!m_drawOrderListModel || !m_map)
    return;

  // get the index
  int modelIndex = m_drawOrderListModel->mappedIndex(index);
  Layer* lyr = m_map->operationalLayers()->at(modelIndex);

  // add the layer to the other list
  m_deletedLayers.append(lyr);
  m_deletedLayersListNames.append(lyr->name());
  deletedLayersListChanged();

  // remove from the map
  m_map->operationalLayers()->removeAt(modelIndex);
}

void ManageOperationalLayers::addLayer(int index)
{
  if (!m_drawOrderListModel || !m_map)
    return;

  // get the layer from the deleted list
  Layer* lyr = m_deletedLayers.at(index);
  if (!lyr)
    return;

  // remove the layer from the deleted list
  m_deletedLayers.removeAt(index);
  m_deletedLayersListNames.removeAt(index);
  deletedLayersListChanged();

  // add the layer to the map's operational layers
  m_map->operationalLayers()->append(lyr);
}
