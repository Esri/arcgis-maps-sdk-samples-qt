// [WriteFile Name=FilterMosaicDatasetRasterByTimeExtent, Category=DisplayInformation]
// [Legal]
// Copyright 2026 Esri.
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
#include "FilterMosaicDatasetRasterByTimeExtent.h"

// ArcGIS Maps SDK headers
#include "Envelope.h"
#include "Error.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "MosaicDatasetRaster.h"
#include "RasterLayer.h"
#include "TimeExtent.h"

// Qt headers
#include <QDebug>
#include <QFuture>
#include <QStandardPaths>

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
static QString defaultDataPath()
{
  QString dataPath;

#ifdef Q_OS_IOS
  dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
  dataPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif

  return dataPath;
}

FilterMosaicDatasetRasterByTimeExtent::FilterMosaicDatasetRasterByTimeExtent(QObject* parent /* = nullptr */) :
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISImageryStandard, this)),
  m_mosaicDatasetRaster(
    new MosaicDatasetRaster(defaultDataPath() + "/ArcGIS/Runtime/Data/raster/ElToroMosaicDataset/el_toro.geodatabase", "el_toro", this)),
  m_rasterLayer(new RasterLayer(m_mosaicDatasetRaster, this))
{
  // Create a mosaic dataset raster from the mobile mosaic dataset and display it in a raster layer
  m_map->operationalLayers()->append(m_rasterLayer);
}

FilterMosaicDatasetRasterByTimeExtent::~FilterMosaicDatasetRasterByTimeExtent() = default;

void FilterMosaicDatasetRasterByTimeExtent::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<FilterMosaicDatasetRasterByTimeExtent>("Esri.Samples", 1, 0, "FilterMosaicDatasetRasterByTimeExtentSample");
}

MapQuickView* FilterMosaicDatasetRasterByTimeExtent::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void FilterMosaicDatasetRasterByTimeExtent::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;

  // Once the raster layer has loaded, zoom to it and get the distinct timestamps of the raster
  connect(m_rasterLayer, &RasterLayer::doneLoading, this, [this](const Error& error)
  {
    if (!error.isEmpty())
    {
      failLoading();
      return;
    }

    m_mapView->setViewpointGeometryAsync(m_rasterLayer->fullExtent(), 100);

    m_mosaicDatasetRaster->distinctTimestampsAsync()
      .then(this,
            [this](const QList<QDateTime>& timestamps)
    {
      m_timestamps = timestamps;
      emit timestampsChanged();

      // Display the imagery for the initial timestamp
      setCurrentIndex(0);
      finishLoading();
    })
      .onFailed(this,
                [this]
    {
      failLoading();
    })
      .onCanceled(this, [this]
    {
      failLoading();
    });
  });

  m_mapView->setMap(m_map);

  emit mapViewChanged();
}

int FilterMosaicDatasetRasterByTimeExtent::currentIndex() const
{
  return m_currentIndex;
}

void FilterMosaicDatasetRasterByTimeExtent::finishLoading()
{
  m_loading = false;
  emit loadingChanged();
}

void FilterMosaicDatasetRasterByTimeExtent::failLoading()
{
  m_loadError = tr("Load error");
  qWarning().noquote() << QStringLiteral("Unable to load offline imagery or its timestamps. Download and extract ElToroMosaicDataset to %1, "
                                         "ensure el_toro.geodatabase is readable, then reopen the sample.")
                            .arg(defaultDataPath() + "/ArcGIS/Runtime/Data/raster/ElToroMosaicDataset");
  finishLoading();
}

void FilterMosaicDatasetRasterByTimeExtent::setCurrentIndex(int index)
{
  if (index < 0 || index >= m_timestamps.size() || index == m_currentIndex)
  {
    return;
  }

  m_currentIndex = index;

  // Filter the raster to the selected timestamp by setting a time extent on the map view
  m_mapView->setTimeExtent(TimeExtent(m_timestamps.at(m_currentIndex)));

  emit currentIndexChanged();
}

int FilterMosaicDatasetRasterByTimeExtent::timestampCount() const
{
  return m_timestamps.size();
}

QString FilterMosaicDatasetRasterByTimeExtent::currentTimestamp() const
{
  if (m_currentIndex < 0)
  {
    return {};
  }

  return m_timestamps.at(m_currentIndex).toUTC().toString("MM/dd/yyyy");
}

void FilterMosaicDatasetRasterByTimeExtent::setRasterOpacity(double opacity)
{
  m_rasterLayer->setOpacity(static_cast<float>(opacity));
}
