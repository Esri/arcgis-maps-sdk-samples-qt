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

#include "AuthenticationManager.h"
#include "Map.h"
#include "MapQuickView.h"
#include "FeatureLayer.h"
#include "Layer.h"
#include "ServiceFeatureTable.h"
#include "ServiceGeodatabase.h"
#include "ArcGISFeatureServiceInfo.h"
#include "ServiceVersionInfo.h"
#include "ServiceVersionParameters.h"
#include "Credential.h"
#include "AttributeListModel.h"
#include "FeatureTableEditResult.h"
#include "FeatureEditResult.h"

#include <QUuid>
#include <QRandomGenerator>

using namespace Esri::ArcGISRuntime;

EditWithBranchVersioning::EditWithBranchVersioning(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::streetsVector(this), this))
//  m_map(new Map(QUrl("https://rt-server1081.esri.com/portal/home/item.html?id=adb5c3090edf43f3853e57d8b0810f9b"), this))
//    m_map(new Map(QUrl("https://nice.esri.com/portal/home/item.html?id=4ce3f10b26394bb3b60c4b13ed0d9649"), this))
{
  m_cred2 = new Credential{"editor01", "editor01.password", this};
  m_cred = new Credential{"editor02", "editor02.password", this};
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

  // connect to the mouse clicked signal on the MapQuickView
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

  //  connectServiceGeodatabaseSignla

  connectSgdbSignals();


  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    // first clear the selection
    m_featureLayer->clearSelection();
    m_busy = true;
    emit busyChanged();

    emit hideWindow();

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
      delete m_selectedFeature;
      m_selectedFeature = nullptr;
      return;
    }
    if (!identifyResult->geoElements().empty())
    {
      // select the item in the result
      m_featureLayer->selectFeature(static_cast<Feature*>(identifyResult->geoElements().at(0)));
      //          emit  featureSelected();

      qDebug() << m_mapView->calloutData()->title();
      //          // Update the parent so the featureLayer is not deleted when the identifyResult is deleted.
      //          m_featureLayer->setParent(this);

      //          // obtain the selected feature with attributes
      QueryParameters queryParams;
      QString whereClause = "objectid=" + identifyResult->geoElements().at(0)->attributes()->attributeValue("objectid").toString();
      qDebug() << whereClause;
      queryParams.setWhereClause(whereClause);

      //          auto featTable = m_serviceGeodatabase->connectedTables()[0];
      //          featTable->queryFeatures(queryParams);
      //          m_featureTable = m_serviceGeodatabase->connectedTables()[0];
      m_featureTable->queryFeatures(queryParams, QueryFeatureFields::LoadAll);
    }
    m_busy = false;
    emit busyChanged();
  });
}

void EditWithBranchVersioning::connectSgdbSignals()
{
  //    m_serviceGeodatabase = new ServiceGeodatabase(QUrl("https://sampleserver7.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer"), m_cred, this);
  m_serviceGeodatabase = new ServiceGeodatabase(QUrl("https://sampleserver7.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer"), this);
  m_busy = true;
  emit busyChanged();

  //    connectServiceGeodatabaseSignals();

  connect(m_serviceGeodatabase, &ServiceGeodatabase::doneLoading, this, &EditWithBranchVersioning::onSgdbDoneLoadingCompleted);

  connect(m_serviceGeodatabase, &ServiceGeodatabase::fetchVersionsCompleted, this, &EditWithBranchVersioning::onFetchVersionsCompleted);

  connect(m_serviceGeodatabase, &ServiceGeodatabase::createVersionCompleted, this, &EditWithBranchVersioning::onCreateVersionCompleted);

  connect(m_serviceGeodatabase, &ServiceGeodatabase::errorOccurred, this, [this](Error error)
  {
    m_errorMessage = error.message() + " - " + error.additionalMessage();
    emit errorMessageChanged();

  });

  connect(m_serviceGeodatabase, &ServiceGeodatabase::switchVersionCompleted, this, [this] (QUuid)
  {
    m_sgdbCurrentVersion = m_serviceGeodatabase->versionName();
    emit sgdbCurrentVersionChanged();
  });

  m_serviceGeodatabase->load();
}

