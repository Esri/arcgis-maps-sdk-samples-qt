// [WriteFile Name=ShowPopup, Category=DisplayInformation]
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

#include "ShowPopup.h"

#include "FeatureLayer.h"
#include "Map.h"
#include "MapQuickView.h"
#include "PopupManager.h"

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
  // In order to use the PopupManager with a QML UI, the PopupManager class must be registered as a QML Type.
  qmlRegisterUncreatableType<PopupManager>("Esri.Samples", 1, 0, "PopupManager", "PopupManager is uncreateable");
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

  connect(m_mapView, &MapQuickView::mouseClicked, this, [this] (QMouseEvent& mouseEvent)
  {
    if (m_map->loadStatus() != LoadStatus::Loaded)
      return;

    m_layer = m_map->operationalLayers()->at(0);
    constexpr double tolerance = 12;
    constexpr bool returnPopupsOnly = false;

    m_taskWatcher = m_mapView->identifyLayer(m_layer, mouseEvent.x(), mouseEvent.y(), tolerance, returnPopupsOnly);

    if (!m_taskWatcher.isValid())
      qWarning() << "Task not valid.";

    emit taskRunningChanged();
  });

  connect(m_mapView, &MapQuickView::identifyLayerCompleted, this, [this](QUuid, Esri::ArcGISRuntime::IdentifyLayerResult* rawIdentifyResult)
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

    if (!identifyResult->popups().isEmpty())
    {
      m_popupManagers.clear();
      for (Popup* popup : identifyResult->popups())
      {
        PopupManager* popupManager = new PopupManager{popup, this};
        m_popupManagers.append(popupManager);
        if(m_layer->layerType() == LayerType::FeatureLayer)
        {
          FeatureLayer* featureLayer = static_cast<FeatureLayer*>(m_layer);
          featureLayer->clearSelection();
          featureLayer->selectFeature(static_cast<Feature*>(identifyResult->geoElements().at(0)));
        }
        emit popupManagersChanged();
      }
    }
  });

  emit mapViewChanged();
}

bool ShowPopup::taskRunning() const
{
  return !m_taskWatcher.isDone();
}

QQmlListProperty<PopupManager> ShowPopup::popupManagers()
{
  return QQmlListProperty<PopupManager>(this, m_popupManagers);
}

void ShowPopup::clearSelection() const
{
  if (m_layer->layerType() == LayerType::FeatureLayer)
  {
    FeatureLayer* featureLayer = static_cast<FeatureLayer*>(m_layer);
    featureLayer->clearSelection();
  }
}
