// [WriteFile Name=ListRelatedFeatures, Category=Features]
// [Legal]
// Copyright 2017 Esri.

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

#include "ListRelatedFeatures.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "FeatureLayer.h"
#include "ViewInsets.h"
#include "Envelope.h"
#include "QueryParameters.h"
#include "ArcGISFeature.h"
#include "ArcGISFeatureTable.h"
#include "RelatedFeatureQueryResult.h"
#include "FeatureQueryResult.h"

#include "RelatedFeature.h"
#include "RelatedFeatureListModel.h"

#include <QList>
#include <QUrl>

using namespace Esri::ArcGISRuntime;

ListRelatedFeatures::ListRelatedFeatures(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void ListRelatedFeatures::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ListRelatedFeatures>("Esri.Samples", 1, 0, "ListRelatedFeaturesSample");
  qmlRegisterType<ViewInsets>();
  qmlRegisterUncreatableType<RelatedFeatureListModel>("Esri.ArcGISRuntimeSamples", 1, 0,
                                                      "RelatedFeatureListModel", "RelatedFeatureListModel is an uncreatable type");
}

void ListRelatedFeatures::componentComplete()
{
  QQuickItem::componentComplete();

  // Create a list model to store the related features
  m_relatedFeaturesModel = new RelatedFeatureListModel(this);
  emit relatedFeaturesModelChanged();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the URL of a web map
  m_map = new Map(QUrl("https://arcgis.com/home/item.html?id=dcc7466a91294c0ab8f7a094430ab437"), this);

  // Set map to map view
  m_mapView->setMap(m_map);

  // connect signals
  connectSignals();
}

void ListRelatedFeatures::connectSignals()
{
  // connect to doneLoading signal
  connect(m_map, &Map::doneLoading, this, [this](Error loadError)
  {
    if (!loadError.isEmpty())
      return;
    for (int i = 0; i < m_map->operationalLayers()->size(); i++)
    {
      // get the Alaska National Parks layer
      if (m_map->operationalLayers()->at(i)->name().contains(QString("- Alaska National Parks")))
      {
        m_alaskaNationalParks = static_cast<FeatureLayer*>(m_map->operationalLayers()->at(i));
        m_alaskaNationalParks->setSelectionColor(QColor("yellow"));
        m_alaskaNationalParks->setSelectionWidth(5.0);

        // connect to selectFeaturesCompleted signal
        connect(m_alaskaNationalParks, &FeatureLayer::selectFeaturesCompleted, this, [this](QUuid, FeatureQueryResult* result)
        {
          // iterate over features returned
          while (result->iterator().hasNext())
          {
            ArcGISFeature* arcGISFeature = static_cast<ArcGISFeature*>(result->iterator().next(this));
            ArcGISFeatureTable* selectedTable = static_cast<ArcGISFeatureTable*>(arcGISFeature->featureTable());

            // connect to queryRelatedFeaturesCompleted signal
            connect(selectedTable, &ArcGISFeatureTable::queryRelatedFeaturesCompleted, this, [this, arcGISFeature](QUuid, QList<RelatedFeatureQueryResult*> relatedResults)
            {
              foreach (RelatedFeatureQueryResult* relatedResult, relatedResults)
              {
                while (relatedResult->iterator().hasNext())
                {
                  // get the related feature
                  ArcGISFeature* feature = static_cast<ArcGISFeature*>(relatedResult->iterator().next(this));
                  ArcGISFeatureTable* relatedTable = static_cast<ArcGISFeatureTable*>(feature->featureTable());
                  QString displayFieldName = relatedTable->layerInfo().displayFieldName();
                  QString serviceLayerName = relatedTable->layerInfo().serviceLayerName();
                  QString displayFieldValue = feature->attributes()->attributeValue(displayFieldName).toString();

                  // add the related feature to the list model
                  RelatedFeature relatedFeature = RelatedFeature(displayFieldName,
                                                                 displayFieldValue,
                                                                 serviceLayerName);
                  m_relatedFeaturesModel->addRelatedFeature(relatedFeature);
                  emit relatedFeaturesModelChanged();
                }
              }

              emit showAttributeTable();
              m_mapView->setViewpointGeometry(arcGISFeature->geometry(), 100);
            });

            // query related FeatureServiceCapabilities
            selectedTable->queryRelatedFeatures(arcGISFeature);
          }
        });
      }
    }
  });


  // connect to the mouseClicked signal
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent mouseEvent)
  {
    // hide the attribute view
    emit hideAttributeTable();

    // clear the list model
    m_relatedFeaturesModel->clear();

    // create objects required to do a selection with a query
    Point clickPoint = m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y());
    double mapTolerance = 10 * m_mapView->unitsPerDIP();
    Envelope envelope = Envelope(clickPoint.x() - mapTolerance,
                                 clickPoint.y() - mapTolerance,
                                 clickPoint.x() + mapTolerance,
                                 clickPoint.y() + mapTolerance,
                                 m_map->spatialReference());
    QueryParameters queryParams;
    queryParams.setGeometry(envelope);
    queryParams.setSpatialRelationship(SpatialRelationship::Intersects);

    // clear any selections
    m_alaskaNationalParks->clearSelection();

    // select features
    m_alaskaNationalParks->selectFeatures(queryParams, SelectionMode::New);
  });
}
