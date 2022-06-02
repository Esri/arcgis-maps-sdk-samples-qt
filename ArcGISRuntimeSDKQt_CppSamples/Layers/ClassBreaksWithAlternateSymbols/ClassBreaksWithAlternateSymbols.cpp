// [WriteFile Name=ClassBreaksWithAlternateSymbols, Category=Layers]
// [Legal]
// Copyright 2022 Esri.

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

#include "ClassBreaksWithAlternateSymbols.h"

#include "FeatureLayer.h"
#include "ServiceFeatureTable.h"
#include "Map.h"
#include "MapQuickView.h"

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
#include "SimpleMarkerSymbol.h"

using namespace Esri::ArcGISRuntime;

ClassBreaksWithAlternateSymbols::ClassBreaksWithAlternateSymbols(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISTopographic, this))
{

  // create the feature table
  ServiceFeatureTable* featureTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/SF311/FeatureServer/0"), this);
  // create the feature layer using the feature table
  m_featureLayer = new FeatureLayer(featureTable, this);

  connect(m_featureLayer, &FeatureLayer::doneLoading, this, [](Error e)
  {
    if (!e.isEmpty())
    {
      qDebug() << "Error" << e.message() << e.additionalMessage();
      return;
    }

    qDebug() << "done loading";
  });

  createClassBreaksRenderer();

  // add the feature layer to the map
  m_map->operationalLayers()->append(m_featureLayer);

  emit mapViewChanged();
}

ClassBreaksWithAlternateSymbols::~ClassBreaksWithAlternateSymbols() = default;

void ClassBreaksWithAlternateSymbols::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ClassBreaksWithAlternateSymbols>("Esri.Samples", 1, 0, "ClassBreaksWithAlternateSymbolsSample");
}

MapQuickView* ClassBreaksWithAlternateSymbols::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ClassBreaksWithAlternateSymbols::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  connect(m_mapView, &MapQuickView::viewpointChanged, this, [this](){
    qDebug() << (int)m_mapView->mapScale();
  });

  emit mapViewChanged();
}

void ClassBreaksWithAlternateSymbols::createClassBreaksRenderer()
{
  // create class breaks renderer using a default symbol and the alternate symbols list
  auto alternate_symbols = createAlternateSymbols();

//  auto red_tent = new PictureMarkerSymbol(QUrl("qrc:/Samples/Layers/ClassBreaksWithAlternateSymbols/tent_red.png"), this);
//  red_tent->setWidth(30);
//  red_tent->setHeight(30);
//  auto multilayer_red_tent = red_tent->toMultilayerSymbol();
//  multilayer_red_tent->setReferenceProperties(new SymbolReferenceProperties(0, 4000000, this));

  SimpleMarkerSymbol* sym1 = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Triangle, QColor("red"), 30, this);
  auto mlSym1 = sym1->toMultilayerSymbol();
  mlSym1->setReferenceProperties(new SymbolReferenceProperties(500000, 0, this));
  //create a classbreak with alternate symbols
  ClassBreak* class_break = new ClassBreak("classbreak_1", "classbreak_1", 0, 1, mlSym1, alternate_symbols, this);


//  auto class_break = new ClassBreak("CB1", "CB1", 0, 10000000, multilayer_red_tent, alternate_symbols, this);

  //create a class breaks renderer
  m_classBreaksRenderer = new ClassBreaksRenderer(this);

  // create and append class breaks
  m_classBreaksRenderer->classBreaks()->append(class_break);

  m_classBreaksRenderer->setFieldName("status");
  SimpleMarkerSymbol* default_sym = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, QColor("purple"), 30, this);
  auto default_sym_ml = default_sym->toMultilayerSymbol();

  m_classBreaksRenderer->setDefaultSymbol(default_sym_ml);
  m_classBreaksRenderer->setMinValue(0);

  //set the cbr on the feature layer
  m_featureLayer->setRenderer(m_classBreaksRenderer);
}

QList<Symbol*> ClassBreaksWithAlternateSymbols::createAlternateSymbols()
{
  // create the first symbol for alternate symbols
//  auto orange_tent = new PictureMarkerSymbol(QUrl("qrc:/Samples/Layers/ClassBreaksWithAlternateSymbols/tent_orange.png"), this);
//  orange_tent->setWidth(30);
//  orange_tent->setHeight(30);
//  auto multilayer_orange_tent = orange_tent->toMultilayerSymbol();
//  multilayer_orange_tent->setReferenceProperties(new SymbolReferenceProperties(4000000, 6000000, this));

  SimpleMarkerSymbol* sym2 = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Square, QColor("blue"), 30, this);
  auto mlSym2 = sym2->toMultilayerSymbol();
  mlSym2->setReferenceProperties(new SymbolReferenceProperties(1000000, 500000, this));

  SimpleMarkerSymbol* sym3 = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, QColor("yellow"), 30, this);
  auto mlSym3 = sym3->toMultilayerSymbol();
  mlSym3->setReferenceProperties(new SymbolReferenceProperties(1500000, 1000000, this));

  // create the picture marker symbol for the alternate symbol
//  auto blue_tent = new PictureMarkerSymbol(QUrl("qrc:/Samples/Layers/ClassBreaksWithAlternateSymbols/tent_blue.png"), this);
//  blue_tent->setWidth(30);
//  blue_tent->setHeight(30);
//  auto multilayer_blue_tent = blue_tent->toMultilayerSymbol();
//  multilayer_blue_tent->setReferenceProperties(new SymbolReferenceProperties(6000000, 8000000, this));

//  qDebug() << multilayer_orange_tent;
//  qDebug() << multilayer_blue_tent;

//  return {multilayer_orange_tent, multilayer_blue_tent};
    return {mlSym2, mlSym3};
}

void ClassBreaksWithAlternateSymbols::setScale(double scale)
{
//  if(!m_map)
//    return;

//  m_map->setReferenceScale(scale);

//  //TODO: also reset the viewpoint back to the tents

//  emit mapViewChanged();
  if(m_mapView)
    m_mapView->setViewpointScale(scale);
}
