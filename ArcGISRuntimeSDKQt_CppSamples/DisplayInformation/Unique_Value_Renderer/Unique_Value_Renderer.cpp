// [WriteFile Name=Unique_Value_Renderer, Category=DisplayInformation]
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


#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "Unique_Value_Renderer.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "ServiceFeatureTable.h"
#include "FeatureLayer.h"
#include "UniqueValueRenderer.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "Viewpoint.h"
#include "Envelope.h"

#include <QUrl>
#include <QStringList>
#include <QVariantList>


using namespace Esri::ArcGISRuntime;

Unique_Value_Renderer::Unique_Value_Renderer(QQuickItem* parent) :
  QQuickItem(parent)
{
}

Unique_Value_Renderer::~Unique_Value_Renderer() = default;

void Unique_Value_Renderer::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<Unique_Value_Renderer>("Esri.Samples", 1, 0, "Unique_Value_RendererSample");
}

void Unique_Value_Renderer::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // create a new basemap instance
  Basemap* basemap = Basemap::topographic(this);
  // create a new map instance
  m_map = new Map(basemap, this);

  // create featureLayer with URL
  m_featureTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/USA/MapServer/2"), this);
  m_featureLayer = new FeatureLayer(m_featureTable, this);
  m_map->operationalLayers()->append(m_featureLayer);

  // create unique renderer
  m_uniqueValueRenderer = new UniqueValueRenderer(this);
  // you can add multiple fields. In this case, only one is used
  m_uniqueValueRenderer->setFieldNames(QStringList("STATE_ABBR"));

  // create symbols to be used in the renderer
  m_defaultSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Null, QColor("black"), new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("gray"), 2, this), this);
  m_californiaSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("red"), new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("red"), 2, this), this);
  m_arizonaSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("green"), new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("green"), 2, this), this);
  m_nevadaSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("blue"), new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("blue"), 2, this), this);

  // create unique values and add to the UniqueValueRenderer
  createUniqueValue("CA", m_californiaSymbol);
  createUniqueValue("AZ", m_arizonaSymbol);
  createUniqueValue("NV", m_nevadaSymbol);

  // set default symbol
  m_uniqueValueRenderer->setDefaultSymbol(m_defaultSymbol);

  // set the renderer on the feature layer
  m_featureLayer->setRenderer(m_uniqueValueRenderer);

  // set initial viewpoint
  m_map->setInitialViewpoint(Viewpoint(Envelope(-13893029.0, 3573174.0, -12038972.0, 5309823.0, SpatialReference::webMercator())));

  // set map on the map view
  m_mapView->setMap(m_map);
}

void Unique_Value_Renderer::createUniqueValue(QString stateName, SimpleFillSymbol* fillSymbol)
{
  // add state's attribute value for field "STATE_ABBR" to QVariantList
  QVariantList stateValue;
  stateValue.append(stateName);

  // set value for a State to be rendered. (label, description, attribute value list, symbol, parent)
  UniqueValue* uniqueValue = new UniqueValue(stateName, "The State of " + stateName, stateValue, fillSymbol, this);
  // append to UniqueValueRenderer
  m_uniqueValueRenderer->uniqueValues()->append(uniqueValue);
}
