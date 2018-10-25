// [WriteFile Name=UpdateGeometryFeatureService, Category=EditData]
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

#include "UpdateGeometryFeatureService.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "Viewpoint.h"
#include "Point.h"
#include "SpatialReference.h"
#include "ServiceFeatureTable.h"
#include "FeatureLayer.h"
#include "Feature.h"
#include "FeatureEditResult.h"
#include "GeoElement.h"
#include <QUrl>
#include <QUuid>
#include <QMouseEvent>
#include <QList>

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

UpdateGeometryFeatureService::UpdateGeometryFeatureService(QQuickItem* parent) :
  QQuickItem(parent)
{
}

UpdateGeometryFeatureService::~UpdateGeometryFeatureService() = default;
void UpdateGeometryFeatureService::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<UpdateGeometryFeatureService>("Esri.Samples", 1, 0, "UpdateGeometryFeatureServiceSample");
}

void UpdateGeometryFeatureService::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

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

void UpdateGeometryFeatureService::connectSignals()
{
  // connect to the mouse clicked signal on the MapQuickView
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    // get the point from the mouse point
    Point mapPoint = m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y());

    // if a feature is already selected, move the selected feature to the new geometry
    if (m_featureSelected)
    {
      // set the selected feature's geometry to the tapped map point
      m_selectedFeature->setGeometry(mapPoint);

      // update the feature table with the new feature
      m_featureTable->updateFeature(m_selectedFeature);

      // reset the feature layer
      m_featureLayer->clearSelection();
      m_featureSelected = false;
    }

    // else select a new feature
    else
    {
      // first clear the selection
      m_featureLayer->clearSelection();

      // call identify on the map view
      m_mapView->identifyLayer(m_featureLayer, mouseEvent.x(), mouseEvent.y(), 5, false, 1);
    }
  });

  // connect to the identifyLayerCompleted signal on the map view
  connect(m_mapView, &MapQuickView::identifyLayerCompleted, this, [this](QUuid, IdentifyLayerResult* identifyResult)
  {
    if(!identifyResult)
      return;

    if (identifyResult->geoElements().size() > 0)
    {
      // first delete if not nullptr
      if (m_selectedFeature)
        delete m_selectedFeature;

      m_selectedFeature = static_cast<Feature*>(identifyResult->geoElements().at(0));
      // Prevent the feature from being deleted along with the identifyResult.
      m_selectedFeature->setParent(this);
      
      // select the item in the result
      m_featureLayer->selectFeature(m_selectedFeature);
      m_featureSelected = true;
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
  connect(m_featureTable, &ServiceFeatureTable::applyEditsCompleted, this, [](QUuid, const QList<FeatureEditResult*>& featureEditResults)
  {
    // Lock is a convenience wrapper that deletes the contents of featureEditResults when we leave scope.
    FeatureEditListResultLock lock(featureEditResults);

    // obtain the first item in the list
    FeatureEditResult* featureEditResult = lock.results.isEmpty() ? nullptr : lock.results.first();
    // check if there were errors, and if not, log the new object ID
    if (featureEditResult && !featureEditResult->isCompletedWithErrors())
      qDebug() << "Successfully updated geometry for Object ID:" << featureEditResult->objectId();
    else
      qDebug() << "Apply edits error.";
  });
}
