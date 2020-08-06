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

  connect(m_map, &Map::doneLoading, this, [this](Error e)
  {
    if (!e.isEmpty())
      return;

    for( Layer* layer : *m_map->operationalLayers()) {
      qDebug() << layer->name();

      auto fl = static_cast<FeatureLayer*>(layer);
      auto sft = static_cast<ServiceFeatureTable*>(fl->featureTable());
      m_serviceGeodatabase = sft->serviceGeodatabase();

      connect(m_serviceGeodatabase, &ServiceGeodatabase::doneLoading, this, [this] (Error e)
      {
        if (!e.isEmpty())
          return;

        qDebug() << "Supports branch versioning: " << m_serviceGeodatabase->isSupportsBranchVersioning();

        // create parameters for branch
//        auto params = createParams();


//        qDebug() << m_serviceGeodatabase->credential()->username();
        m_sgdbCurrentVersion = m_serviceGeodatabase->versionName();
        emit sgdbCurrentVersionChanged();
//        m_serviceGeodatabase->fetchVersions();
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

        // create parameters for branch
//        auto params = createParams();

//        m_serviceGeodatabase->createVersion(params);

      });

      connect(m_serviceGeodatabase, &ServiceGeodatabase::createVersionCompleted, this, [this](QUuid, Esri::ArcGISRuntime::ServiceVersionInfo* serviceVersionInfo)
      {
        if (m_serviceGeodatabase->hasLocalEdits())
          return;

        m_createdVersion = serviceVersionInfo->name();
        qDebug() << serviceVersionInfo->isOwner() << " - Not working API level";
        m_serviceGeodatabase->switchVersion(serviceVersionInfo->name());
//        m_serviceGeodatabase->fetchVersions();


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
