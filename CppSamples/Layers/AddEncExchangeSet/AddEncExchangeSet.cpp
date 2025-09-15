// [WriteFile Name=AddEncExchangeSet, Category=Layers]
// [Legal]
// Copyright 2019 Esri.
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
#include "AddEncExchangeSet.h"

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
#include "HydrographyTypes.h"
#include "Envelope.h"
#include "Error.h"
#include "ErrorException.h"
#include "GeometryEngine.h"
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

AddEncExchangeSet::AddEncExchangeSet(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISOceans, this))
{
  // set resource path
  EncEnvironmentSettings::setResourcePath(defaultDataPath() + "/ArcGIS/Runtime/Data/ENC/hydrography");

  // Apply initial display settings
  updateDisplaySettings();

  // create ENC Exchange Set
  QStringList encPaths = { defaultDataPath() + "/ArcGIS/Runtime/Data/ENC/ExchangeSetwithoutUpdates/ENC_ROOT/CATALOG.031" };
  m_encExchangeSet = new EncExchangeSet(encPaths, this);
}

AddEncExchangeSet::~AddEncExchangeSet()
{
  // ENC environment settings apply to the entire application.
  // They need to be reset after leaving the sample to avoid affecting other samples.
  EncDisplaySettings* globalDisplaySettings = EncEnvironmentSettings::displaySettings();
  globalDisplaySettings->marinerSettings()->resetToDefaults();
  globalDisplaySettings->viewingGroupSettings()->resetToDefaults();
  globalDisplaySettings->textGroupVisibilitySettings()->resetToDefaults();
}

void AddEncExchangeSet::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<AddEncExchangeSet>("Esri.Samples", 1, 0, "AddEncExchangeSetSample");
}

MapQuickView* AddEncExchangeSet::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void AddEncExchangeSet::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  // Initialize callout data (following ShowCallout pattern)
  m_mapView->calloutData()->setVisible(false);
  m_mapView->calloutData()->setTitle("ENC Feature");

  // Connect to mouse clicked events for feature identification
  connect(m_mapView, &MapQuickView::mouseClicked, this, &AddEncExchangeSet::onGeoViewTapped);

  // connect to doneLoading signal of EncExchangeSet
  connect(m_encExchangeSet, &EncExchangeSet::doneLoading, this, [this](const Error& e)
  {
    if (!e.isEmpty())
      return;

    // loop through datasets
    const auto datasets = m_encExchangeSet->datasets();
    for (EncDataset* dataset : datasets)
    {
      // create an EncCell from each dataset
      EncCell* cell = new EncCell(dataset, this);

      // create an EncLayer from each cell
      EncLayer* encLayer = new EncLayer(cell, this);

      // connect to the doneLoading signal of the layer
      connect(encLayer, &EncLayer::doneLoading, this, [this, encLayer](const Error& e)
      {
        if (!e.isEmpty())
          return;

        m_extents << encLayer->fullExtent();

        if (m_extents.length() != m_encExchangeSet->datasets().length())
          return;

        // combine the extents
        Envelope fullExtent = GeometryEngine::combineExtents(m_extents);
        m_mapView->setViewpointAsync(Viewpoint{fullExtent});
      });

      // add the layer to the map
      m_map->operationalLayers()->append(encLayer);
    }
  });

  // load the EncExchangeSet
  m_encExchangeSet->load();

  emit mapViewChanged();
}

QString AddEncExchangeSet::colorScheme() const
{
  return m_colorScheme;
}

void AddEncExchangeSet::setColorScheme(const QString& colorScheme)
{
  if (m_colorScheme == colorScheme)
    return;

  m_colorScheme = colorScheme;
  updateDisplaySettings();
  emit colorSchemeChanged();
}

QString AddEncExchangeSet::areaSymbolizationType() const
{
  return m_areaSymbolizationType;
}

void AddEncExchangeSet::setAreaSymbolizationType(const QString& areaSymbolizationType)
{
  if (m_areaSymbolizationType == areaSymbolizationType)
    return;

  m_areaSymbolizationType = areaSymbolizationType;
  updateDisplaySettings();
  emit areaSymbolizationTypeChanged();
}

QString AddEncExchangeSet::pointSymbolizationType() const
{
  return m_pointSymbolizationType;
}

void AddEncExchangeSet::setPointSymbolizationType(const QString& pointSymbolizationType)
{
  if (m_pointSymbolizationType == pointSymbolizationType)
    return;

  m_pointSymbolizationType = pointSymbolizationType;
  updateDisplaySettings();
  emit pointSymbolizationTypeChanged();
}

void AddEncExchangeSet::updateDisplaySettings()
{
  // Hold a reference to the application-wide ENC Display Settings
  EncDisplaySettings* globalDisplaySettings = EncEnvironmentSettings::displaySettings();
  
  // Hold a reference to the application-wide ENC Mariner Settings (part of display settings)
  EncMarinerSettings* globalMarinerSettings = globalDisplaySettings->marinerSettings();

  // Apply color scheme
  if (m_colorScheme == "Day")
    globalMarinerSettings->setColorScheme(Esri::ArcGISRuntime::EncColorScheme::Day);
  else if (m_colorScheme == "Dusk")
    globalMarinerSettings->setColorScheme(Esri::ArcGISRuntime::EncColorScheme::Dusk);
  else if (m_colorScheme == "Night")
    globalMarinerSettings->setColorScheme(Esri::ArcGISRuntime::EncColorScheme::Night);

  // Apply area symbolization
  if (m_areaSymbolizationType == "Plain")
    globalMarinerSettings->setAreaSymbolizationType(Esri::ArcGISRuntime::EncAreaSymbolizationType::Plain);
  else
    globalMarinerSettings->setAreaSymbolizationType(Esri::ArcGISRuntime::EncAreaSymbolizationType::Symbolized);

  // Apply point symbolization
  if (m_pointSymbolizationType == "PaperChart")
    globalMarinerSettings->setPointSymbolizationType(Esri::ArcGISRuntime::EncPointSymbolizationType::PaperChart);
  else
    globalMarinerSettings->setPointSymbolizationType(Esri::ArcGISRuntime::EncPointSymbolizationType::Simplified);
}

void AddEncExchangeSet::clearAllSelections()
{
  if (!m_mapView)
    return;

  // For each layer in the operational layers that is an ENC layer
  LayerListModel* operationalLayers = m_map->operationalLayers();
  for (int i = 0; i < operationalLayers->size(); ++i)
  {
    EncLayer* encLayer = qobject_cast<EncLayer*>(operationalLayers->at(i));
    if (encLayer)
    {
      // Clear the layer's selection
      encLayer->clearSelection();
    }
  }

  // Hide the callout
  m_mapView->calloutData()->setVisible(false);
}

void AddEncExchangeSet::onGeoViewTapped(QMouseEvent& mouseEvent)
{
  if (!m_mapView)
    return;

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
    if (!geoElements.isEmpty())
    {
      EncFeature* encFeature = dynamic_cast<EncFeature*>(geoElements.first());
      if (encFeature)
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
    }
    else
    {
      m_mapView->calloutData()->setVisible(false);
    }
  })
    .onFailed(this, [this](const ErrorException& e)
  {
    // If identify fails, hide callout
    m_mapView->calloutData()->setVisible(false);
  });
}
