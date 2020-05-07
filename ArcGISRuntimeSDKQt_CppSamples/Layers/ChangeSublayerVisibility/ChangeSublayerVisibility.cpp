// [WriteFile Name=ChangeSublayerVisibility, Category=Layers]
// [Legal]
// Copyright 2016 Esri.

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

#include "ChangeSublayerVisibility.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "ArcGISMapImageLayer.h"
#include "ArcGISSublayerListModel.h"
#include "Point.h"
#include "Viewpoint.h"
#include "SpatialReference.h"
#include <QUrl>

using namespace Esri::ArcGISRuntime;

ChangeSublayerVisibility::ChangeSublayerVisibility(QQuickItem* parent) :
  QQuickItem(parent)
{
}

ChangeSublayerVisibility::~ChangeSublayerVisibility() = default;

void ChangeSublayerVisibility::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ChangeSublayerVisibility>("Esri.Samples", 1, 0, "ChangeSublayerVisibilitySample");
  qmlRegisterUncreatableType<QAbstractListModel>("Esri.Samples", 1, 0, "AbstractListModel", "AbstractListModel is uncreateable");
}

void ChangeSublayerVisibility::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // create a new map instance
  m_map = new Map(Basemap::topographic(this), this);
  m_map->setInitialViewpoint(Viewpoint(Point(-11e6, 6e6, SpatialReference(102100)), 9e7));

  // add the map image layer
  m_mapImageLayer = new ArcGISMapImageLayer(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/SampleWorldCities/MapServer"), this);
  m_map->operationalLayers()->append(m_mapImageLayer);

  // set map on the map view
  m_mapView->setMap(m_map);

  // set the map image layer's sublayer list model to the Q_PROPERTY
  m_sublayerModel = m_mapImageLayer->mapImageSublayers();
  sublayerModelChanged();
}

QAbstractListModel* ChangeSublayerVisibility::sublayerModel()
{
  return m_sublayerModel;
}

