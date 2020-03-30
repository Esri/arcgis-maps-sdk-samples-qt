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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

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
#include <memory>

using namespace Esri::ArcGISRuntime;

namespace
{
  // Convenience RAII struct that deletes all pointers in given container.
  struct FeatureQueryListResultLock
  {
    FeatureQueryListResultLock(const QList<RelatedFeatureQueryResult*>& list) : results(list) { }
    ~FeatureQueryListResultLock() { qDeleteAll(results); }
    const QList<RelatedFeatureQueryResult*>& results;
  };
}

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
  m_mapView->setSelectionProperties(SelectionProperties(QColor(Qt::yellow)));

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

    bool foundLayer = false; // Found the Alaska National Parks layer.
    for (int i = 0; i < m_map->operationalLayers()->size() || !foundLayer; ++i)
    {
      // get the Alaska National Parks layer
      if (m_map->operationalLayers()->at(i)->name().contains(QStringLiteral("- Alaska National Parks")))
      {
        foundLayer = true;
        m_alaskaNationalParks = static_cast<FeatureLayer*>(m_map->operationalLayers()->at(i));
        m_alaskaFeatureTable = static_cast<ArcGISFeatureTable*>(m_alaskaNationalParks->featureTable());

        // connect to queryRelatedFeaturesCompleted signal
        connect(m_alaskaFeatureTable, &ArcGISFeatureTable::queryRelatedFeaturesCompleted,
                this, [this](QUuid, QList<RelatedFeatureQueryResult*> rawRelatedResults)
                {
                  FeatureQueryListResultLock lock(rawRelatedResults);
                  for (const RelatedFeatureQueryResult* relatedResult : lock.results)
                  {
                    while (relatedResult->iterator().hasNext())
                    {
                      // get the related feature
                      const ArcGISFeature* feature = static_cast<ArcGISFeature*>(relatedResult->iterator().next());
                      const ArcGISFeatureTable* relatedTable = static_cast<ArcGISFeatureTable*>(feature->featureTable());
                      const QString displayFieldName = relatedTable->layerInfo().displayFieldName();
                      const QString serviceLayerName = relatedTable->layerInfo().serviceLayerName();
                      const QString displayFieldValue = feature->attributes()->attributeValue(displayFieldName).toString();

                      // add the related feature to the list model
                      RelatedFeature relatedFeature = RelatedFeature(displayFieldName,
                                                                     displayFieldValue,
                                                                     serviceLayerName);
                      m_relatedFeaturesModel->addRelatedFeature(relatedFeature);
                      emit relatedFeaturesModelChanged();
                    }
                  }
                  emit showAttributeTable();
                });

        // connect to selectFeaturesCompleted signal
        connect(m_alaskaNationalParks, &FeatureLayer::selectFeaturesCompleted, this, [this](QUuid, FeatureQueryResult* rawResult)
        {
          auto result = std::unique_ptr<FeatureQueryResult>(rawResult);
          // The result could contain more than 1 feature, but we assume that
          // there is only ever 1. If more are given they are ignored. We
          // are only interested in the first (and only) feature.
          if (result->iterator().hasNext())
          {
            ArcGISFeature* arcGISFeature = static_cast<ArcGISFeature*>(result->iterator().next());

            // zoom to the selected feature
            m_mapView->setViewpointGeometry(arcGISFeature->geometry().extent(), 100);

            // query related features
            m_alaskaFeatureTable->queryRelatedFeatures(arcGISFeature);
          }
        });
      }
    }
  });


  // connect to the mouseClicked signal
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
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
