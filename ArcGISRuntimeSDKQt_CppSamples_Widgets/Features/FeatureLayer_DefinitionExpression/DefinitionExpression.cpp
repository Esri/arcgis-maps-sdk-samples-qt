// [WriteFile Name=DefinitionExpression, Category=Features]
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

#include "DefinitionExpression.h"
#include "Map.h"
#include "MapGraphicsView.h"
#include "Viewpoint.h"
#include "FeatureLayer.h"
#include "SpatialReference.h"
#include "ServiceFeatureTable.h"
#include <QGraphicsProxyWidget>
#include <QVBoxLayout>
#include <QPushButton>

using namespace Esri::ArcGISRuntime;

DefinitionExpression::DefinitionExpression(QWidget* parent) :
  QWidget(parent),
  m_map(nullptr),
  m_mapView(nullptr),
  m_featureLayer(nullptr),
  m_applyExpressionButton(nullptr),
  m_resetButton(nullptr)
{
  QString style = "QPushbutton#text {color: black;}";
  m_applyExpressionButton = new QPushButton("Apply expression", this);
  m_applyExpressionButton->setStyleSheet(style);
  m_applyExpressionButton->setEnabled(false);

  m_resetButton = new QPushButton("Reset", this);
  m_resetButton->setStyleSheet(style);
  m_resetButton->setEnabled(false);

  // Create a map using the imagery with labels basemap
  m_map = new Map(Basemap::topographic(this), this);

  // Create a map view, and pass in the map
  m_mapView = new MapGraphicsView(m_map, this);

  // create the feature table
  ServiceFeatureTable* featureTable = new ServiceFeatureTable(QUrl("http://sampleserver6.arcgisonline.com/arcgis/rest/services/SF311/FeatureServer/0"), this);
  // create the feature layer using the feature table
  m_featureLayer = new FeatureLayer(featureTable, this);

  connect(m_featureLayer, &FeatureLayer::doneLoading,[this]()
  {
    // zoom to a specific area
    m_mapView->setViewpointCenter(Point(-13630484, 4545415, SpatialReference(102100)), 300000);
    // enable the button once the layer is loaded
    m_applyExpressionButton->setEnabled(true);
    m_resetButton->setEnabled(true);
  });

  // lamda expression for the expression button click
  connect(m_applyExpressionButton, &QPushButton::clicked, [this](){
    // set a definition expression
    m_featureLayer->setDefinitionExpression(QString("req_Type = \'Tree Maintenance or Damage\'"));
  });

  // lambda expression for the reset button click
  connect(m_resetButton, &QPushButton::clicked, [this](){
    // remove the definition expression
    m_featureLayer->setDefinitionExpression(QString(""));
  });

  // add the feature layer to the map
  m_map->operationalLayers()->append(m_featureLayer);

  createUi();
}

// create and add the UI elements
void DefinitionExpression::createUi()
{
  QWidget* widget = new QWidget();
  QVBoxLayout* layout = new QVBoxLayout();
  layout->setMargin(10);
  layout->addWidget(m_applyExpressionButton);
  layout->addWidget(m_resetButton);
  widget->setPalette(QPalette(QPalette::Base));
  widget->setLayout(layout);

  QGraphicsProxyWidget* proxy = m_mapView->scene()->addWidget(widget);
  proxy->setPos(10, 10);
  proxy->setOpacity(0.95);

  QVBoxLayout* vBoxLayout = new QVBoxLayout();
  vBoxLayout->addWidget(m_mapView);
  setLayout(vBoxLayout);
}

DefinitionExpression::~DefinitionExpression()
{
}
