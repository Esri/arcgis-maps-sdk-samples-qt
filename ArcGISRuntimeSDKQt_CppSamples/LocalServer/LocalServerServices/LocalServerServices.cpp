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

// constructor
LocalServerServices::LocalServerServices(QQuickItem* parent) :
  QQuickItem(parent)
{
}

// destructor
LocalServerServices::~LocalServerServices()
{
}

void LocalServerServices::componentComplete()
{
  QQuickItem::componentComplete();

  QString dataPath = QDir::homePath() + "/ArcGIS/Runtime/Data";
  // create a map service
  m_localMapService = new LocalMapService(dataPath + "/mpk/PointsofInterest.mpk", this);
  // create a feature service
  m_localFeatureService = new LocalFeatureService(dataPath + "/mpk/PointsofInterest.mpk", this);
  // create a gp service
  m_localGPService = new LocalGeoprocessingService(dataPath + "/gpk/MessageInABottle.gpk", this);

  // create a list model
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
    // append to the status string
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
    updateStatus(m_localMapService, "Map");
  });

  connect(m_localFeatureService, &LocalFeatureService::statusChanged, this, [this]()
  {
    updateStatus(m_localFeatureService, "Feature");
  });

  connect(m_localGPService, &LocalGeoprocessingService::statusChanged, this, [this]()
  {
    updateStatus(m_localGPService, "Geoprocessing");
  });
}

// start the server
void LocalServerServices::startLocalServer()
{
  if (m_isServerRunning)
    return;

  // clear all the status messages
  m_serverStatus.clear();
  // start local server
  LocalServer::start();
}

// stop the server
void LocalServerServices::stopLocalServer()
{
  LocalServer::stop();
}

// start a service
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

// stop any service
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

// open the link in a browser
void LocalServerServices::openURL(const QString& serviceURL)
{
  QDesktopServices::openUrl(QUrl(serviceURL));
}

// check if any one of the services is running
bool LocalServerServices::isAnyServiceRunning()
{
  return !(m_localMapService->status() == LocalServerStatus::Stopped
      && m_localFeatureService->status() == LocalServerStatus::Stopped
      && m_localGPService->status() == LocalServerStatus::Stopped);
}

// get the current status of any service
void LocalServerServices::updateStatus(LocalService *service, const QString& serviceName)
{
  if (service->status() == LocalServerStatus::Starting)
  {
    // append the status string
    m_serverStatus.append(serviceName + " Service Status: STARTING\n");
  }
  else if (service->status() == LocalServerStatus::Started)
  {
    m_serverStatus.append(serviceName + " Service Status: STARTED\n");
    m_isServiceRunning = true;
    emit isServiceRunningChanged();

    // append to the string list
    m_services.append(service->url().toString());
    // set the string list to the model and emit
    m_servicesList->setStringList(m_services);
    emit servicesChanged();
  }
  else if (service->status() == LocalServerStatus::Stopping)
  {
    m_serverStatus.append(serviceName + " Service Status: STOPPING\n");
  }
  else if (service->status() == LocalServerStatus::Stopped)
  {
    m_serverStatus.append(serviceName + " Service Status: STOPPED\n");
    if (!isAnyServiceRunning())
    {
      m_isServiceRunning = false;
      emit isServiceRunningChanged();
    }

    // remove the url from the string list
    m_services.removeOne(service->url().toString());
    // set the stringlist to the model and emit
    m_servicesList->setStringList(m_services);
    emit servicesChanged();
  }
  emit serverStatusChanged();
}
