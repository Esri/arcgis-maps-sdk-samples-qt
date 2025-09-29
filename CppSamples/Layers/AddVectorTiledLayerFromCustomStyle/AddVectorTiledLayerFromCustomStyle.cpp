// [WriteFile Name=AddVectorTiledLayerFromCustomStyle, Category=Layers]
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

#include "AddVectorTiledLayerFromCustomStyle.h"

#include "ArcGISVectorTiledLayer.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Viewpoint.h"
#include "Portal.h"
#include "PortalItem.h"
#include "ExportVectorTilesTask.h"
#include "ExportVectorTilesJob.h"
#include "ExportVectorTilesResult.h"
#include "ItemResourceCache.h"
#include "Basemap.h"
#include "VectorTileCache.h"

#include <QStandardPaths>
#include <QFuture>

using namespace Esri::ArcGISRuntime;

namespace
{
  QString defaultDataPath()
  {
#ifdef Q_OS_IOS
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
    return QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif
  }
} // namespace

AddVectorTiledLayerFromCustomStyle::AddVectorTiledLayerFromCustomStyle(QObject* parent) :
  QObject(parent),
  m_dataPath(defaultDataPath() + "/ArcGIS/Runtime/Data/vtpk/dodge_city.vtpk"),
  m_map(new Map(this))
{
}

AddVectorTiledLayerFromCustomStyle::~AddVectorTiledLayerFromCustomStyle() = default;

void AddVectorTiledLayerFromCustomStyle::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<AddVectorTiledLayerFromCustomStyle>("Esri.Samples", 1, 0, "AddVectorTiledLayerFromCustomStyleSample");
}

MapQuickView* AddVectorTiledLayerFromCustomStyle::mapView() const
{
  return m_mapView;
}

void AddVectorTiledLayerFromCustomStyle::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  const Viewpoint defaultViewpoint(10, 5.5, 1e8);
  m_map->setInitialViewpoint(defaultViewpoint);

  populatePortalIdMap();
  emit mapViewChanged();
}

void AddVectorTiledLayerFromCustomStyle::populatePortalIdMap()
{
  // The labels and portal item IDs of the online styles.
  m_portalIds.insert("Default", "1349bfa0ed08485d8a92c442a3850b06");
  m_portalIds.insert("Style 1", "bd8ac41667014d98b933e97713ba8377");
  m_portalIds.insert("Style 2", "02f85ec376084c508b9c8e5a311724fa");
  m_portalIds.insert("Style 3", "1bf0cc4a4380468fbbff107e100f65a5");

  // The labels and portal item IDs of the offline styles.
  m_portalIds.insert("Offline custom style - Light", "e01262ef2a4f4d91897d9bbd3a9b1075");
  m_portalIds.insert("Offline custom style - Dark", "ce8a34e5d4ca4fa193a097511daa8855");

  initialize();

  m_styleNames = m_portalIds.keys();
  emit setStyleNames();
}

void AddVectorTiledLayerFromCustomStyle::initialize()
{
  Portal* portal = new Portal(this);

  // Store a list all portal items.
  for (const QString& itemID : m_portalIds.values())
  {
    PortalItem* portalItem = new PortalItem(portal, itemID, this);
    m_vectorTiledLayers.insert(m_portalIds.key(itemID), portalItem);
  }

  // Export offline custom styles.
  connect(this, &AddVectorTiledLayerFromCustomStyle::styleExported, this, [this](const QString& styleName, ItemResourceCache* cache)
  {
    if (styleName == "Light")
    {
      m_lightStyleResourceCache = cache;
    }
    else if (styleName == "Dark")
    {
      m_darkStyleResourceCache = cache;
    }
  });

  exportStyle(m_vectorTiledLayers.value("Offline custom style - Light"), "Light");
  exportStyle(m_vectorTiledLayers.value("Offline custom style - Dark"), "Dark");
}

void AddVectorTiledLayerFromCustomStyle::selectionChanged(const QString style)
{
  // Check if the user selected a offline custom style.
  // Create a new basemap with the appropriate style.
  if (style.contains("Offline"))
  {
    ItemResourceCache* cache = nullptr;

    if (style.contains("Light"))
    {
      cache = m_lightStyleResourceCache;
    }
    else
    {
      cache = m_darkStyleResourceCache;
    }

    m_map->setBasemap(new Basemap(new ArcGISVectorTiledLayer(new VectorTileCache(m_dataPath), cache)));

    const Viewpoint dodgeCityViewpoint(37.76528, -100.01766, 4e4);
    m_mapView->setViewpointAsync(dodgeCityViewpoint);
  }
  else
  {
    m_map->setBasemap(new Basemap(new ArcGISVectorTiledLayer(m_vectorTiledLayers.value(style))));
    const Viewpoint defaultViewpoint(10, 5.5, 1e8);
    m_mapView->setViewpointAsync(defaultViewpoint);
  }
}

void AddVectorTiledLayerFromCustomStyle::exportStyle(PortalItem* vectorTiledLayer, const QString& styleName)
{
  vectorTiledLayer->load();
  connect(vectorTiledLayer, &PortalItem::loadStatusChanged, this, [this, vectorTiledLayer, styleName](LoadStatus status)
  {
    if (status == LoadStatus::Loaded)
    {
      // Create the task.
      ExportVectorTilesTask* exportTask = new ExportVectorTilesTask(vectorTiledLayer->url(), this);
      // Get the item resource path for the basemap styling.
      const QString itemResourceCachePath = m_tempDir.path() + QString("/itemResources%1").arg(QDateTime::currentMSecsSinceEpoch());

      // Create the export job and start it.
      Esri::ArcGISRuntime::ExportVectorTilesJob* exportJob = exportTask->exportStyleResourceCache(itemResourceCachePath);
      exportJob->start();

      // Wait for the job to complete.
      connect(exportJob, &ExportVectorTilesJob::jobDone, this, [this, exportJob, styleName]()
      {
        ExportVectorTilesResult* vectorTilesResult = exportJob->result();

        ItemResourceCache* cache = vectorTilesResult->itemResourceCache();

        if (styleName == "Light")
        {
          m_lightVectorTilesResult = vectorTilesResult;
        }
        else if (styleName == "Dark")
        {
          m_darkVectorTilesResult = vectorTilesResult;
        }

        connect(cache, &ItemResourceCache::loadStatusChanged, this, [this, cache, styleName](LoadStatus status)
        {
          if (status == LoadStatus::Loaded)
          {
            emit styleExported(styleName, cache);
          }
        });

        cache->load();
      });
    }
  });
}

QStringList AddVectorTiledLayerFromCustomStyle::styleNames() const
{
  return m_styleNames;
}
