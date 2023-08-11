// [WriteFile Name=DisplayPointsUsingClusteringFeatureReduction, Category=DisplayInformation]
// [Legal]
// Copyright 2023 Esri.

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

#include "DisplayPointsUsingClusteringFeatureReduction.h"

#include "CalloutData.h"
#include "Error.h"
#include "FeatureLayer.h"
#include "FeatureReduction.h"
#include "FeatureTable.h"
#include "GeoElement.h"
#include "IdentifyLayerResult.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "Popup.h"
#include "PopupManager.h"
#include "PortalItem.h"

#include <QFuture>

using namespace Esri::ArcGISRuntime;

DisplayPointsUsingClusteringFeatureReduction::DisplayPointsUsingClusteringFeatureReduction(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(new PortalItem("8916d50c44c746c1aafae001552bad23", this), this))
{
  connect(m_map, &Map::doneLoading, this, [this](const Error& e)
  {
    if (!e.isEmpty())
    {
      qWarning() << e.message() << e.additionalMessage();
      return;
    }

    // Get the power plants feature layer for querying
    m_powerPlantsLayer = static_cast<FeatureLayer*>(m_map->operationalLayers()->first());
    m_taskRunning = false;
    emit taskRunningChanged();
  });
}

DisplayPointsUsingClusteringFeatureReduction::~DisplayPointsUsingClusteringFeatureReduction() = default;

void DisplayPointsUsingClusteringFeatureReduction::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DisplayPointsUsingClusteringFeatureReduction>("Esri.Samples", 1, 0, "DisplayPointsUsingClusteringFeatureReductionSample");
}

MapQuickView* DisplayPointsUsingClusteringFeatureReduction::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void DisplayPointsUsingClusteringFeatureReduction::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  connect(m_mapView, &MapQuickView::mouseClicked, this, &DisplayPointsUsingClusteringFeatureReduction::onMouseClicked);

  emit mapViewChanged();
}

void DisplayPointsUsingClusteringFeatureReduction::onMouseClicked(const QMouseEvent &mouseClick)
{
  if (m_taskRunning)
    return;

  m_taskRunning = true;
  emit taskRunningChanged();

  m_mapView->calloutData()->setVisible(false);

  // Clean up any children objects associated with this parent
  m_resultParent.reset(new QObject(this));

  m_mapView->identifyLayerAsync(m_powerPlantsLayer, mouseClick.position(), 3, false, m_resultParent.get())
      .then(this, [this](IdentifyLayerResult* identifyResult)
  {
    m_taskRunning = false;
    emit taskRunningChanged();

    // Invalid identify result
    if (!identifyResult)
      return;

    if (!identifyResult->error().isEmpty())
    {
      qDebug() << "Identify error occurred:" << identifyResult->error().message() << identifyResult->error().additionalMessage();
      return;
    }

    if (identifyResult->popups().isEmpty())
      return;


    Popup* popup = identifyResult->popups().constFirst();

    // Create a PopupManager with the IdentifyLayerResult's parent so it will get cleaned up as well.
    PopupManager* popupManager = new PopupManager(popup, identifyResult->parent());

    // Use the custom HTML description in the PopupManager to popuplate a Callout and display it.
    m_calloutText = popupManager->customHtmlDescription();
    m_mapView->calloutData()->setLocation(Point(popup->geoElement()->geometry()));
    m_mapView->calloutData()->setVisible(true);

    emit calloutTextChanged();
  });
}

void DisplayPointsUsingClusteringFeatureReduction::toggleClustering()
{
  if (m_map->loadStatus() != LoadStatus::Loaded)
    return;

  if (!m_powerPlantsLayer)
  {
    m_powerPlantsLayer = static_cast<FeatureLayer*>(m_map->operationalLayers()->first());

    // Check if the cast was successful
    if (!m_powerPlantsLayer)
      return;
  }

  m_powerPlantsLayer->featureReduction()->setEnabled(!m_powerPlantsLayer->featureReduction()->isEnabled());

  m_mapView->calloutData()->setVisible(false);
}

QString DisplayPointsUsingClusteringFeatureReduction::calloutText() const
{
  return m_calloutText;
}

bool DisplayPointsUsingClusteringFeatureReduction::taskRunning() const
{
  return m_taskRunning;
}
