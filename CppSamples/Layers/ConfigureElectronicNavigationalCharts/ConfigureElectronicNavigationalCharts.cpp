// [WriteFile Name=ConfigureElectronicNavigationalCharts, Category=Layers]
// [Legal]
// Copyright 2025 Esri.
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
#include "ConfigureElectronicNavigationalCharts.h"

// ArcGIS Maps SDK headers
#include "CalloutData.h"
#include "EncCell.h"
#include "EncDataset.h"
#include "EncDisplaySettings.h"
#include "EncEnvironmentSettings.h"
#include "EncExchangeSet.h"
#include "EncFeature.h"
#include "EncLayer.h"
#include "EncMarinerSettings.h"
#include "EncTextGroupVisibilitySettings.h"
#include "EncViewingGroupSettings.h"
#include "Envelope.h"
#include "Error.h"
#include "ErrorException.h"
#include "GeometryEngine.h"
#include "HydrographyTypes.h"
#include "IdentifyLayerResult.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "Viewpoint.h"

// Qt headers
#include <QDebug>
#include <QFuture>
#include <QMouseEvent>
#include <QStandardPaths>
#include <QtCore/qglobal.h>

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
  QString defaultDataPath()
  {
    QString dataPath;

#ifdef Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
    dataPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif

    return dataPath;
  }
} // namespace

ConfigureElectronicNavigationalCharts::ConfigureElectronicNavigationalCharts(QObject* parent /* = nullptr */) :
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISOceans, this)),
  m_colorScheme(EncColorScheme::Day),
  m_areaSymbolizationType(EncAreaSymbolizationType::Plain),
  m_pointSymbolizationType(EncPointSymbolizationType::PaperChart)
{
  // Set resource path
  EncEnvironmentSettings::setResourcePath(defaultDataPath() + "/ArcGIS/Runtime/Data/ENC/hydrography");

  // Apply initial display settings
  updateDisplaySettings();

  // Create ENC Exchange Set
  QStringList encPaths = {defaultDataPath() + "/ArcGIS/Runtime/Data/ENC/ExchangeSetwithoutUpdates/ENC_ROOT/CATALOG.031"};
  m_encExchangeSet = new EncExchangeSet(encPaths, this);
}

ConfigureElectronicNavigationalCharts::~ConfigureElectronicNavigationalCharts()
{
  // Reset settings to avoid affecting other samples.
  EncDisplaySettings* globalDisplaySettings = EncEnvironmentSettings::displaySettings();
  globalDisplaySettings->marinerSettings()->resetToDefaults();
  globalDisplaySettings->viewingGroupSettings()->resetToDefaults();
  globalDisplaySettings->textGroupVisibilitySettings()->resetToDefaults();
}

void ConfigureElectronicNavigationalCharts::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ConfigureElectronicNavigationalCharts>("Esri.Samples", 1, 0, "ConfigureElectronicNavigationalChartsSample");
}

MapQuickView* ConfigureElectronicNavigationalCharts::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ConfigureElectronicNavigationalCharts::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  // Initialize callout data (following ShowCallout pattern)
  m_mapView->calloutData()->setVisible(false);
  m_mapView->calloutData()->setTitle("ENC Feature");

  // Connect to mouse clicked events for feature identification
  connect(m_mapView, &MapQuickView::mouseClicked, this, &ConfigureElectronicNavigationalCharts::onGeoViewTapped);

  // Connect to doneLoading signal of EncExchangeSet
  connect(m_encExchangeSet, &EncExchangeSet::doneLoading, this, [this](const Error& error)
  {
    if (!error.isEmpty())
    {
      return;
    }

    // Loop through datasets
    const QList<EncDataset*> datasets = m_encExchangeSet->datasets();
    for (EncDataset* dataset : datasets)
    {
      // Create an EncCell from each dataset
      EncCell* cell = new EncCell(dataset, this);

      // Create an EncLayer from each cell
      EncLayer* encLayer = new EncLayer(cell, this);

      // Connect to the doneLoading signal of the layer
      connect(encLayer, &EncLayer::doneLoading, this, [this, encLayer](const Error& error)
      {
        if (!error.isEmpty())
        {
          return;
        }

        m_extents << encLayer->fullExtent();

        if (m_extents.length() != m_encExchangeSet->datasets().length())
        {
          return;
        }

        // Combine the extents
        Envelope fullExtent = GeometryEngine::combineExtents(m_extents);
        m_mapView->setViewpointAsync(Viewpoint{fullExtent});
      });

      // Add the layer to the map
      m_map->operationalLayers()->append(encLayer);
    }
  });

  // Load the EncExchangeSet
  m_encExchangeSet->load();

  emit mapViewChanged();
}

EncColorScheme ConfigureElectronicNavigationalCharts::colorScheme() const
{
  return m_colorScheme;
}

