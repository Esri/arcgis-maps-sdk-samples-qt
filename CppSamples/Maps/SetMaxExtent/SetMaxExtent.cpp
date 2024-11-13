// [WriteFile Name=SetMaxExtent, Category=Maps]
// [Legal]
// Copyright 2022 Esri.

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

#include "SetMaxExtent.h"

#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "Map.h"
#include "MapQuickView.h"
#include "SimpleLineSymbol.h"
#include "MapTypes.h"
#include "GraphicsOverlayListModel.h"
#include "SymbolTypes.h"
#include "GraphicListModel.h"
#include "SpatialReference.h"
#include "Envelope.h"

#include <QFuture>

using namespace Esri::ArcGISRuntime;

SetMaxExtent::SetMaxExtent(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISStreets, this))
{
}

SetMaxExtent::~SetMaxExtent() = default;

void SetMaxExtent::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<SetMaxExtent>("Esri.Samples", 1, 0, "SetMaxExtentSample");
}

MapQuickView* SetMaxExtent::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void SetMaxExtent::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_coloradoOverlay = new GraphicsOverlay(this);
  // Create an envelope to be used as the Colorado geometry
  m_coloradoEnvelope = Envelope(-12138232, 4441198, -11362327, 5012861, SpatialReference(3857));

  m_mapView = mapView;
  m_mapView->setMap(m_map);
  m_mapView->graphicsOverlays()->append(m_coloradoOverlay);
  m_map->setMaxExtent(m_coloradoEnvelope);
  createExtentBorder();

  emit mapViewChanged();
}

void SetMaxExtent::createExtentBorder()
{
  m_coloradoOutline = new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, QColor("red"), 3.0f, this);
  m_coloradoGraphic = new Graphic(m_coloradoEnvelope, m_coloradoOutline, this);
  m_coloradoOverlay->graphics()->append(m_coloradoGraphic);
}

void SetMaxExtent::toggleMaxExtent()
{
  m_maxExtentEnabled ? m_maxExtentEnabled = false : m_maxExtentEnabled = true;

  if (!m_maxExtentEnabled) {
    m_map->setMaxExtent(Envelope{});
  }
  else {
    m_map->setMaxExtent(m_coloradoEnvelope);
    m_mapView->setViewpointGeometryAsync(m_coloradoEnvelope, 50);
  }
}
