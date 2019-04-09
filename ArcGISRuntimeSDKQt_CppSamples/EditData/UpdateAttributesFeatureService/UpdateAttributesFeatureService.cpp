// [WriteFile Name=UpdateAttributesFeatureService, Category=EditData]
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

#include "UpdateAttributesFeatureService.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "Viewpoint.h"
#include "Point.h"
#include "SpatialReference.h"
#include "ServiceFeatureTable.h"
#include "FeatureLayer.h"
#include "Feature.h"
#include "ArcGISFeature.h"
#include "FeatureEditResult.h"
#include "FeatureQueryResult.h"
#include <QUrl>
#include <QUuid>
#include <QMouseEvent>

using namespace Esri::ArcGISRuntime;

namespace
{
  // Convenience RAII struct that deletes all pointers in given container.
  struct FeatureEditListResultLock
  {
    FeatureEditListResultLock(const QList<FeatureEditResult*>& list) : results(list) { }
    ~FeatureEditListResultLock() { qDeleteAll(results); }
    const QList<FeatureEditResult*>& results;
  };
}

UpdateAttributesFeatureService::UpdateAttributesFeatureService(QQuickItem* parent) :
  QQuickItem(parent)
{    
}

UpdateAttributesFeatureService::~UpdateAttributesFeatureService() = default;

void UpdateAttributesFeatureService::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<UpdateAttributesFeatureService>("Esri.Samples", 1, 0, "UpdateAttributesFeatureServiceSample");
  qmlRegisterUncreatableType<CalloutData>("Esri.Samples", 1, 0, "CalloutData", "CalloutData is an uncreatable type");
}

void UpdateAttributesFeatureService::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);
  emit calloutDataChanged();

  // create a Map by passing in the Basemap
  m_map = new Map(Basemap::streets(this), this);
  m_map->setInitialViewpoint(Viewpoint(Point(-10800000, 4500000, SpatialReference(102100)), 3e7));

  // set map on the map view
  m_mapView->setMap(m_map);

  // create the ServiceFeatureTable
  m_featureTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer/0"), this);

  // create the FeatureLayer with the ServiceFeatureTable and add it to the Map
  m_featureLayer = new FeatureLayer(m_featureTable, this);
  m_map->operationalLayers()->append(m_featureLayer);

  connectSignals();
}

void UpdateAttributesFeatureService::connectSignals()
{   
  // connect to the mouse clicked signal on the MapQuickView
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    // first clear the selection
    m_featureLayer->clearSelection();

    // set the properties for qml
    emit hideWindow();

    // call identify on the map view
    m_mapView->identifyLayer(m_featureLayer, mouseEvent.x(), mouseEvent.y(), 5, false, 1);
  });

  // connect to the viewpoint changed signal on the MapQuickView
  connect(m_mapView, &MapQuickView::viewpointChanged, this, [this]()
  {
    m_featureLayer->clearSelection();
    emit hideWindow();
  });

  // connect to the identifyLayerCompleted signal on the map view
  connect(m_mapView, &MapQuickView::identifyLayerCompleted, this, [this](QUuid, IdentifyLayerResult* identifyResult)
  {
    if(!identifyResult)
      return;
    if (!identifyResult->geoElements().empty())
    {
      // select the item in the result
      m_featureLayer->selectFeature(static_cast<Feature*>(identifyResult->geoElements().at(0)));
      // Update the parent so the featureLayer is not deleted when the identifyResult is deleted.
      m_featureLayer->setParent(this);

      // obtain the selected feature with attributes
      QueryParameters queryParams;
      QString whereClause = "objectid=" + identifyResult->geoElements().at(0)->attributes()->attributeValue("objectid").toString();
      queryParams.setWhereClause(whereClause);
      m_featureTable->queryFeatures(queryParams);
    }
  });

  // connect to the queryFeaturesCompleted signal on the feature table
  connect(m_featureTable, &FeatureTable::queryFeaturesCompleted, this, [this](QUuid, FeatureQueryResult* featureQueryResult)
  {
    if (featureQueryResult && featureQueryResult->iterator().hasNext())
    {
      // first delete if not nullptr
      if (m_selectedFeature)
        delete m_selectedFeature;

      // set selected feature member
      m_selectedFeature = static_cast<ArcGISFeature*>(featureQueryResult->iterator().next(this));
      m_selectedFeature->setParent(this);
      m_featureType = m_selectedFeature->attributes()->attributeValue("typdamage").toString();
      m_mapView->calloutData()->setTitle(QString("<br><font size=\"+2\"><b>%1</b></font>").arg(m_featureType));
      m_mapView->calloutData()->setLocation(m_selectedFeature->geometry().extent().center());
      emit featureTypeChanged();
      emit featureSelected();
    }
  });

  // connect to the updateFeatureCompleted signal to determine if the update was successful
  connect(m_featureTable, &ServiceFeatureTable::updateFeatureCompleted, this, [this](QUuid, bool success)
  {
    // if the update was successful, call apply edits to apply to the service
    if (success)
      m_featureTable->applyEdits();
  });

  // connect to the applyEditsCompleted signal from the ServiceFeatureTable
  connect(m_featureTable, &ServiceFeatureTable::applyEditsCompleted, this, [this](QUuid, const QList<FeatureEditResult*>& featureEditResults)
  {
    // Lock is a convenience wrapper that deletes the contents of featureEditResults when we leave scope.
    FeatureEditListResultLock lock(featureEditResults);

    // check if result list is not empty
    if (!lock.results.isEmpty())
    {
      // obtain the first item in the list
      FeatureEditResult* featureEditResult = lock.results.first();
      // check if there were errors, and if not, log the new object ID
      if (!featureEditResult->isCompletedWithErrors())
        qDebug() << "Successfully updated attribute for Object ID:" << featureEditResult->objectId();
      else
        qDebug() << "Apply edits error.";
    }
    m_featureLayer->clearSelection();
  });
}

CalloutData* UpdateAttributesFeatureService::calloutData() const
{
 return m_mapView ? m_mapView->calloutData() : nullptr;
}

void UpdateAttributesFeatureService::updateSelectedFeature(QString fieldVal)
{
  // If the last connection is still hanging around we want to ensure it is disconnected.
  disconnect(m_featureLoadStatusChangedConnection);

  // connect to load status changed signal, remember the connection so we can kill it once
  // the slot has invoked.
  m_featureLoadStatusChangedConnection =
      connect(
          m_selectedFeature, &ArcGISFeature::loadStatusChanged,
          this, [this, fieldVal](Esri::ArcGISRuntime::LoadStatus)
                {
                  if (m_selectedFeature->loadStatus() == LoadStatus::Loaded)
                  {
                    // The conenction is invoked so we now forget all about this connection after this point.
                    disconnect(m_featureLoadStatusChangedConnection);

                    // update the select feature's attribute value
                    m_selectedFeature->attributes()->replaceAttribute("typdamage", fieldVal);

                    // update the feature in the feature table
                    m_featureTable->updateFeature(m_selectedFeature);
                  }
                }
  );

  // load selecte feature
  m_selectedFeature->load();
}

QString UpdateAttributesFeatureService::featureType() const
{
  return m_featureType;
}
