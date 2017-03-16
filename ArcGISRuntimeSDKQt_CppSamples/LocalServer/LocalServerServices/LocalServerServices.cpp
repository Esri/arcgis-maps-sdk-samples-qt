// [WriteFile Name=ChangeBasemap, Category=Maps]
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

#include "LocalServerServices.h"

#include "Basemap.h"
#include "MapQuickView.h"
#include "Map.h"
#include "LocalServer.h"
#include "LocalMapService.h"
#include "LocalFeatureService.h"
#include "LocalGeoprocessingService.h"

#include <QDir>
#include <QDesktopServices>

using namespace Esri::ArcGISRuntime;

LocalServerServices::LocalServerServices(QQuickItem* parent) :
  QQuickItem(parent)
{
}

LocalServerServices::~LocalServerServices()
{
}

void LocalServerServices::componentComplete()
{
  QQuickItem::componentComplete();

  QString dataPath = QDir::homePath() + "/ArcGIS/Runtime/Data";
  m_localMapService = new LocalMapService(dataPath + "/mpk/PointsofInterest.mpk", this);
  m_localFeatureService = new LocalFeatureService(dataPath + "/mpk/PointsofInterest.mpk", this);
  m_localGPService = new LocalGeoprocessingService(dataPath + "/gpk/MessageInABottle.gpk", this);

  m_servicesList = new QStringListModel(this);

  if (LocalServer::instance()->isInstallValid())
  {
    connectSignals();
  }
}

void LocalServerServices::connectSignals()
{
  // local server status
  connect(LocalServer::instance(), &LocalServer::statusChanged, this, [this]()
  {
    if (LocalServer::status() == LocalServerStatus::Starting)
    {
      m_serverStatus.append("Server Status: STARTING\n");
    }
    else if(LocalServer::status() == LocalServerStatus::Started)
    {
      m_serverStatus.append("Server Status: STARTED\n");
      m_isServerRunning = true;
      emit isServerRunningChanged();
    }
    else if(LocalServer::status() == LocalServerStatus::Stopping)
    {
      m_serverStatus.append("Server Status: STOPPING\n");
    }
    else if(LocalServer::status() == LocalServerStatus::Stopped)
    {
      m_serverStatus.append("Server Status: STOPPED\n");
      m_isServerRunning = false;
      emit isServerRunningChanged();
    }
    else if(LocalServer::status() == LocalServerStatus::Failed)
    {
      m_serverStatus.append("Server Status: FAILED\n");
    }
    emit serverStatusChanged();
  });

  connect(m_localMapService, &LocalMapService::statusChanged, this, [this]()
  {
    if (m_localMapService->status() == LocalServerStatus::Starting)
    {
      m_serverStatus.append("Map Service Status: STARTING\n");
    }
    else if (m_localMapService->status() == LocalServerStatus::Started)
    {
      m_serverStatus.append("Map Service Status: STARTED\n");
      m_isServiceRunning = true;
      emit isServiceRunningChanged();

      m_services.append(m_localMapService->url().toString());
      m_servicesList->setStringList(m_services);
      emit servicesChanged();
    }
    else if (m_localMapService->status() == LocalServerStatus::Stopping)
    {
      m_serverStatus.append("Map Service Status: STOPPING\n");
    }
    else if (m_localMapService->status() == LocalServerStatus::Stopped)
    {
      m_serverStatus.append("Map Service Status: STOPPED\n");
      if (m_localFeatureService->status() == LocalServerStatus::Stopped && m_localGPService->status() == LocalServerStatus::Stopped)
      {
        m_isServiceRunning = false;
        emit isServiceRunningChanged();
      }

      m_services.removeOne(m_localMapService->url().toString());
      m_servicesList->setStringList(m_services);
      emit servicesChanged();
    }
    emit serverStatusChanged();
  });

  connect(m_localFeatureService, &LocalFeatureService::statusChanged, this, [this]()
  {
    if (m_localFeatureService->status() == LocalServerStatus::Starting)
    {
      m_serverStatus.append("Feature Service Status: STARTING\n");
    }
    else if (m_localFeatureService->status() == LocalServerStatus::Started)
    {
      m_serverStatus.append("Feature Service Status: STARTED\n");
      m_isServiceRunning = true;
      emit isServiceRunningChanged();

      m_services.append(m_localFeatureService->url().toString());
      m_servicesList->setStringList(m_services);
      emit servicesChanged();
    }
    else if (m_localFeatureService->status() == LocalServerStatus::Stopping)
    {
      m_serverStatus.append("Feature Service Status: STOPPING\n");
    }
    else if (m_localFeatureService->status() == LocalServerStatus::Stopped)
    {
      m_serverStatus.append("Feature Service Status: STOPPED\n");
      if (m_localMapService->status() == LocalServerStatus::Stopped && m_localGPService->status() == LocalServerStatus::Stopped)
      {
        m_isServiceRunning = false;
        emit isServiceRunningChanged();
      }

      m_services.removeOne(m_localFeatureService->url().toString());
      m_servicesList->setStringList(m_services);
      emit servicesChanged();
    }
    emit serverStatusChanged();
  });

  connect(m_localGPService, &LocalGeoprocessingService::statusChanged, this, [this]()
  {
    if (m_localGPService->status() == LocalServerStatus::Starting)
    {
      m_serverStatus.append("Geoprocessing Service Status: STARTING\n");
    }
    else if (m_localGPService->status() == LocalServerStatus::Started)
    {
      m_serverStatus.append("Geoprocessing Service Status: STARTED\n");
      m_isServiceRunning = true;
      emit isServiceRunningChanged();

      m_services.append(m_localGPService->url().toString());
      m_servicesList->setStringList(m_services);
      emit servicesChanged();
    }
    else if (m_localGPService->status() == LocalServerStatus::Stopping)
    {
      m_serverStatus.append("Geoprocessing Service Status: STOPPING\n");
    }
    else if (m_localGPService->status() == LocalServerStatus::Stopped)
    {
      m_serverStatus.append("Geoprocessing Service Status: STOPPED\n");
      if (m_localMapService->status() == LocalServerStatus::Stopped && m_localFeatureService->status() == LocalServerStatus::Stopped)
      {
        m_isServiceRunning = false;
        emit isServiceRunningChanged();
      }

      m_services.removeOne(m_localGPService->url().toString());
      m_servicesList->setStringList(m_services);
      emit servicesChanged();
    }
    emit serverStatusChanged();
  });
}

