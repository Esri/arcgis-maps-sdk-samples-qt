// Copyright 2022 ESRI
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

#include "ClassBreaksWithAlternateSymbols.h"

#include "Basemap.h"
#include "Map.h"
#include "MapQuickView.h"
#include "ServiceFeatureTable.h"
#include "FeatureLayer.h"
#include "ClassBreaksRenderer.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "ArcGISMapImageLayer.h"
#include "ArcGISMapImageSublayer.h"
#include "MultilayerPolygonSymbol.h"
#include "SymbolReferenceProperties.h"
#include "SimpleFillSymbol.h"
#include "PictureMarkerSymbol.h"
#include "MultilayerPointSymbol.h"

#include <QUrl>

using namespace Esri::ArcGISRuntime;

ClassBreaksWithAlternateSymbols::ClassBreaksWithAlternateSymbols(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISStreets, this))
{
}

ClassBreaksWithAlternateSymbols::~ClassBreaksWithAlternateSymbols()
{
}

MapQuickView* ClassBreaksWithAlternateSymbols::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ClassBreaksWithAlternateSymbols::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  // create the feature table
  m_featureTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/SF311/FeatureServer/0"), this);
  // create the feature layer using the feature table
  m_featureLayer = new FeatureLayer(m_featureTable, this);

  connect(m_featureLayer, &FeatureLayer::loadStatusChanged, this, [this](LoadStatus loadStatus)
  {
    loadStatus == LoadStatus::Loaded ? m_initialized = true : m_initialized = false;
  });

  m_featureLayer->setRenderer(m_renderer);

  // add the feature layer to the map
  m_map->operationalLayers()->append(m_featureLayer);

  m_map->setInitialViewpoint(Viewpoint(Envelope(-229100, 6550700, -223300, 6552100, SpatialReference::webMercator())));


  emit mapViewChanged();
}
void ClassBreaksWithAlternateSymbols::createClassBreaksRenderer()
{
  // create class breaks renderer using a default symbol and the alternate symbols list
  auto alternate_symbols = createAlternateSymbols();

  auto orange_tent = new PictureMarkerSymbol(QUrl("qrc:/Samples/Layers/ClassBreaksWithAlternateSymbols/tent_orange.png"), this);
  orange_tent->setWidth(80);
  orange_tent->setHeight(80);
  auto multilayer_orange_tent = orange_tent->toMultilayerSymbol();

  auto class_break = new ClassBreak("CB1", "CB1", 0, 10000000, multilayer_orange_tent, alternate_symbols, this);

  //create a class breaks renderer
  m_classBreaksRenderer = new ClassBreaksRenderer(this);

  // create and append class breaks
  m_classBreaksRenderer->classBreaks()->append(class_break);
  m_renderer = m_classBreaksRenderer;
}

QList<Symbol*> ClassBreaksWithAlternateSymbols::createAlternateSymbols()
{
  // create the first symbol for alternate symbols
  auto red_tent = new PictureMarkerSymbol(QUrl("qrc:/Samples/Layers/ClassBreaksWithAlternateSymbols/tent_red.png"), this);
  red_tent->setWidth(800);
  red_tent->setHeight(800);
  auto multilayer_red_tent = red_tent->toMultilayerSymbol();
  multilayer_red_tent->setReferenceProperties(new SymbolReferenceProperties(0, 4000000, this));

  // create the picture marker symbol for the alternate symbol
  auto blue_tent = new PictureMarkerSymbol(QUrl("qrc:/Samples/Layers/ClassBreaksWithAlternateSymbols/tent_blue.png"), this);
  red_tent->setWidth(80);
  red_tent->setHeight(80);
  auto multilayer_blue_tent = blue_tent->toMultilayerSymbol();
  multilayer_blue_tent->setReferenceProperties(new SymbolReferenceProperties(4000000, 5000000, this));

  return {multilayer_red_tent, multilayer_blue_tent};
}

void ClassBreaksWithAlternateSymbols::setScale(int16_t scale)
{
  if(!m_map)
    return;

  m_map->setReferenceScale(scale);

  emit mapViewChanged();
}
