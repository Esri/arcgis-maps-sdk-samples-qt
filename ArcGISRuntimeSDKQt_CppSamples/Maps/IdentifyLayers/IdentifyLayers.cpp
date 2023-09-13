// [WriteFile Name=IdentifyLayers, Category=Maps]
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

#include "IdentifyLayers.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "ArcGISMapImageLayer.h"
#include "FeatureLayer.h"
#include "ServiceFeatureTable.h"
#include "Viewpoint.h"
#include "Point.h"
#include "IdentifyLayerResult.h"
#include "Error.h"
#include "MapTypes.h"
#include "LayerListModel.h"
#include "ArcGISSublayerListModel.h"
#include "ArcGISSublayer.h"
#include "SpatialReference.h"

#include <QFuture>
#include <QUuid>
#include <QQueue>

using namespace Esri::ArcGISRuntime;

IdentifyLayers::IdentifyLayers(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void IdentifyLayers::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<IdentifyLayers>("Esri.Samples", 1, 0, "IdentifyLayersSample");
}

void IdentifyLayers::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the topographic basemap
  m_map = new Map(BasemapStyle::ArcGISTopographic, this);

  // add a map image layer
  QUrl mapServiceUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/SampleWorldCities/MapServer");
  ArcGISMapImageLayer* imageLayer = new ArcGISMapImageLayer(mapServiceUrl, this);
  m_map->operationalLayers()->append(imageLayer);

  // hide 2 of the sublayers in the map image layer
  connect(imageLayer, &ArcGISMapImageLayer::doneLoading, this, [imageLayer](const Error& e)
  {
    if (!e.isEmpty())
    {
      qDebug() << e.message() << e.additionalMessage();
      return;
    }

    imageLayer->mapImageSublayers()->at(1)->setVisible(false);
    imageLayer->mapImageSublayers()->at(2)->setVisible(false);
  });

  // add a feature layer
  QUrl featureLayerUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer/0");
  ServiceFeatureTable* featureTable = new ServiceFeatureTable(featureLayerUrl, this);
  FeatureLayer* featureLayer = new FeatureLayer(featureTable, this);
  m_map->operationalLayers()->append(featureLayer);

  // set an initial viewpoint
  const double mapScale = 68015210;
  const double x = -10977012.785807;
  const double y = 4514257.550369;
  const Point pt(x, y, SpatialReference::webMercator());
  Viewpoint initialViewpoint(pt, mapScale);
  m_map->setInitialViewpoint(initialViewpoint);

  // Set map to map view
  m_mapView->setMap(m_map);

  // connect signals
  connectSignals();
}

void IdentifyLayers::connectSignals()
{
  // identify layers on mouse click
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    const double tolerance = 12.0;
    const bool returnPopups = false;
    const int maxResults = 10;

    m_mapView->identifyLayersAsync(mouseEvent.position(), tolerance, returnPopups, maxResults).then(this,
    [this](QList<IdentifyLayerResult*> results){
      // reset the message text
      m_message = QString();

      for (IdentifyLayerResult* result : results)
      {
        const int count = countsublayerResults(result);
        QString layerName = result->layerContent()->name();
        m_message += QString("%1 : %2").arg(layerName).arg(count);
        m_message += "\n";
      }
      //remove last newline character
      if (!m_message.isEmpty())
        m_message.chop(1);

      emit messageChanged();
      emit showMessage();
      qDeleteAll(results);
    });

  });
}

int IdentifyLayers::countsublayerResults(IdentifyLayerResult*& result)
{
  if (!result) {
    return 0;
  }

  int totalCount = 0;
  QQueue<const IdentifyLayerResult*> queue;
  queue.enqueue(result);

  while (!queue.isEmpty())
  {
    const IdentifyLayerResult* current = queue.dequeue();
    QList<IdentifyLayerResult*> sublayerResults = current->sublayerResults();

    totalCount += result->geoElements().length();

    for (IdentifyLayerResult* result : sublayerResults)
    {
      queue.enqueue(result);
    }
  }

  return totalCount;
}
