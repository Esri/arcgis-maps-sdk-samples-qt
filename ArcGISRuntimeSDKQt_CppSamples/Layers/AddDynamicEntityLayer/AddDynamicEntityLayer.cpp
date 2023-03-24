// COPYRIGHT 2023 ESRI
// TRADE SECRETS: ESRI PROPRIETARY AND CONFIDENTIAL
// Unpublished material - all rights reserved under the
// Copyright Laws of the United States and applicable international
// laws, treaties, and conventions.
//
// For additional information, contact:
// Environmental Systems Research Institute, Inc.
// Attn: Contracts and Legal Services Department
// 380 New York Street
// Redlands, California, 92373
// USA
//
// email: contracts@esri.com
/// \file AddDynamicEntityLayer.cpp

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

#include "AddDynamicEntityLayer.h"

#include "ArcGISStreamService.h"
#include "ArcGISStreamServiceFilter.h"
#include "DynamicEntityDataSourcePurgeOptions.h"
#include "DynamicEntityLayer.h"
#include "Envelope.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "RealTimeTypes.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleRenderer.h"
#include "SpatialReference.h"
#include "SymbolTypes.h"
#include "TaskWatcher.h"
#include "TrackDisplayProperties.h"
#include "UniqueValue.h"
#include "UniqueValueRenderer.h"
#include "Viewpoint.h"

using namespace Esri::ArcGISRuntime;

namespace
{
// This envelope is a limited region around Sandy, Utah. It will be the extent used by the `DynamicEntityFilter`.
auto utahSandyEnvelope = Envelope(
      Point(-112.110052, 40.718083, SpatialReference::wgs84()),
      Point(-111.814782, 40.535247, SpatialReference::wgs84())
      );
}

AddDynamicEntityLayer::AddDynamicEntityLayer(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISDarkGray, this))
{
  // Create a dynamic entity data source from a given URL
  QUrl streamServiceUrl("https://realtimegis2016.esri.com:6443/arcgis/rest/services/SandyVehicles/StreamServer");
  m_dynamicEntityDataSource = new ArcGISStreamService(streamServiceUrl, this);

  // Create and set an ArcGISStreamServiceFilter to filter what data is received from the server
  auto streamServiceFilter = new ArcGISStreamServiceFilter(this);
  streamServiceFilter->setGeometry(utahSandyEnvelope);
  streamServiceFilter->setWhereClause("speed > 0");
  m_dynamicEntityDataSource->setFilter(streamServiceFilter);

  // Set purge options to manage when entities are removed from the cache
  m_dynamicEntityDataSource->purgeOptions()->setMaximumDuration(300.0 /*seconds*/);

  // Handle signals emitted when connection status changes to update the UI
  connect(m_dynamicEntityDataSource, &ArcGISStreamService::connectionStatusChanged, this, &AddDynamicEntityLayer::connectionStatusChanged);

  // Create a dynamic entity layer to display on the map and set the properties
  m_dynamicEntityLayer = new DynamicEntityLayer(m_dynamicEntityDataSource, this);

  // Set the track display properties to change what is displayed to the user
  m_dynamicEntityLayer->trackDisplayProperties()->setShowTrackLine(true);
  m_dynamicEntityLayer->trackDisplayProperties()->setShowPreviousObservations(true);

  // Create renderers for observations and their tracklines to change how they are styled

  // Create a unique value renderer for the latest observations
  QList<UniqueValue*> entityValues;
  entityValues.append(new UniqueValue("","", {3}, new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, Qt::magenta, 8, this), this));
  entityValues.append(new UniqueValue("","", {4}, new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, Qt::green, 8, this), this));

  UniqueValueRenderer* entityRenderer = new UniqueValueRenderer("", new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, Qt::blue, 8, this), {"agency"}, entityValues, this);
  m_dynamicEntityLayer->setRenderer(entityRenderer);

  // Create a unique value renderer for the previous observations
  QList<UniqueValue*> previousObservationValues;

  previousObservationValues.append(new UniqueValue("","", {3}, new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, Qt::magenta, 3, this), this));
  previousObservationValues.append(new UniqueValue("","", {4}, new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, Qt::green, 3, this), this));

  UniqueValueRenderer* trackRenderer = new UniqueValueRenderer("", new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, Qt::blue, 3, this), {"agency"}, previousObservationValues);
  m_dynamicEntityLayer->trackDisplayProperties()->setPreviousObservationRenderer(trackRenderer);

  // Use a simple renderer to change the style of the trackline
  m_dynamicEntityLayer->trackDisplayProperties()->setTrackLineRenderer(new SimpleRenderer(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::lightGray, 2, this)));

  m_map->operationalLayers()->append(m_dynamicEntityLayer);
}

AddDynamicEntityLayer::~AddDynamicEntityLayer() = default;

void AddDynamicEntityLayer::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<AddDynamicEntityLayer>("Esri.Samples", 1, 0, "AddDynamicEntityLayerSample");
}

MapQuickView* AddDynamicEntityLayer::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void AddDynamicEntityLayer::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  // Create and show a dashed line around the filter area
  GraphicsOverlay* borderOverlay = new GraphicsOverlay(this);
  borderOverlay->graphics()->append(new Graphic(utahSandyEnvelope, new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, Qt::red, 2, this), this));
  m_mapView->graphicsOverlays()->append(borderOverlay);

  // Set the initial viewpoint to this area
  m_mapView->setViewpointAndWait(Viewpoint(utahSandyEnvelope));

  emit mapViewChanged();
}

void AddDynamicEntityLayer::setObservationsPerTrack(int observationsPerTrack)
{
  // Update the number of entity observations displayed using the value from the UI slider
  m_dynamicEntityLayer->trackDisplayProperties()->setMaximumObservations(observationsPerTrack);
}

void AddDynamicEntityLayer::showTrackLines(bool showTrackLines)
{
  // Show or hide the lines that connect previous observations
  m_dynamicEntityLayer->trackDisplayProperties()->setShowTrackLine(showTrackLines);
}

void AddDynamicEntityLayer::showPreviousObservations(bool showPreviousObservations)
{
  // Show or hide previous observations (if maximum observations is greater than 1)
  m_dynamicEntityLayer->trackDisplayProperties()->setShowPreviousObservations(showPreviousObservations);
}

QString AddDynamicEntityLayer::connectionStatus() const
{
  // Return the current dynamic entity data source connection status as a string to display in the UI
  switch (m_dynamicEntityDataSource->connectionStatus())
  {
    case ConnectionStatus::Disconnected:
      return "Disconnected";
    case ConnectionStatus::Connecting:
      return "Connecting";
    case ConnectionStatus::Connected:
      return "Connected";
    case ConnectionStatus::Failed:
      return "Failed";
    default:
      return "Unknown";
  }
}

void AddDynamicEntityLayer::enableDisableConnection()
{
  // Handle the UI connection switch and disable or enable connection
  switch (m_dynamicEntityDataSource->connectionStatus())
  {
    case ConnectionStatus::Disconnected:
      m_dynamicEntityDataSource->connectDataSource();
      break;
    case ConnectionStatus::Connecting:
      // Do nothing and allow data source to finish connecting
      break;
    case ConnectionStatus::Connected:
      m_dynamicEntityDataSource->disconnectDataSource();
      break;
    case ConnectionStatus::Failed:
      qWarning() << "Unable to connect to dynamic entity data source";
      break;
    default:
      break;
  }
}

void AddDynamicEntityLayer::purgeAllObservations()
{
  // Remove all current observations from the cache
  m_dynamicEntityDataSource->purgeAll();
}
