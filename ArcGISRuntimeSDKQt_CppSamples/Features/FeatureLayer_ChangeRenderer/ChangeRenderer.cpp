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

#include "ChangeRenderer.h"
#include "Map.h"
#include "MapView.h"
#include "FeatureLayer.h"
#include "Basemap.h"
#include "SpatialReference.h"
#include "ServiceFeatureTable.h"
#include "SimpleLineSymbol.h"
#include "SimpleRenderer.h"
#include "QPushButton"
#include <QGraphicsProxyWidget>
#include <QVBoxLayout>

using namespace Esri::ArcGISRuntime;

ChangeRenderer::ChangeRenderer(QWidget* parent) :
  QWidget(parent),
  m_map(nullptr),
  m_mapView(nullptr),
  m_featureLayer(nullptr),
  m_changeRendererButton(nullptr)
{
  m_changeRendererButton = new QPushButton("Change Renderer", this);
  m_changeRendererButton->setStyleSheet("QPushbutton#text {color: black;}");
  m_changeRendererButton->setEnabled(false);
  connect (m_changeRendererButton, SIGNAL(clicked()), this, SLOT(onChangeRendererClicked()));

  // Create a map using the imagery with labels basemap
  m_map = new Map(Basemap::topographic(this), this);

  // Create a map view, and pass in the map
  m_mapView = new MapView(m_map, this);

  // create the feature table
  ServiceFeatureTable* featureTable = new ServiceFeatureTable(QUrl("http://sampleserver6.arcgisonline.com/arcgis/rest/services/PoolPermits/FeatureServer/0"), this);
  // create the feature layer using the feature table
  m_featureLayer = new FeatureLayer(featureTable, this);

  connect(featureTable, &ServiceFeatureTable::doneLoading,[this](){
    // zoom to a specific area
    m_mapView->setViewpoint(Viewpoint(Envelope(-13075816.4047166, 4014771.46954516, -13073005.6797177, 4016869.78617381, SpatialReference(102100))));
    // enable the button once the layer is loaded
    m_changeRendererButton->setEnabled(true);
  });

  // add the feature layer to the map
  m_map->operationalLayers()->append(m_featureLayer);

  createUi();
}

void ChangeRenderer::onChangeRendererClicked()
{
  // create a line symbol
  SimpleLineSymbol* sls = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("blue"), 2.0f, true, 0.8f, this);
  // create a renderer with the symbol above
  SimpleRenderer* simpleRenderer = new SimpleRenderer(sls, this);
  // change the renderer with the renderer created above
  m_featureLayer->setRenderer(simpleRenderer);
}

void ChangeRenderer::createUi()
{
  QWidget* widget = new QWidget();
  QVBoxLayout* layout = new QVBoxLayout();
  layout->setMargin(10);
  layout->addWidget(m_changeRendererButton);
  widget->setPalette(QPalette(QPalette::Base));
  widget->setLayout(layout);

  QGraphicsProxyWidget* proxy = m_mapView->scene()->addWidget(widget);
  proxy->setPos(10, 10);
  proxy->setOpacity(0.95);

  QVBoxLayout* vBoxLayout = new QVBoxLayout();
  vBoxLayout->addWidget(m_mapView);
  setLayout(vBoxLayout);
}

ChangeRenderer::~ChangeRenderer()
{
}
