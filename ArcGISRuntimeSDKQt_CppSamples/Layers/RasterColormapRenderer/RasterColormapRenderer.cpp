// [WriteFile Name=RasterColormapRenderer, Category=Layers]
// [Legal]
// Copyright 2017 Esri.

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

#include "RasterColormapRenderer.h"

#include "ColormapRenderer.h"
#include "Map.h"
#include "MapQuickView.h"
#include "Raster.h"
#include "RasterLayer.h"

#include <QDir>
#include <QtCore/qglobal.h>

#ifdef Q_OS_IOS
#include <QStandardPaths>
#endif // Q_OS_IOS

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
QString defaultDataPath()
{
  QString dataPath;

#ifdef Q_OS_ANDROID
  dataPath = "/sdcard";
#elif defined Q_OS_IOS
  dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
  dataPath = QDir::homePath();
#endif

  return dataPath;
}
} // namespace

RasterColormapRenderer::RasterColormapRenderer(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_dataPath(defaultDataPath() + "/ArcGIS/Runtime/Data/raster")
{
}

RasterColormapRenderer::~RasterColormapRenderer() = default;
void RasterColormapRenderer::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<RasterColormapRenderer>("Esri.Samples", 1, 0, "RasterColormapRendererSample");
}

void RasterColormapRenderer::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  //! [RasterColormapRenderer cpp add raster basemap]
  // Create the raster and raster layer
  Raster* raster = new Raster(m_dataPath + "/ShastaBW.tif", this);
  m_rasterLayer = new RasterLayer(raster, this);

  connect(m_rasterLayer, &RasterLayer::doneLoading, this, [this](Error error)
  {
    if (!error.isEmpty())
    {
      qDebug() << error.message() << error.additionalMessage();
      return;
    }

    m_mapView->setViewpointGeometry(m_rasterLayer->fullExtent(), 50);
  });

  ColormapRenderer* colormapRenderer = createRenderer();

  // set the colormap renderer on the raster layer
  m_rasterLayer->setRenderer(colormapRenderer);

  // Add the raster to the map as an operational layer
  Map* map = new Map(Basemap::imagery(this), this);

  map->operationalLayers()->append(m_rasterLayer);

  m_mapView->setMap(map);
  //! [RasterColormapRenderer cpp add raster basemap]
}

ColormapRenderer *RasterColormapRenderer::createRenderer()
{
  // create a color map where values 0-150 are red and 150-250 are yellow
  QList<QColor> colors;
  colors.reserve(250);
  for (int i = 0; i < 250; ++i)
    colors.append( i < 150 ? Qt::red : Qt::yellow);

  // create a colormap renderer
  ColormapRenderer* colormapRenderer = new ColormapRenderer(colors, this);

  return colormapRenderer;
}
