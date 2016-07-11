// Copyright 2016 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#include "DisplayLayerViewDrawStatus.h"

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

DisplayLayerViewDrawStatus::DisplayLayerViewDrawStatus(QQuickItem* parent) :
  QQuickItem(parent),
  m_map(nullptr),
  m_mapView(nullptr),
  m_imageLayer(nullptr),
  m_tiledLayer(nullptr),
  m_featureTable(nullptr),
  m_featureLayer(nullptr)
{
}

DisplayLayerViewDrawStatus::~DisplayLayerViewDrawStatus()
{
}

void DisplayLayerViewDrawStatus::componentComplete()
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
    m_layerNames.append(m_map->operationalLayers()->at(i)->name());
    m_layerViewStates.append(QString("Unknown"));
  }

  emit namesChanged();

  connectSignals();
}

//add layers and connect layerViewStateChanged signal
void DisplayLayerViewDrawStatus::addLayers()
{
  // create tiled layer using a url
  m_tiledLayer = new ArcGISTiledLayer(QUrl("http://sampleserver6.arcgisonline.com/arcgis/rest/services/WorldTimeZones/MapServer"), this);
  // add to map
  m_map->operationalLayers()->append(m_tiledLayer);

  // create map image using url
  m_imageLayer = new ArcGISMapImageLayer(QUrl("http://sampleserver6.arcgisonline.com/arcgis/rest/services/Census/MapServer"), this);
  m_imageLayer->setMinScale(40000000);
  m_imageLayer->setMaxScale(2000000);
  // add to map
  m_map->operationalLayers()->append(m_imageLayer);

  // create feature table using url
  m_featureTable = new ServiceFeatureTable(QUrl("http://sampleserver6.arcgisonline.com/arcgis/rest/services/Recreation/FeatureServer/0"), this);
  // create feature layer using table
  m_featureLayer = new FeatureLayer(m_featureTable, this);
  // add feature layer to map
  m_map->operationalLayers()->append(m_featureLayer);
}

void DisplayLayerViewDrawStatus::connectSignals()
{
  // connect layerViewStateChanged signal
  connect(m_mapView,&MapQuickView::layerViewStateChanged,[this](Layer* layer, LayerViewState viewState)
  {
    int rIndex;

    // find index in QStringList of that layer
    for (int i = 0; i < m_map->operationalLayers()->size(); ++i)
      if (layer == m_map->operationalLayers()->at(i))
        rIndex = i;

    // replace layer name in QStringList
    m_layerNames[rIndex] = layer->name();

    // use insert to replace values mapped to layer name
    if (viewState.status() == Esri::ArcGISRuntime::LayerViewStatus::Active)
      m_layerViewStates[rIndex] = QString("Active");
    else if (viewState.status() == Esri::ArcGISRuntime::LayerViewStatus::NotVisible)
     m_layerViewStates[rIndex] = QString("Not Visible");
    else if (viewState.status() == Esri::ArcGISRuntime::LayerViewStatus::OutOfScale)
      m_layerViewStates[rIndex] = QString("Out of Scale");
    else if (viewState.status() == Esri::ArcGISRuntime::LayerViewStatus::Loading)
      m_layerViewStates[rIndex] = QString("Loading");
    else if (viewState.status() == Esri::ArcGISRuntime::LayerViewStatus::Error)
      m_layerViewStates[rIndex] = QString("Error");
    else
      m_layerViewStates[rIndex] = QString("Unknown");

    emit statusChanged();
  });
}

QStringList DisplayLayerViewDrawStatus::layerViewStates() const
{
  return m_layerViewStates;
}

QStringList DisplayLayerViewDrawStatus::layerNames() const
{
  return m_layerNames;
}

