// [WriteFile Name=IdentifyRasterCell, Category=Layers]
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
#include "IdentifyRasterCell.h"

// C++ API headers
#include "AttributeListModel.h"
#include "CalloutData.h"
#include "Envelope.h"
#include "Error.h"
#include "IdentifyLayerResult.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Raster.h"
#include "RasterCell.h"
#include "RasterLayer.h"

// Qt headers
#include <QFuture>
#include <QStandardPaths>
#include <QString>
#include <QUuid>

// STL headers
#include <memory>

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

IdentifyRasterCell::IdentifyRasterCell(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISOceans, this))
{
  // initialize the raster layer
  const QString filepath = defaultDataPath() + "/ArcGIS/Runtime/Data/raster/SA_EVI_8Day_03May20/SA_EVI_8Day_03May20.tif";
  Raster* raster = new Raster(filepath, this);
  m_rasterLayer = new RasterLayer(raster, this);

  m_map->operationalLayers()->append(m_rasterLayer);
}

IdentifyRasterCell::~IdentifyRasterCell() = default;

void IdentifyRasterCell::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<IdentifyRasterCell>("Esri.Samples", 1, 0, "IdentifyRasterCellSample");
}

MapQuickView* IdentifyRasterCell::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void IdentifyRasterCell::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  connectSignals();

  emit mapViewChanged();
}

void IdentifyRasterCell::connectSignals()
{
  connect(m_rasterLayer, &Layer::doneLoading, this, [this](const Error& error)
  {
    if (!error.isEmpty())
    {
      qDebug() << error.message() << error.additionalMessage();
      return;
    }

    m_mapView->setViewpointGeometryAsync(m_rasterLayer->fullExtent());
  });

  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](const QMouseEvent& e)
  {
    m_mapView->identifyLayerAsync(m_rasterLayer, e.position(), 10, false, 1).then(this, [this](IdentifyLayerResult* rawIdentifyResult)
    {
      onIdentifyLayerCompleted_(rawIdentifyResult);
    });
    m_clickedPoint = m_mapView->screenToLocation(e.position().x(), e.position().y());
  });

  connect(m_mapView, &MapQuickView::mousePressedAndHeld, this, [this](const QMouseEvent& e)
  {
    m_mapView->identifyLayerAsync(m_rasterLayer, e.position(), 10, false, 1).then(this, [this](IdentifyLayerResult* rawIdentifyResult)
    {
      onIdentifyLayerCompleted_(rawIdentifyResult);
    });
    m_clickedPoint = m_mapView->screenToLocation(e.position().x(), e.position().y());
    m_mousePressed = true;
  });

  connect(m_mapView, &MapQuickView::mouseReleased, this, [this](QMouseEvent&)
  {
    m_mousePressed = false;
  });

  // if mouse is moved after press-and-hold, then update callout on-the-fly
  connect(m_mapView, &MapQuickView::mouseMoved, this, [this](const QMouseEvent& e)
  {
    if (m_mousePressed)
    {
      m_mapView->identifyLayerAsync(m_rasterLayer, e.position(), 10, false, 1).then(this, [this](IdentifyLayerResult* rawIdentifyResult)
      {
        onIdentifyLayerCompleted_(rawIdentifyResult);
      });
      m_clickedPoint = m_mapView->screenToLocation(e.position().x(), e.position().y());
    }
  });
}

void IdentifyRasterCell::onIdentifyLayerCompleted_(IdentifyLayerResult* rawIdentifyResult)
{
  const auto identifyResult = std::unique_ptr<IdentifyLayerResult>(rawIdentifyResult);
  const auto elements = identifyResult->geoElements();
  for (GeoElement* geoElement : elements)
  {
    if (RasterCell* rasterCell = dynamic_cast<RasterCell*>(geoElement))
    {
      QString calloutString;
      const AttributeListModel* attributes = rasterCell->attributes();
      const QStringList attributeNames = rasterCell->attributes()->attributeNames();

      for (int i = 0; i < attributeNames.size(); ++i)
      {
        const QString value = QVariant((*attributes)[attributeNames[i]]).toString();
        calloutString.append(attributeNames[i] + ": " + value + "\n");
      }

      const double xPoint = rasterCell->geometry().extent().xMin();
      const double yPoint = rasterCell->geometry().extent().yMin();

      calloutString.append("X: " + QString::number(xPoint, 'f', 2) + " Y: " + QString::number(yPoint, 'f', 2));

      m_mapView->calloutData()->setLocation(m_clickedPoint);
      m_mapView->calloutData()->setDetail(calloutString);
      m_mapView->calloutData()->setVisible(true);
      m_calloutData = m_mapView->calloutData();
      emit calloutDataChanged();
    }
  }
}

CalloutData* IdentifyRasterCell::calloutData() const
{
  return m_calloutData;
}
