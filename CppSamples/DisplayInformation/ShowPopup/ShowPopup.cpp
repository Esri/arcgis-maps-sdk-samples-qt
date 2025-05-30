// [WriteFile Name=ShowPopup, Category=DisplayInformation]
// [Legal]
// Copyright 2020 Esri.
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
#include "ShowPopup.h"

// ArcGIS Maps SDK headers
#include "Error.h"
#include "Feature.h"
#include "FeatureLayer.h"
#include "IdentifyLayerResult.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Popup.h"

// Qt headers
#include <QUuid>

using namespace Esri::ArcGISRuntime;

ShowPopup::ShowPopup(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(QUrl("https://runtime.maps.arcgis.com/home/webmap/viewer.html?webmap=e4c6eb667e6c43b896691f10cc2f1580"), this))
{
}

ShowPopup::~ShowPopup() = default;

void ShowPopup::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ShowPopup>("Esri.Samples", 1, 0, "ShowPopupSample");
}

MapQuickView* ShowPopup::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ShowPopup::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  // once map is set, connect to MapQuickView mouse clicked signal
  connect(m_mapView, &MapQuickView::mouseClicked, this, &ShowPopup::onMouseClicked);

  emit mapViewChanged();
}

void ShowPopup::onIdentifyLayerCompleted(const QUuid&, IdentifyLayerResult* rawIdentifyResult)
{
  emit taskRunningChanged();
  auto identifyResult = std::unique_ptr<IdentifyLayerResult>(rawIdentifyResult);

  // Invalid identify result
  if (!identifyResult)
    return;

  if (!identifyResult->error().isEmpty())
  {
    qDebug() << "Identify error occurred: " << identifyResult->error().message();
    return;
  }

  m_featureLayer->clearSelection();
  if (!identifyResult->popups().isEmpty())
  {
    GeoElement* geoElement = identifyResult->geoElements().first();
    Feature* feature = static_cast<Feature*>(geoElement);
    m_featureLayer->selectFeature(feature);

    m_popup = new Popup(geoElement, this);
    emit popupChanged();
  }
}

void ShowPopup::onMouseClicked(QMouseEvent& mouseEvent)
{
  if (m_map->loadStatus() != LoadStatus::Loaded)
    return;

  Layer* layer = m_map->operationalLayers()->at(0);
  if (layer->layerType() == LayerType::FeatureLayer)
  {
    m_featureLayer = static_cast<FeatureLayer*>(layer);
  }

  constexpr double tolerance = 12;
  constexpr bool returnPopupsOnly = false;
  constexpr int maximumResults = 10;

  m_future = m_mapView->identifyLayerAsync(m_featureLayer, mouseEvent.position(), tolerance, returnPopupsOnly, maximumResults);
  m_future.then(this, [this](IdentifyLayerResult* result)
  {
    onIdentifyLayerCompleted(QUuid(),result);
  });

  if (!m_future.isValid())
    qWarning() << "Future not valid.";

  emit taskRunningChanged();
}

bool ShowPopup::taskRunning() const
{
  return m_future.isRunning();
}

Esri::ArcGISRuntime::Popup* ShowPopup::popup()
{
  return m_popup;
}

void ShowPopup::clearSelection() const
{
  m_featureLayer->clearSelection();
}