void EditWithBranchVersioning::onSgdbDoneLoadingCompleted(Error error)
{
  if (!error.isEmpty())
    return;

  m_featureTable = m_serviceGeodatabase->table(0);

  connect(m_featureTable, &ServiceFeatureTable::doneLoading, this, [this](Error error)
  {
    if (!error.isEmpty())
      return;

    m_mapView->setViewpoint(m_featureLayer->fullExtent());

    m_busy = false;
    emit busyChanged();
  });

  connect(m_featureTable, &ServiceFeatureTable::updateFeatureCompleted, this, [this] (QUuid, bool success)
  {
    m_serviceGeodatabase->applyEdits();

  });

  m_featureLayer = new FeatureLayer(m_featureTable, this);

  m_map->operationalLayers()->append(m_featureLayer);

  m_sgdbCurrentVersion = m_serviceGeodatabase->versionName();
  emit sgdbCurrentVersionChanged();

  connect(m_featureTable, &FeatureTable::queryFeaturesCompleted, this, [this](QUuid, FeatureQueryResult* featureQueryResult)
  {
    if (featureQueryResult && featureQueryResult->iterator().hasNext())
    {
      // first delete if not nullptr
      if (m_selectedFeature)
      {
        delete m_selectedFeature;
        m_selectedFeature = nullptr;
      }

      // set selected feature member
      m_selectedFeature = static_cast<ArcGISFeature*>(featureQueryResult->iterator().next(this));
      m_selectedFeature->setParent(this);
      m_featureType = m_selectedFeature->attributes()->attributeValue("PLACENAME").toString();
      m_mapView->calloutData()->setTitle(QString("<br><font size=\"+2\"><b>%1</b></font>").arg(m_featureType));
      m_mapView->calloutData()->setLocation(m_selectedFeature->geometry().extent().center());

      m_currentTypeDamage = m_selectedFeature->attributes()->attributeValue("TYPDAMAGE").toString();
      emit featureTypeChanged();
      emit currentTypeDamageChanged();
      emit featureSelected();
    }
  });
}

// might not be needed
void EditWithBranchVersioning::onFetchVersionsCompleted(QUuid, const QList<ServiceVersionInfo*> &serviceVersionInfos)
{
  qDebug() << serviceVersionInfos.count();
  for(auto serviceInfo : serviceVersionInfos)
  {
    auto owner = serviceInfo->isOwner() ? "true" : "false";
    auto access = serviceInfo->access() == VersionAccess::Private ? "Private" : "Other";
    qDebug() << serviceInfo->name() << "\t owner?: " << serviceInfo->isOwner() << "\t access?: " << access;
  }
}

void EditWithBranchVersioning::onCreateVersionCompleted(QUuid, Esri::ArcGISRuntime::ServiceVersionInfo* serviceVersionInfo)
{
  if (m_serviceGeodatabase->hasLocalEdits())
    return;

  if (!serviceVersionInfo)
    return;

  emit createVersionSuccess();

  m_createdVersion = serviceVersionInfo->name();
  qDebug() << m_createdVersion;
  qDebug() << "createVersionCompleted, ServiceGeodatabase::isOwner() - " << serviceVersionInfo->isOwner();
  m_serviceGeodatabase->switchVersion(serviceVersionInfo->name());
}

void EditWithBranchVersioning::switchVersion() const
{
  if (m_serviceGeodatabase->hasLocalEdits())
  {
    m_serviceGeodatabase->applyEdits();
  }

  if (m_sgdbCurrentVersion == m_createdVersion)
    m_serviceGeodatabase->switchVersion(m_serviceGeodatabase->defaultVersionName());
  else
    m_serviceGeodatabase->switchVersion(m_createdVersion);
}

QString EditWithBranchVersioning::featureType() const
{
  return m_featureType;
}

void EditWithBranchVersioning::updateAttribute(const QString& attributeValue)
{
  if (!m_selectedFeature)
    return;

  // update the two attirbutes with the inputed text.
  qDebug() << m_selectedFeature->attributes()->attributeValue("TYPDAMAGE");
  m_selectedFeature->attributes()->replaceAttribute("TYPDAMAGE", attributeValue);
  m_selectedFeature->featureTable()->updateFeature(m_selectedFeature);
}

void EditWithBranchVersioning::createVersion(const QString& versionName, const QString& versionAccess, const QString& description)
{
  // create parameters for branch

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
  const Polyline geom = m_selectedFeature->geometry();
  // if the selected feature is a point, change the geometry with the map point
  m_selectedFeature->setGeometry(mapPoint);

  // update the selected feature with the new geometry
  m_selectedFeature->featureTable()->updateFeature(m_selectedFeature);
  clearSelection();
  delete m_selectedFeature;
  m_selectedFeature = nullptr;
}

void EditWithBranchVersioning::clearSelection()
{
  if (m_featureLayer)
    m_featureLayer->clearSelection();
}

