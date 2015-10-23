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

#include "Selection.h"
#include "Map.h"
#include "Basemap.h"
#include "MapView.h"
#include "Viewpoint.h"
#include "SpatialReference.h"
#include "FeatureLayer.h"
#include "ServiceFeatureTable.h"
#include "QueryParameters.h"
#include <QGraphicsProxyWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

using namespace Esri::ArcGISRuntime;

Selection::Selection(QWidget* parent) :
  QWidget(parent),
  m_map(nullptr),
  m_mapView(nullptr),
  m_featureLayer(nullptr),
  m_featureTable(nullptr),
  m_selectionResult(nullptr)
{
  m_selectionResult = new QLabel("Tap/Click to select features", this);

  // create a new basemap instance
  m_map = new Map(Basemap::streets(this), this);
  // set the initial extent
  m_map->setInitialViewpoint(Viewpoint(Envelope(-1131596.019761, 3893114.069099, 3926705.982140, 7977912.461790, SpatialReference::webMercator())));
  // create a new mapview instance
  m_mapView = new MapView(m_map, this);

  // create the feature layer from a feature table
  m_featureTable = new ServiceFeatureTable(QUrl("http://sampleserver6.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer/0"), this);
  m_featureLayer = new FeatureLayer(m_featureTable, this);

  // set a selection width and color on the feature layer
  m_featureLayer->setSelectionColor("cyan");
  m_featureLayer->setSelectionWidth(3);

  // once the selection on the feature layer is done
  connect(m_featureLayer, &FeatureLayer::selectFeaturesCompleted, [this](QUuid taskId, QSharedPointer<FeatureQueryResult> queryResult)
  {
    m_queryResult = queryResult;
    onSelectionQueryComplete(taskId);
  });

  // add the featurelayer to the map's operational layers
  m_map->operationalLayers()->append(m_featureLayer);

  // lambda expression for the mouse press event on the mapview
  connect(m_mapView, &MapView::mousePressRelease, [this](QMouseEvent& mouseEvent)
  {
    // get the point from the mouse point
     Point mapPoint = m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y());
     qDebug() << "X: " << mapPoint.x() << "; Y: " << mapPoint.y();

     // create a query parameter object
     QueryParameters queryParams;
     QStringList outFields;
     queryParams.setOutFields(outFields << "*");

     double tolerance = 22;
     double mapTolerance = tolerance * m_mapView->unitsPerPixel();
     // create an envelope based on the tolerance
     Envelope env(mapPoint.x() - mapTolerance, mapPoint.y() - mapTolerance, mapPoint.x() + mapTolerance, mapPoint.y() + mapTolerance, SpatialReference(102100));

     // set the envelope as the geometry of the query param
     queryParams.setGeometry(env);
     m_featureLayer->selectFeatures(queryParams, SelectionMode::New);
  });

  // create the app ui
  createUi();
}

// function to count the number of selected features
void Selection::onSelectionQueryComplete(QUuid taskId)
{
  Q_UNUSED(taskId)

  // return if no features selected
  if (!m_queryResult->iterator().hasNext())
  {
    m_selectionResult->setText("No features selected");
    return;
  }
  else // if more than one feature selected
  {
    auto count = 0;
    while (m_queryResult->iterator().hasNext())
    {
      // increment the count
      ++count;
      // increment the iterator
      m_queryResult->iterator().next();
    }

    // print the count
    QString labelText;
    labelText = count > 1 ? QString::number(count) + " features selected." : QString::number(count) + " feature selected.";
    m_selectionResult->setText(labelText);
  }
}

// create and add the UI elements
void Selection::createUi()
{
  QWidget* widget = new QWidget();
  QVBoxLayout* layout = new QVBoxLayout();
  layout->setMargin(10);
  layout->addWidget(m_selectionResult);
  widget->setPalette(QPalette(QPalette::Base));
  widget->setLayout(layout);

  QGraphicsProxyWidget* proxy = m_mapView->scene()->addWidget(widget);
  proxy->setPos(10, 10);
  proxy->setOpacity(0.95);

  QVBoxLayout* vBoxLayout = new QVBoxLayout();
  vBoxLayout->addWidget(m_mapView);
  setLayout(vBoxLayout);
}

// destructor
Selection::~Selection()
{
}
