// [WriteFile Name=DisplayDrawingStatus, Category=Maps]
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

#include "DisplayDrawingStatus.h"

#include "Map.h"
#include "MapQuickView.h"
#include "FeatureLayer.h"
#include "ServiceFeatureTable.h"

#include <QUrl>

using namespace Esri::ArcGISRuntime;

DisplayDrawingStatus::DisplayDrawingStatus(QQuickItem* parent):
  QQuickItem(parent)
{
}

DisplayDrawingStatus::~DisplayDrawingStatus() = default;

void DisplayDrawingStatus::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DisplayDrawingStatus>("Esri.Samples", 1, 0, "DisplayDrawingStatusSample");
}

void DisplayDrawingStatus::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create a map using the topographic basemap
  m_map = new Map(Basemap::topographic(this), this);
  m_map->setInitialViewpoint(Viewpoint(Envelope(-13639984, 4537387, -13606734, 4558866, SpatialReference::webMercator())));

  // create feature layer
  ServiceFeatureTable* featureTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer/0"), this);
  m_featureLayer = new FeatureLayer(featureTable, this);

  // add the layer to the map
  m_map->operationalLayers()->append(m_featureLayer);

  // Set map to map view
  m_mapView->setMap(m_map);

  connect(m_mapView, &MapQuickView::drawStatusChanged, this, [this](DrawStatus drawStatus)
  {
    drawStatus == DrawStatus::InProgress ? m_mapDrawing = true : m_mapDrawing = false;
    emit mapDrawStatusChanged();
  });
}

bool DisplayDrawingStatus::mapDrawing() const
{
  return m_mapDrawing;
}
