// [WriteFile Name=EditWithBranchVersioning, Category=EditData]
// [Legal]
// Copyright 2020 Esri.

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

#include "EditWithBranchVersioning.h"

#include "ArcGISFeatureServiceInfo.h"
#include "AttributeListModel.h"
#include "AuthenticationManager.h"
#include "Credential.h"
#include "FeatureEditResult.h"
#include "FeatureLayer.h"
#include "FeatureTableEditResult.h"
#include "Layer.h"
#include "Map.h"
#include "MapQuickView.h"
#include "ServiceFeatureTable.h"
#include "ServiceGeodatabase.h"
#include "ServiceVersionInfo.h"
#include "ServiceVersionParameters.h"

using namespace Esri::ArcGISRuntime;

EditWithBranchVersioning::EditWithBranchVersioning(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::streetsVector(this), this))
{
  m_cred = new Credential{"editor01", "editor01.password", this};
//  m_cred = new Credential{"editor02", "editor02.password", this};
}

EditWithBranchVersioning::~EditWithBranchVersioning() = default;

void EditWithBranchVersioning::init()
{
  // Register the map view for QML
  qmlRegisterUncreatableType<AuthenticationManager>("Esri.Samples", 1, 0, "AuthenticationManager", "AuthenticationManager is uncreateable");
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<EditWithBranchVersioning>("Esri.Samples", 1, 0, "EditWithBranchVersioningSample");
}

MapQuickView* EditWithBranchVersioning::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void EditWithBranchVersioning::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  m_busy = true;
  emit busyChanged();

  // When map is done loading set up service geodatabse signals and map signals
  connect(m_map, &Map::doneLoading, this, &EditWithBranchVersioning::onMapDoneLoading);

  emit mapViewChanged();
}

AuthenticationManager *EditWithBranchVersioning::authManager() const
{
  return AuthenticationManager::instance();
}

void EditWithBranchVersioning::onMapDoneLoading(Error error)
{
  if (!error.isEmpty())
    return;

  // connect all needed service geodatabase signals
  connectSgdbSignals();

  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    m_busy = true;
    emit busyChanged();
    // first clear the selection
    m_featureLayer->clearSelection();

    emit hideWindow();

    // if feature is already selected, then move to new location
    if (m_selectedFeature)
    {
      const Point clickedPoint = m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y());
      moveFeature(clickedPoint);
    }
    else
    {
      // call identify on the map view
      m_mapView->identifyLayer(m_featureLayer, mouseEvent.x(), mouseEvent.y(), 5, false, 1);
    }
  });

  connect(m_mapView, &MapQuickView::viewpointChanged, this, [this]()
  {
    if (!m_featureLayer)
      return;

    m_featureLayer->clearSelection();
    emit hideWindow();
  });

  // connect to the identifyLayerCompleted signal on the map view
  connect(m_mapView, &MapQuickView::identifyLayerCompleted, this, [this](QUuid, IdentifyLayerResult* identifyResult)
  {
    if(!identifyResult)
    {
      clearSelectedFeature();
      return;
    }
    if (!identifyResult->geoElements().empty())
    {

      m_selectedFeature = static_cast<ArcGISFeature*>(identifyResult->geoElements().at(0));
      qDebug() << m_selectedFeature->attributes()->size();
      // select the item in the result
      m_featureLayer->selectFeature(static_cast<Feature*>(identifyResult->geoElements().at(0)));
      QueryParameters queryParams;
      QString whereClause = "objectid=" + identifyResult->geoElements().at(0)->attributes()->attributeValue("objectid").toString();
      queryParams.setWhereClause(whereClause);
      m_featureTable->queryFeatures(queryParams, QueryFeatureFields::LoadAll);
    }
    m_busy = false;
    emit busyChanged();
  });
}

void EditWithBranchVersioning::connectSgdbSignals()
{
  m_serviceGeodatabase = new ServiceGeodatabase(QUrl("https://sampleserver7.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer"), m_cred,this);
//  m_serviceGeodatabase = new ServiceGeodatabase(QUrl("https://sampleserver7.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer"), this);
  m_busy = true;
  emit busyChanged();

  connect(m_serviceGeodatabase, &ServiceGeodatabase::doneLoading, this, &EditWithBranchVersioning::onSgdbDoneLoadingCompleted);

  connect(m_serviceGeodatabase, &ServiceGeodatabase::createVersionCompleted, this, &EditWithBranchVersioning::onCreateVersionCompleted);

  connect(m_serviceGeodatabase, &ServiceGeodatabase::applyEditsCompleted, this, [this]()
  {
    m_busy = false;
    emit busyChanged();
  });

  connect(m_serviceGeodatabase, &ServiceGeodatabase::errorOccurred, this, [this](Error error)
  {
    m_errorMessage = error.message() + " - " + error.additionalMessage();
    emit errorMessageChanged();

  });

  connect(m_serviceGeodatabase, &ServiceGeodatabase::switchVersionCompleted, this, [this] (QUuid)
  {
    m_busy = false;
    emit busyChanged();
    // set the current version name for the UI
    m_sgdbCurrentVersion = m_serviceGeodatabase->versionName();
    // if the current version is the default, prevent editing
    m_allowEditing = m_sgdbCurrentVersion == "sde.DEFAULT" ? false : true;
    clearSelectedFeature();
    emit allowEditingChanged();
    emit sgdbCurrentVersionChanged();
  });

  m_serviceGeodatabase->load();
}