void ConfigureElectronicNavigationalCharts::setColorScheme(EncColorScheme colorScheme)
{
  if (m_colorScheme == colorScheme)
  {
    return;
  }
  m_colorScheme = colorScheme;
  updateDisplaySettings();
  emit colorSchemeChanged();
}

EncAreaSymbolizationType ConfigureElectronicNavigationalCharts::areaSymbolizationType() const
{
  return m_areaSymbolizationType;
}

void ConfigureElectronicNavigationalCharts::setAreaSymbolizationType(EncAreaSymbolizationType areaSymbolizationType)
{
  if (m_areaSymbolizationType == areaSymbolizationType)
  {
    return;
  }
  m_areaSymbolizationType = areaSymbolizationType;
  updateDisplaySettings();
  emit areaSymbolizationTypeChanged();
}

EncPointSymbolizationType ConfigureElectronicNavigationalCharts::pointSymbolizationType() const
{
  return m_pointSymbolizationType;
}

void ConfigureElectronicNavigationalCharts::setPointSymbolizationType(EncPointSymbolizationType pointSymbolizationType)
{
  if (m_pointSymbolizationType == pointSymbolizationType)
  {
    return;
  }
  m_pointSymbolizationType = pointSymbolizationType;
  updateDisplaySettings();
  emit pointSymbolizationTypeChanged();
}

void ConfigureElectronicNavigationalCharts::updateDisplaySettings()
{
  // Hold a reference to the application-wide ENC Display Settings
  EncDisplaySettings* globalDisplaySettings = EncEnvironmentSettings::displaySettings();

  // Hold a reference to the application-wide ENC Mariner Settings (part of display settings)
  EncMarinerSettings* globalMarinerSettings = globalDisplaySettings->marinerSettings();

  // Directly apply stored enum values
  globalMarinerSettings->setColorScheme(m_colorScheme);
  globalMarinerSettings->setAreaSymbolizationType(m_areaSymbolizationType);
  globalMarinerSettings->setPointSymbolizationType(m_pointSymbolizationType);
}

void ConfigureElectronicNavigationalCharts::clearAllSelections()
{
  if (!m_mapView)
  {
    return;
  }

  // For each layer in the operational layers that is an ENC layer
  LayerListModel* operationalLayers = m_map->operationalLayers();
  for (QObject* obj : *operationalLayers)
  {
    if (EncLayer* encLayer = qobject_cast<EncLayer*>(obj); encLayer)
    {
      encLayer->clearSelection();
    }
  }

  // Hide the callout
  m_mapView->calloutData()->setVisible(false);
}

void ConfigureElectronicNavigationalCharts::onGeoViewTapped(QMouseEvent& mouseEvent)
{
  if (!m_mapView)
  {
    return;
  }

  // Set callout position for new location
  const Point mapPoint = m_mapView->screenToLocation(mouseEvent.position().x(), mouseEvent.position().y());
  m_mapView->calloutData()->setLocation(mapPoint);

  // Clear existing selections
  clearAllSelections();

  // Perform the identify operation on ENC layers specifically
  const double tolerance = 12.0;
  const bool returnPopupsOnly = false;

  m_mapView->identifyLayersAsync(mouseEvent.position(), tolerance, returnPopupsOnly)
    .then(this,
          [this](const QList<IdentifyLayerResult*>& results)
  {
    // Filter to get only ENC layer results
    QList<IdentifyLayerResult*> encResults;
    for (IdentifyLayerResult* result : results)
    {
      if (qobject_cast<EncLayer*>(result->layerContent()))
      {
        encResults.append(result);
      }
    }

    // Return early if no ENC results
    if (encResults.isEmpty())
    {
      m_mapView->calloutData()->setVisible(false);
      return;
    }

    // Get the first ENC result
    IdentifyLayerResult* firstResult = encResults.first();
    EncLayer* containingLayer = qobject_cast<EncLayer*>(firstResult->layerContent());

    // Get the first ENC feature from the result
    const QList<GeoElement*> geoElements = firstResult->geoElements();
    if (geoElements.isEmpty())
    {
      m_mapView->calloutData()->setVisible(false);
      return;
    }

    if (EncFeature* encFeature = dynamic_cast<EncFeature*>(geoElements.first()); encFeature)
    {
      // Select the feature
      containingLayer->selectFeature(encFeature);

      // Create callout content
      QString title = encFeature->acronym().isEmpty() ? "ENC Feature" : encFeature->acronym();
      QString detail = encFeature->description().isEmpty() ? "ENC Chart Feature" : encFeature->description();

      // Show callout
      m_mapView->calloutData()->setTitle(title);
      m_mapView->calloutData()->setDetail(detail);
      m_mapView->calloutData()->setVisible(true);
    }
    else
    {
      m_mapView->calloutData()->setVisible(false);
    }
  })
    .onFailed(this, [this](const ErrorException& error)
  {
    // If identify fails, hide callout
    m_mapView->calloutData()->setVisible(false);
  });
}
