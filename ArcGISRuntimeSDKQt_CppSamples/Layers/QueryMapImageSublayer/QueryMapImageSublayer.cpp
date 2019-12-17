// [WriteFile Name=QueryMapImageSublayer, Category=Layers]
// [Legal]
// Copyright 2018 Esri.

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

#include "QueryMapImageSublayer.h"

#include "Basemap.h"
#include "Map.h"
#include "MapQuickView.h"
#include "ArcGISMapImageLayer.h"
#include "ArcGISMapImageSublayer.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "QueryParameters.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "GraphicListModel.h"

using namespace Esri::ArcGISRuntime;

QueryMapImageSublayer::QueryMapImageSublayer(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void QueryMapImageSublayer::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<QueryMapImageSublayer>("Esri.Samples", 1, 0, "QueryMapImageSublayerSample");
}

void QueryMapImageSublayer::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the streets vector basemap
  m_map = new Map(Basemap::streetsVector(this), this);

  // add map image layer
  m_usaImageLayer = new ArcGISMapImageLayer(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/USA/MapServer"), this);
  m_map->operationalLayers()->append(m_usaImageLayer);

  // add a graphics overlay
  m_selectionOverlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(m_selectionOverlay);

  // create symbols
  createSymbols();

  // connect signals
  connectSignals();

  // set initial viewpoint
  Viewpoint vp(Point(-12716000, 4170400, SpatialReference::webMercator()), 6000000 /*scale*/);
  m_map->setInitialViewpoint(vp);

  // Set map to map view
  m_mapView->setMap(m_map);
}

void QueryMapImageSublayer::connectSignals()
{
  connect(m_usaImageLayer, &ArcGISMapImageLayer::loadTablesAndLayersCompleted, this, [this](QUuid)
  {
    ArcGISSublayerListModel* sublayers = m_usaImageLayer->mapImageSublayers();
    if (sublayers->size() < 4)
      return;

    // get the sublayer's tables
    m_citiesTable = dynamic_cast<ArcGISMapImageSublayer*>(sublayers->at(0))->table();
    m_statesTable = dynamic_cast<ArcGISMapImageSublayer*>(sublayers->at(2))->table();
    m_countiesTable = dynamic_cast<ArcGISMapImageSublayer*>(sublayers->at(3))->table();

    // connect to city sublayer query signal
    connect(m_citiesTable, &ServiceFeatureTable::queryFeaturesCompleted, this, [this](QUuid, FeatureQueryResult* result)
    {
      addResultsAsGraphics(result, m_citySymbol);
      delete result;
    });

    // connect to county sublayer query signal
    connect(m_countiesTable, &ServiceFeatureTable::queryFeaturesCompleted, this, [this](QUuid, FeatureQueryResult* result)
    {
      addResultsAsGraphics(result, m_countySymbol);
      delete result;
    });

    // connect to states sublayer query signal
    connect(m_statesTable, &ServiceFeatureTable::queryFeaturesCompleted, this, [this](QUuid, FeatureQueryResult* result)
    {
      addResultsAsGraphics(result, m_stateSymbol);
      delete result;
    });
  });

  connect(m_usaImageLayer, &ArcGISMapImageLayer::doneLoading, this, [this](Error e)
  {
    if (!e.isEmpty())
      return;

    // load the sublayers and tables of the map image layer
    m_usaImageLayer->loadTablesAndLayers();
  });
}

void QueryMapImageSublayer::createSymbols()
{
  // create symbol for city selection
  m_citySymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("red"), 16.0f /*size*/, this);

  // create symbol for county selection
  SimpleLineSymbol* countyOutline = new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, QColor("cyan"), 2.0f /*width*/, this);
  m_countySymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::DiagonalCross, QColor("cyan"), countyOutline, this);

  // create symbol for state selection
  SimpleLineSymbol* stateOutline = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("darkcyan"), 6.0f /*width*/, this);
  m_stateSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Null, QColor("transparent"), stateOutline, this);
}

void QueryMapImageSublayer::addResultsAsGraphics(FeatureQueryResult* result, Symbol* symbol)
{
  // get the iterator of features
  FeatureIterator iter = result->iterator();

  // Create local parent
  QObject localParent;

  // add a graphic for each feature
  while (iter.hasNext())
  {
    Feature* feat = iter.next(&localParent);
    Graphic* graphic = new Graphic(feat->geometry(), symbol, this);
    m_selectionOverlay->graphics()->append(graphic);
  }
}

void QueryMapImageSublayer::query(const QString& whereClause)
{
  if (!m_citiesTable || !m_countiesTable || !m_statesTable || !m_mapView || !m_selectionOverlay)
    return;

  // clear & delete previous graphics
  GraphicListModel* graphics = m_selectionOverlay->graphics();
  const int graphicSize = graphics->size();
  for (int i = 0; i < graphicSize; i++)
  {
    delete graphics->at(i);
  }
  graphics->clear();

  // create the parameters
  QueryParameters queryParams;
  queryParams.setGeometry(m_mapView->currentViewpoint(ViewpointType::BoundingGeometry).targetGeometry().extent());
  queryParams.setWhereClause(whereClause);

  // query the feature tables
  m_citiesTable->queryFeatures(queryParams);
  m_countiesTable->queryFeatures(queryParams);
  m_statesTable->queryFeatures(queryParams);
}
