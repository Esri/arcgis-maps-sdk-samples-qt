// [Legal]
// Copyright 2015 Esri.

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

#include "Query.h"
#include "Map.h"
#include "MapView.h"
#include "Viewpoint.h"
#include "SpatialReference.h"
#include "FeatureLayer.h"
#include "ServiceFeatureTable.h"
#include "SimpleRenderer.h"
#include "SimpleLineSymbol.h"
#include "SimpleFillSymbol.h"
#include "QueryParameters.h"
#include <QGraphicsProxyWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>

using namespace Esri::ArcGISRuntime;

Query::Query(QWidget* parent) :
  QWidget(parent),
  m_map(nullptr),
  m_mapView(nullptr),
  m_featureLayer(nullptr),
  m_featureTable(nullptr),
  m_state(nullptr),
  m_queryButton(nullptr)
{
  QString style = "QPushbutton#text {color: black;}";
  m_state = new QLineEdit(this);
  m_state->setAlignment(Qt::AlignLeft);
  m_state->setStyleSheet("QLineEdit {color: black;}");
  m_state->setPlaceholderText(QString("Enter a state name. Eg: California or WA"));

  m_queryButton = new QPushButton("Query", this);
  m_queryButton->setStyleSheet(style);
  m_queryButton->setEnabled(false);
  connect(m_queryButton, SIGNAL(clicked()), this, SLOT(onQueryClicked()));

  // Create a map using the imagery with labels basemap
  m_map = new Map(Basemap::topographic(this), this);

  // Create a map view, and pass in the map
  m_mapView = new MapView(m_map, this);

  // create the feature table
  m_featureTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/USA/MapServer/2"), this);
  // create the feature layer using the feature table
  m_featureLayer = new FeatureLayer(m_featureTable, this);

  // line symbol for the outline
  SimpleLineSymbol* outline = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("black"), 2.0f, true, 1.0f, this);
  // fill symbol
  SimpleFillSymbol* sfs = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("yellow"), 0.6f, outline, this);
  // create the renderer using the symbology created above
  SimpleRenderer* renderer = new SimpleRenderer(sfs, this);
  // set the renderer for the feature layer
  m_featureLayer->setRenderer(renderer);

  connect(m_featureTable, &ServiceFeatureTable::doneLoading,[this](){
    // zoom to a specific area
    m_mapView->setViewpointCenter(Point(-11e6, 5e6, SpatialReference(102100)), 9e7);
    // enable the button once the layer is loaded
    m_queryButton->setEnabled(true);
  });

  // add the feature layer to the map
  m_map->operationalLayers()->append(m_featureLayer);

  createUi();
}

void Query::onQueryClicked()
{
  // create a query parameter object and set the where clause
  QueryParameters queryParams;
  queryParams.setWhereClause(QString("STATE_NAME LIKE \'" + m_state->text().toUpper() + "%\'"));

  // iterate over the query results once the query is done
  connect(m_featureTable, &ServiceFeatureTable::queryFeaturesCompleted, [this](QUuid, QSharedPointer<Esri::ArcGISRuntime::FeatureQueryResult> queryResult){
    if (!queryResult->iterator().hasNext())
      return;

    // clear any existing selection
    m_featureLayer->clearSelection();
    QList<Feature*> features;

    // iterate over the result object
    while(queryResult->iterator().hasNext())
    {
      auto feature = queryResult->iterator().next();
      feature->setParent(this);
      // add each feature to the list
      features.append(feature);
    }

    // make a selection with the features found. Ideally calling
    // calling selectFeatures using the query would do both querying and
    // selection at once.  We are just showing you how to do it in two steps.
    m_featureLayer->selectFeatures(features);
    // zoom to the first feature
    m_mapView->setViewpointGeometry(features.at(0)->geometry(), 200);
  });

  m_featureTable->queryFeatures(queryParams);
}

// create and add the UI elements
void Query::createUi()
{
  QWidget* widget = new QWidget();
  QHBoxLayout* layout = new QHBoxLayout();
  layout->setMargin(10);
  layout->addWidget(m_state);
  layout->addWidget(m_queryButton);
  widget->setPalette(QPalette(QPalette::Base));
  widget->setLayout(layout);

  QGraphicsProxyWidget* proxy = m_mapView->scene()->addWidget(widget);
  proxy->setPos(10, 10);
  proxy->setOpacity(0.95);

  QVBoxLayout* vBoxLayout = new QVBoxLayout();
  vBoxLayout->addWidget(m_mapView);
  setLayout(vBoxLayout);
}

Query::~Query()
{
}
