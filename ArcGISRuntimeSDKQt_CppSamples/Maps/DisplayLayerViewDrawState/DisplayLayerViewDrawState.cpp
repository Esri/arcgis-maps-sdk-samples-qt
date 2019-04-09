// [WriteFile Name=DisplayLayerViewDrawState, Category=Maps]
// [Legal]
// Copyright 2016 Esri.

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

#include "DisplayLayerViewDrawState.h"

#include "ArcGISMapImageLayer.h"
#include "ArcGISTiledLayer.h"
#include "FeatureLayer.h"
#include "FeatureTable.h"
#include "Point.h"
#include "Map.h"
#include "MapQuickView.h"
#include "ServiceFeatureTable.h"
#include "SpatialReference.h"
#include "Viewpoint.h"

#include <QUrl>

using namespace Esri::ArcGISRuntime;

DisplayLayerViewDrawState::DisplayLayerViewDrawState(QQuickItem* parent) :
  QQuickItem(parent)
{
}

DisplayLayerViewDrawState::~DisplayLayerViewDrawState() = default;

void DisplayLayerViewDrawState::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DisplayLayerViewDrawState>("Esri.Samples", 1, 0, "DisplayLayerViewDrawStateSample");
}

const QList<QObject*>& DisplayLayerViewDrawState::statusModel() const
{
  return m_statuses;
}

void DisplayLayerViewDrawState::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // create a new map instance without a basemap
  m_map = new Map(this);

  // add three layers to map and connect signal
  addLayers();

  // set map on the map view
  m_mapView->setMap(m_map);

  // set Initial Viewpoint
  m_mapView->setViewpointCenter(Point(-11e6, 45e5, SpatialReference(102100)), 5e7);

  // initialize QStringList of layer names and states
  for (int i = 0; i < m_map->operationalLayers()->size(); ++i)
  {
    DisplayItem* o = new DisplayItem(m_map->operationalLayers()->at(i)->name(),
                                     "Unknown",
                                     this);
    m_statuses << o;
  }

  emit modelChanged();

  connectSignals();
}

//add layers and connect layerViewStateChanged signal
void DisplayLayerViewDrawState::addLayers()
{
  // create tiled layer using a url
  m_tiledLayer = new ArcGISTiledLayer(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/WorldTimeZones/MapServer"), this);
  // add to map
  m_map->operationalLayers()->append(m_tiledLayer);

  // create map image using url
  m_imageLayer = new ArcGISMapImageLayer(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/Census/MapServer"), this);
  m_imageLayer->setMinScale(40000000);
  m_imageLayer->setMaxScale(2000000);
  // add to map
  m_map->operationalLayers()->append(m_imageLayer);

  // create feature table using url
  m_featureTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/Recreation/FeatureServer/0"), this);
  // create feature layer using table
  m_featureLayer = new FeatureLayer(m_featureTable, this);
  // add feature layer to map
  m_map->operationalLayers()->append(m_featureLayer);
}

void DisplayLayerViewDrawState::connectSignals()
{
  // connect layerViewStateChanged signal
  connect(m_mapView, &MapQuickView::layerViewStateChanged, this, [this](Layer* layer, LayerViewState viewState)
  {
    int rIndex = 0;

    // find index in QStringList of that layer
    for (int i = 0; i < m_map->operationalLayers()->size(); ++i)
    {
      if (layer == m_map->operationalLayers()->at(i))
        rIndex = i;
    }

    QObject* drawStatus = m_statuses[rIndex];
    // replace layer name in QStringList
   drawStatus->setProperty("name", layer->name());

    // use insert to replace values mapped to layer name
    if (viewState.statusFlags() & Esri::ArcGISRuntime::LayerViewStatus::Active)
      drawStatus->setProperty("status", "Active");
    else if (viewState.statusFlags() & Esri::ArcGISRuntime::LayerViewStatus::NotVisible)
      drawStatus->setProperty("status", "Not visible");
    else if (viewState.statusFlags() & Esri::ArcGISRuntime::LayerViewStatus::OutOfScale)
      drawStatus->setProperty("status", "Out of scale");
    else if (viewState.statusFlags() & Esri::ArcGISRuntime::LayerViewStatus::Loading)
      drawStatus->setProperty("status", "Loading");
    else if (viewState.statusFlags() & Esri::ArcGISRuntime::LayerViewStatus::Error)
      drawStatus->setProperty("status", "Error");
    else
      drawStatus->setProperty("status", "Unknown");

    emit modelChanged();
  });
}

DisplayItem::DisplayItem(const QString& name, const QString& status, QObject* parent) :
  QObject(parent),
  m_name(name),
  m_status(status)
{
}

DisplayItem::DisplayItem(QObject* parent) :
  QObject(parent)
{
}

DisplayItem::~DisplayItem() = default;

void DisplayItem::setName(const QString& name)
{
  m_name = name;
  emit nameChanged();
}

QString DisplayItem::name() const
{
  return m_name;
}

void DisplayItem::setStatus(const QString& status)
{
  m_status = status;
  emit statusChanged();
}
QString DisplayItem::status() const
{
  return m_status;
}


