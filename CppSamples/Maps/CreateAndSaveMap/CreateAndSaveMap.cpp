// [WriteFile Name=CreateAndSaveMap, Category=Maps]
// [Legal]
// Copyright 2018 Esri.
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
#include "CreateAndSaveMap.h"

// ArcGIS Maps SDK headers
#include "ArcGISMapImageLayer.h"
#include "Basemap.h"
#include "Error.h"
#include "ErrorException.h"
#include "Item.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Portal.h"

// Qt headers
#include <QFuture>
#include <QUuid>

using namespace Esri::ArcGISRuntime;

CreateAndSaveMap::CreateAndSaveMap(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void CreateAndSaveMap::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<CreateAndSaveMap>("Esri.Samples", 1, 0, "CreateAndSaveMapSample");
}

void CreateAndSaveMap::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // create the Portal object
  constexpr bool loginRequired = true;
  m_portal = new Portal(QUrl("https://www.arcgis.com"), loginRequired, this);
  connect(m_portal, &Portal::doneLoading, this, [this](const Error& e)
  {
    if (!e.isEmpty())
      return;

    emit portalLoaded();
  });
}

void CreateAndSaveMap::createMap(const QString& basemap, const QStringList& operationalLayers)
{
  // Create the Basemap
  Basemap* selectedBasemap = nullptr;
  if (basemap == "Streets")
    selectedBasemap = new Basemap(BasemapStyle::ArcGISStreets, this);
  else if (basemap == "Imagery")
    selectedBasemap = new Basemap(BasemapStyle::ArcGISImageryStandard, this);
  else if (basemap == "Topographic")
    selectedBasemap = new Basemap(BasemapStyle::ArcGISTopographic, this);
  else if (basemap == "Oceans")
    selectedBasemap = new Basemap(BasemapStyle::ArcGISOceans, this);

  // Create the Map with the basemap
  m_map = new Map(selectedBasemap, this);

  // Add Operational Layers
  for (const QString& layer : operationalLayers)
  {
    if (layer == "WorldElevations")
    {
      ArcGISMapImageLayer* elevationLyr = new ArcGISMapImageLayer(QUrl("https://sampleserver5.arcgisonline.com/arcgis/rest/services/Elevation/WorldElevations/MapServer"), this);
      elevationLyr->setOpacity(0.5f);
      m_map->operationalLayers()->append(elevationLyr);
    }
    else if (layer == "Census")
    {
      m_map->operationalLayers()->append(new ArcGISMapImageLayer(QUrl("https://sampleserver5.arcgisonline.com/arcgis/rest/services/Census/MapServer"), this));
    }
  }

  // Set the Map on the MapView
  m_mapView->setMap(m_map);
}

// Load the portal if not already loaded
void CreateAndSaveMap::loadPortal()
{
  if (!m_portal)
    return;

  if (m_portal->loadStatus() == LoadStatus::Loaded)
    emit portalLoaded();
  else
    m_portal->load();
}

void CreateAndSaveMap::saveMap(const QString& title, const QString& tags, const QString& description)
{
  if (!m_map || !m_portal)
    return;

  // create save parameters
  const QStringList tagsList = tags.split(",");
  constexpr bool forceSave = false;
  const PortalFolder folder;
  const QByteArray thumbnail;

  // save the map
  m_map->saveAsAsync(m_portal, title, tagsList, forceSave, folder, description, thumbnail).then(this,
  [this]()
  {
    const QString itemId = m_map->item()->itemId();
    emit saveMapCompleted(true, itemId);
  })
  .onFailed(
  [this](ErrorException e)
  {
    emit saveMapCompleted(false, "", QString("%1 %2").arg(e.error().message(), e.error().additionalMessage()));
  });
}
