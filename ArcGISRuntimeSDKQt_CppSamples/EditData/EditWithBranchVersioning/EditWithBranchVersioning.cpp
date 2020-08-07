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

#include <QUuid>
#include <QRandomGenerator>

using namespace Esri::ArcGISRuntime;

EditWithBranchVersioning::EditWithBranchVersioning(QObject* parent /* = nullptr */):
  QObject(parent),
//  m_map(new Map(m_serviceUrl, this))
  m_map(new Map(QUrl("https://rt-server1081.esri.com/portal/home/item.html?id=adb5c3090edf43f3853e57d8b0810f9b"), this))
{
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


  connect(m_map, &Map::doneLoading, this, [this](Error e)
  {
    if (!e.isEmpty())
      return;

    for( Layer* layer : *m_map->operationalLayers()) {
      qDebug() << layer->name();

      m_featureLayer = static_cast<FeatureLayer*>(layer);
//      m_featureTable = m_featureLayer->featureTable();
//      auto sft = static_cast<ServiceFeatureTable*>(m_featureLayer->featureTable());
//      m_serviceGeodatabase = sft->serviceGeodatabase();
      m_featureTable = static_cast<ServiceFeatureTable*>(m_featureLayer->featureTable());
      m_serviceGeodatabase = m_featureTable->serviceGeodatabase();



      connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
      {
        // first clear the selection
        m_featureLayer->clearSelection();

//        emit hideWindow();

        // call identify on the map view
        m_mapView->identifyLayer(m_featureLayer, mouseEvent.x(), mouseEvent.y(), 5, false, 1);
      });


      connect(m_mapView, &MapQuickView::viewpointChanged, this, [this]()
      {
        m_featureLayer->clearSelection();

//        emit hideWindow();
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
//          emit  featureSelected();

          qDebug() << m_mapView->calloutData()->title();
//          // Update the parent so the featureLayer is not deleted when the identifyResult is deleted.
//          m_featureLayer->setParent(this);

//          // obtain the selected feature with attributes
          QueryParameters queryParams;
          QString whereClause = "objectid=" + identifyResult->geoElements().at(0)->attributes()->attributeValue("objectid").toString();
          queryParams.setWhereClause(whereClause);

//          auto featTable = m_serviceGeodatabase->connectedTables()[0];
//          featTable->queryFeatures(queryParams);
//          m_featureTable = m_serviceGeodatabase->connectedTables()[0];
          m_featureTable->queryFeatures(queryParams);
        }
      });

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
          m_featureType = m_selectedFeature->attributes()->attributeValue("NAME").toString();
          m_mapView->calloutData()->setTitle(QString("<br><font size=\"+2\"><b>%1</b></font>").arg(m_featureType));
          m_mapView->calloutData()->setLocation(m_selectedFeature->geometry().extent().center());
          qDebug() << m_mapView->calloutData()->title();
          emit featureTypeChanged();
          emit featureSelected();
        }
      });


      connect(m_serviceGeodatabase, &ServiceGeodatabase::doneLoading, this, [this] (Error e)
      {
        if (!e.isEmpty())
          return;

        qDebug() << "Supports branch versioning: " << m_serviceGeodatabase->isSupportsBranchVersioning();

        // create parameters for branch
//        auto params = createParams();
        m_sgdbCurrentVersion = m_serviceGeodatabase->versionName();
        emit sgdbCurrentVersionChanged();
      });

      connect(m_serviceGeodatabase, &ServiceGeodatabase::fetchVersionsCompleted, this, [this] (QUuid, const QList<ServiceVersionInfo*> &serviceVersionInfos)
      {
        qDebug() << serviceVersionInfos.count();
        for(auto serviceInfo : serviceVersionInfos)
        {
          auto owner = serviceInfo->isOwner() ? "true" : "false";
          auto access = serviceInfo->access() == VersionAccess::Private ? "Private" : "Other";
          qDebug() << serviceInfo->name() << "\t owner?(api broken): " << owner << "\t access?: " << access;
        }
      });

      connect(m_serviceGeodatabase, &ServiceGeodatabase::createVersionCompleted, this, [this](QUuid, Esri::ArcGISRuntime::ServiceVersionInfo* serviceVersionInfo)
      {
        if (m_serviceGeodatabase->hasLocalEdits())
          return;

        m_createdVersion = serviceVersionInfo->name();
        qDebug() << serviceVersionInfo->isOwner() << " - Not working API level";
        m_serviceGeodatabase->switchVersion(serviceVersionInfo->name());
      });

      connect(m_serviceGeodatabase, &ServiceGeodatabase::errorOccurred, this, [this](Error e)
      {
        qDebug() << e.message();

      });

      connect(m_serviceGeodatabase, &ServiceGeodatabase::switchVersionCompleted, this, [this] (QUuid)
      {
        m_sgdbCurrentVersion = m_serviceGeodatabase->versionName();
        emit sgdbCurrentVersionChanged();
      });

    }

  });

  emit mapViewChanged();
}

AuthenticationManager *EditWithBranchVersioning::authManager() const
{
  return AuthenticationManager::instance();
}


ServiceVersionParameters* EditWithBranchVersioning::createParams()
{
  ServiceVersionParameters* params = new ServiceVersionParameters(this);
  const quint32 value = QRandomGenerator::global()->generate();
  const auto s_value = QString::number(value);
  const QString name{"SAMPLE_DESIGN_" + s_value};
  params->setName(name);
  params->setAccess(VersionAccess::Private);
  params->setDescription("description + " + name);

  auto access_s = params->access() == VersionAccess::Private ? "Private" : "Other";
  qDebug() << params->name();
  qDebug() << access_s;
  qDebug() << params->description();

  return std::move(params);
}

void EditWithBranchVersioning::createVersion()
{
  // create parameters for branch
        auto params = createParams();

        m_serviceGeodatabase->createVersion(params);
}

void EditWithBranchVersioning::switchVersion() const
{
  if (m_sgdbCurrentVersion == m_createdVersion)
    m_serviceGeodatabase->switchVersion(m_serviceGeodatabase->defaultVersionName());
  else
    m_serviceGeodatabase->switchVersion(m_createdVersion);
}

void EditWithBranchVersioning::fetchVersions() const
{
  if (!m_serviceGeodatabase)
    return;

  m_serviceGeodatabase->fetchVersions();
}

QString EditWithBranchVersioning::featureType() const
{
  return m_featureType;
}
