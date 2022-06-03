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
#include "SymbolReferenceProperties.h"
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

  // this function creates a class breaks renderer using
  // one class break that includes a list of alternate symbols
  // for various scales
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

  // set the initial view point upon opening the sample to focus on an area with a lot of features
  Viewpoint vpCenter = Viewpoint(Point(-13632095.660131, 4545009.846004, SpatialReference::webMercator()), 50000);
  m_mapView->setViewpoint(vpCenter);

  emit mapViewChanged();
}

void ClassBreaksWithAlternateSymbols::createClassBreaksRenderer()
{
  // create class breaks renderer using a default symbol and the alternate symbols list
  QList<Symbol*> alternate_symbols = createAlternateSymbols();


  SimpleMarkerSymbol* symbol_1 = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Triangle, QColor("red"), 30, this);
  MultilayerPointSymbol* multi_layer_symbol_1 = symbol_1->toMultilayerSymbol();
  multi_layer_symbol_1->setReferenceProperties(new SymbolReferenceProperties(5000, 0, this));

  //create a classbreak with alternate symbolsre
  ClassBreak* class_break = new ClassBreak("classbreak", "classbreak", 0, 1, multi_layer_symbol_1, alternate_symbols, this);

  //create a class breaks renderer
  m_classBreaksRenderer = new ClassBreaksRenderer(this);

  // create and append class breaks
  m_classBreaksRenderer->classBreaks()->append(class_break);

  m_classBreaksRenderer->setFieldName("status");
  SimpleMarkerSymbol* default_sym = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, QColor("purple"), 30, this);
  MultilayerPointSymbol* default_sym_ml = default_sym->toMultilayerSymbol();

  m_classBreaksRenderer->setDefaultSymbol(default_sym_ml);
  m_classBreaksRenderer->setMinValue(0);

  //set the class breaks renderer on the feature layer
  m_featureLayer->setRenderer(m_classBreaksRenderer);
}

QList<Symbol*> ClassBreaksWithAlternateSymbols::createAlternateSymbols()
{
  SimpleMarkerSymbol* alternate_symbol_1 = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Square, QColor("blue"), 30, this);
  MultilayerPointSymbol* alternate_symbol_1_ml = alternate_symbol_1->toMultilayerSymbol();
  alternate_symbol_1_ml->setReferenceProperties(new SymbolReferenceProperties(10000, 5000, this));

  SimpleMarkerSymbol* alternate_symbol_2 = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, QColor("yellow"), 30, this);
  MultilayerPointSymbol* alternate_symbol_2_ml = alternate_symbol_2->toMultilayerSymbol();
  alternate_symbol_2_ml->setReferenceProperties(new SymbolReferenceProperties(20000, 10000, this));

  return {alternate_symbol_1_ml, alternate_symbol_2_ml};
}

void ClassBreaksWithAlternateSymbols::setScale(double scale)
{
  if(m_mapView)
    m_mapView->setViewpointScale(scale);
}
