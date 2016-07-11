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
    QQuickItem(parent),
    m_map(nullptr),
    m_mapView(nullptr),
    m_featureLayer(nullptr),
    m_featureTable(nullptr),
    m_defaultSymbol(nullptr),
    m_californiaSymbol(nullptr),
    m_arizonaSymbol(nullptr),
    m_nevadaSymbol(nullptr),
    m_uniqueValueRenderer(nullptr)
{
}

Unique_Value_Renderer::~Unique_Value_Renderer()
{
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
    m_featureTable = new ServiceFeatureTable(QUrl("http://sampleserver6.arcgisonline.com/arcgis/rest/services/USA/MapServer/2"), this);
    m_featureLayer = new FeatureLayer(m_featureTable, this);
    m_map->operationalLayers()->append(m_featureLayer);

    // create unique renderer
    m_uniqueValueRenderer = new UniqueValueRenderer(this);
    // can add multiple fields. only using one in this sample
    m_uniqueValueRenderer->setFieldNames(QStringList("STATE_NAME"));

    // create symbols to be used in the renderer
    m_defaultSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Null, QColor("black"), new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("gray"), 2, this), this);
    m_californiaSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("red"), new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("red"), 2, this), this);
    m_arizonaSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("green"), new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("green"), 2, this), this);
    m_nevadaSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("blue"), new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("blue"), 2, this), this);

    // set default symbol
    m_uniqueValueRenderer->setDefaultSymbol(m_defaultSymbol);

    // set value for California. define: (label, description, attribute value, symbol, parent)
    QVariantList californiaValue;
    californiaValue.append("California");
    m_uniqueValueRenderer->uniqueValues()->append(new UniqueValue("California", "The State of California", californiaValue, m_californiaSymbol, this));

    // set value for Arizona
    QVariantList arizonaValue;
    arizonaValue.append("Arizona");
    m_uniqueValueRenderer->uniqueValues()->append(new UniqueValue("Arizona", "The State of Arizona", arizonaValue, m_arizonaSymbol, this));

    // set value for Nevada
    QVariantList nevadaValue;
    nevadaValue.append("Nevada");
    m_uniqueValueRenderer->uniqueValues()->append(new UniqueValue("Nevada", "The State of Nevada", nevadaValue, m_nevadaSymbol, this));

    // set the renderer on the feature layer
    m_featureLayer->setRenderer(m_uniqueValueRenderer);

    // set initial viewpoint
    m_map->setInitialViewpoint(Viewpoint(Envelope(-13893029.0, 3573174.0, -12038972.0, 5309823.0, SpatialReference::webMercator())));

    // set map on the map view
    m_mapView->setMap(m_map);
}