void LocalServerServices::startLocalServer()
{
  if (m_isServerRunning)
    return;

  // clear all the status messages
  m_serverStatus.clear();
  // start local server
  LocalServer::start();
}

void LocalServerServices::stopLocalServer()
{
  LocalServer::stop();
}

void LocalServerServices::startService(const QString& serviceName)
{
  if (serviceName == "Map Service")
  {
    if (m_localMapService->status() == LocalServerStatus::Started)
      return;
    m_localMapService->start();
  }
  else if (serviceName == "Feature Service")
  {
    if (m_localFeatureService->status() == LocalServerStatus::Started)
      return;
    m_localFeatureService->start();
  }
  else if (serviceName == "Geoprocessing Service")
  {
    if (m_localGPService->status() == LocalServerStatus::Started)
      return;
    m_localGPService->start();
  }
}

void LocalServerServices::stopService(const QString& serviceName)
{
  if (serviceName == "Map Service")
  {
    if (m_localMapService->status() == LocalServerStatus::Started)
      m_localMapService->stop();
  }
  else if (serviceName == "Feature Service")
  {
    if (m_localFeatureService->status() == LocalServerStatus::Started)
      m_localFeatureService->stop();
  }
  else if (serviceName == "Geoprocessing Service")
  {
    if (m_localGPService->status() == LocalServerStatus::Started)
      m_localGPService->stop();
  }
}

void LocalServerServices::openURL(const QString &serviceURL)
{
  QDesktopServices::openUrl(QUrl(serviceURL));
}