void EditWithBranchVersioning::onSgdbDoneLoadingCompleted(Error error)
{
  if (!error.isEmpty())
    return;

  // created service feature table from the table contained in the service geodatabase
  m_featureTable = m_serviceGeodatabase->table(0);

  // set allow editing to false as the initial version loaded is the default
  m_allowEditing = false;
  emit allowEditingChanged();

  connect(m_featureTable, &ServiceFeatureTable::doneLoading, this, [this](Error error)
  {
    if (!error.isEmpty())
      return;

    // once the service feature table is loaded set the mapview extent to the full extent of the feature layer
    m_mapView->setViewpoint(m_featureLayer->fullExtent());

    m_busy = false;
    emit busyChanged();
  });

  connect(m_featureTable, &ServiceFeatureTable::updateFeatureCompleted, this, [this] (QUuid, bool success)
  {
    m_busy = false;
    emit busyChanged();
    // once update featuers has completed we can apply the edits back to our version
//    m_serviceGeodatabase->applyEdits();
  });

  // create a feature layer from the service feature table
  m_featureLayer = new FeatureLayer(m_featureTable, this);

  // add the feature layer to the map
  m_map->operationalLayers()->append(m_featureLayer);

  // update current version for UI
  m_sgdbCurrentVersion = m_serviceGeodatabase->versionName();
  emit sgdbCurrentVersionChanged();

  connect(m_featureTable, &FeatureTable::queryFeaturesCompleted, this, [this](QUuid, FeatureQueryResult* featureQueryResult)
  {
    if (featureQueryResult && featureQueryResult->iterator().hasNext())
    {
      // first delete if not nullptr
      if (m_selectedFeature)
      {
        clearSelectedFeature();
      }

      // set selected feature member
      m_selectedFeature = static_cast<ArcGISFeature*>(featureQueryResult->iterator().next(this));
      m_selectedFeature->setParent(this);
      // Obtain placename attribute for the callout title
      const QString featureName = m_selectedFeature->attributes()->attributeValue("PLACENAME").toString();
      m_mapView->calloutData()->setTitle(QString("<br><font size=\"+2\"><b>%1</b></font>").arg(featureName));
      m_mapView->calloutData()->setLocation(m_selectedFeature->geometry().extent().center());

      m_currentTypeDamage = m_selectedFeature->attributes()->attributeValue("TYPDAMAGE").toString();
      emit currentTypeDamageChanged();
      emit featureSelected();
    }
  });
}

void EditWithBranchVersioning::onCreateVersionCompleted(QUuid, Esri::ArcGISRuntime::ServiceVersionInfo* serviceVersionInfo)
{
  // check for local edits before switching versions
  if (m_serviceGeodatabase->hasLocalEdits())
    return;

  // ensure the created version was succesful
  if (!serviceVersionInfo)
    return;

  m_busy = false;
  emit busyChanged();
  emit createVersionSuccess();

  // store create version name for easy switching between default and created version
  m_createdVersionName = serviceVersionInfo->name();

  // switch to the version you just created
  m_serviceGeodatabase->switchVersion(m_createdVersionName);
}

void EditWithBranchVersioning::switchVersion()
{
  m_busy = true;
  emit busyChanged();

//  if (m_serviceGeodatabase->hasLocalEdits())
//  {
//    // apply local edits before switching versions
//    m_serviceGeodatabase->applyEdits();
//  }

  // if the current version is our created version switch to the default
  if (m_sgdbCurrentVersion == m_createdVersionName)
  {
    if (m_serviceGeodatabase->hasLocalEdits())
    {
      // apply local edits before switching versions
      m_serviceGeodatabase->applyEdits();
    }

    m_serviceGeodatabase->switchVersion(m_serviceGeodatabase->defaultVersionName());
  }
  else
    m_serviceGeodatabase->switchVersion(m_createdVersionName);
}

void EditWithBranchVersioning::updateAttribute(const QString& attributeValue)
{
  m_busy = true;
  emit busyChanged();
  if (!m_selectedFeature)
    return;

  if (sgdbVerionIsDefault())
  {
    clearSelectedFeature();
    return;
  }

  // update the attirbute with the selection from the combo box
  m_selectedFeature->attributes()->replaceAttribute("TYPDAMAGE", attributeValue);
  m_selectedFeature->featureTable()->updateFeature(m_selectedFeature);
}

void EditWithBranchVersioning::createVersion(const QString& versionName, const QString& versionAccess, const QString& description)
{
  m_busy = true;
  emit busyChanged();
  // create parameters for version from the inputed fields from the UI
  ServiceVersionParameters* params = new ServiceVersionParameters(this);
  params->setName(versionName);
  params->setDescription(description);

  if (versionAccess == "Private")
    params->setAccess(VersionAccess::Private);
  else if (versionAccess == "Protected")
    params->setAccess(VersionAccess::Protected);
  else
    params->setAccess(VersionAccess::Public);

  m_serviceGeodatabase->createVersion(params);
}

void EditWithBranchVersioning::moveFeature(const Point& mapPoint)
{
  if (sgdbVerionIsDefault())
  {
    clearSelectedFeature();
    return;
  }
  const Polyline geom = m_selectedFeature->geometry();
  m_selectedFeature->setGeometry(mapPoint);

  // update the selected feature with the new geometry
  m_selectedFeature->featureTable()->updateFeature(m_selectedFeature);
  clearSelectedFeature();
}

void EditWithBranchVersioning::clearSelection()
{
  // helper function to clear feature layer selection
  if (m_featureLayer)
    m_featureLayer->clearSelection();
}

bool EditWithBranchVersioning::sgdbVerionIsDefault() const
{
  // helper function to check if the current version is the default version
  return m_serviceGeodatabase->versionName() == m_serviceGeodatabase->defaultVersionName() ? true : false;
}

void EditWithBranchVersioning::clearSelectedFeature()
{
  // helper function to clean up the selected feature when invalidated
  delete m_selectedFeature;
  m_selectedFeature = nullptr;
  clearSelection();
}
