// [WriteFile Name=ApplyUniqueValuesWithAlternateSymbols, Category=Layers]
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

#include "ApplyUniqueValuesWithAlternateSymbols.h"

#include "Basemap.h"
#include "FeatureLayer.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MultilayerPointSymbol.h"
#include "ServiceFeatureTable.h"
#include "SimpleMarkerSymbol.h"
#include "SymbolReferenceProperties.h"
#include "UniqueValueRenderer.h"

using namespace Esri::ArcGISRuntime;

ApplyUniqueValuesWithAlternateSymbols::ApplyUniqueValuesWithAlternateSymbols(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISTopographic, this))
{
  // create the feature table
  ServiceFeatureTable* featureTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/SF311/FeatureServer/0"), this);

  // create the feature layer using the feature table
  m_featureLayer = new FeatureLayer(featureTable, this);

  // this function creates a unique value renderer using
  // one unique value "req_type" that includes a list of
  // alternate symbols for various scales
  createUniqueValueRenderer();

  // add the feature layer to the map
  m_map->operationalLayers()->append(m_featureLayer);
}

ApplyUniqueValuesWithAlternateSymbols::~ApplyUniqueValuesWithAlternateSymbols() = default;

void ApplyUniqueValuesWithAlternateSymbols::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ApplyUniqueValuesWithAlternateSymbols>("Esri.Samples", 1, 0, "ApplyUniqueValuesWithAlternateSymbolsSample");
}

MapQuickView* ApplyUniqueValuesWithAlternateSymbols::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ApplyUniqueValuesWithAlternateSymbols::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  // set the initial view point upon opening the sample to focus on an area with a lot of features
  Viewpoint vpCenter = Viewpoint(Point(-13631205.660131, 4546829.846004, SpatialReference::webMercator()), 25000);
  m_mapView->setViewpoint(vpCenter);

  connect(m_mapView, &MapQuickView::viewpointChanged, this, [this](){
    qDebug() << (int)m_mapView->mapScale();
  });

  queryCurrentScale();

  connect(m_mapView, &MapQuickView::viewpointChanged, this, [this](){
    /*qDebug() << */
    m_currentScale = m_mapView->mapScale();
    emit currentScaleChanged();
  });

  emit mapViewChanged();
}

void ApplyUniqueValuesWithAlternateSymbols::createUniqueValueRenderer()
{
  QList<Symbol*> alternateSymbols = createAlternateSymbols();

  double minScale = 5000;
  double maxScale = 0;
  SimpleMarkerSymbol* symbol1 = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Triangle, QColor("red"), 30, this);
  MultilayerPointSymbol* multilayerSymbol1 = symbol1->toMultilayerSymbol();
  multilayerSymbol1->setReferenceProperties(new SymbolReferenceProperties(minScale, maxScale, this));

  //create a unique value with alternate symbols
  UniqueValue* unique_value = new UniqueValue("unique value", "unique values based on request type", QVariantList{"Damaged Property"}, multilayerSymbol1, alternateSymbols, this);

  //create a unique value renderer
  m_uniqueValueRenderer = new UniqueValueRenderer(this);

  // create and append unique value
  m_uniqueValueRenderer->uniqueValues()->append(unique_value);

  m_uniqueValueRenderer->setFieldNames({"req_type"});

  SimpleMarkerSymbol* defaultSym = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, QColor("purple"), 15, this);
  m_uniqueValueRenderer->setDefaultSymbol(defaultSym->toMultilayerSymbol());

  //set the unique value renderer on the feature layer
  m_featureLayer->setRenderer(m_uniqueValueRenderer);
}

QList<Symbol*> ApplyUniqueValuesWithAlternateSymbols::createAlternateSymbols()
{
  double minScale1 = 10000;
  double maxScale1 = 5000;
  SimpleMarkerSymbol* alternateSymbol1 = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Square, QColor("blue"), 45, this);
  MultilayerPointSymbol* alternateSymbolMultilayer1 = alternateSymbol1->toMultilayerSymbol();
  alternateSymbolMultilayer1->setReferenceProperties(new SymbolReferenceProperties(minScale1, maxScale1, this));

  double minScale2 = 20000;
  double maxScale2 = 10000;
  SimpleMarkerSymbol* alternateSymbol2 = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Diamond, QColor("yellow"), 30, this);
  MultilayerPointSymbol* alternateSymbolMultilayer2 = alternateSymbol2->toMultilayerSymbol();
  alternateSymbolMultilayer2->setReferenceProperties(new SymbolReferenceProperties(minScale2, maxScale2, this));

  return {alternateSymbolMultilayer1, alternateSymbolMultilayer2};
}

void ApplyUniqueValuesWithAlternateSymbols::resetViewpoint()
{
  if(m_mapView)
  {
    Viewpoint vpCenter = Viewpoint(Point(-13631205.660131, 4546829.846004, SpatialReference::webMercator()), 25000);
    m_mapView->setViewpoint(vpCenter, 5);
  }
}

double ApplyUniqueValuesWithAlternateSymbols::currentScale() const
{
  return m_currentScale;
}

void ApplyUniqueValuesWithAlternateSymbols::queryCurrentScale()
{
  connect(m_mapView, &MapQuickView::viewpointChanged, this, [this](){
    /*qDebug() << */
    m_currentScale = m_mapView->mapScale();
    emit currentScaleChanged();
  });

}
