// [WriteFile Name=ControlTimeExtentTimeSlider, Category=Features]
// [Legal]
// Copyright 2021 Esri.
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
#include "ControlTimeExtentTimeSlider.h"

// C++ API headers
#include "FeatureLayer.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "ServiceFeatureTable.h"

using namespace Esri::ArcGISRuntime;

ControlTimeExtentTimeSlider::ControlTimeExtentTimeSlider(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISTopographic, this))
{
  ServiceFeatureTable* featureTable = new ServiceFeatureTable(QUrl("https://services5.arcgis.com/N82JbI5EYtAkuUKU/ArcGIS/rest/services/Hurricane_time_enabled_layer_2005_1_day/FeatureServer/0"), this);
  FeatureLayer* featureLayer = new FeatureLayer(featureTable, this);
  m_map->operationalLayers()->append(featureLayer);
}

ControlTimeExtentTimeSlider::~ControlTimeExtentTimeSlider() = default;

void ControlTimeExtentTimeSlider::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ControlTimeExtentTimeSlider>("Esri.Samples", 1, 0, "ControlTimeExtentTimeSliderSample");
}

MapQuickView* ControlTimeExtentTimeSlider::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ControlTimeExtentTimeSlider::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}
