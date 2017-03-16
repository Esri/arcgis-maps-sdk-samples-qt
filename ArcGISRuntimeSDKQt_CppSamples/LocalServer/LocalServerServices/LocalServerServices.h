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

#ifndef LOCAL_SERVER_SERVICES_H
#define LOCAL_SERVER_SERVICES_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class LocalServer;
class LocalMapService;
class LocalFeatureService;
class LocalGeoprocessingService;
}
}

#include <QQuickItem>
#include <QStringListModel>

class LocalServerServices : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(QStringListModel* services READ services NOTIFY servicesChanged)
  Q_PROPERTY(QString serverStatus READ serverStatus NOTIFY serverStatusChanged)
  Q_PROPERTY(bool isServerRunning READ isServerRunning NOTIFY isServerRunningChanged)
  Q_PROPERTY(bool isServiceRunning READ isServiceRunning NOTIFY isServiceRunningChanged)

public:
  explicit LocalServerServices(QQuickItem* parent = nullptr);
  ~LocalServerServices();

  void componentComplete() Q_DECL_OVERRIDE;
  Q_INVOKABLE void startLocalServer();
  Q_INVOKABLE void stopLocalServer();
  Q_INVOKABLE void startService(const QString& serviceName);
  Q_INVOKABLE void stopService(const QString& serviceName);
  Q_INVOKABLE void openURL(const QString& serviceURL);

signals:
  void servicesChanged();
  void serverStatusChanged();
  void isServerRunningChanged();
  void isServiceRunningChanged();

private:
  void connectSignals();
  QStringListModel* services() const { return m_servicesList; }
  QString serverStatus() const { return m_serverStatus; }
  bool isServerRunning() const { return m_isServerRunning; }
  bool isServiceRunning() const { return m_isServiceRunning; }

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::LocalMapService* m_localMapService = nullptr;
  Esri::ArcGISRuntime::LocalFeatureService* m_localFeatureService = nullptr;
  Esri::ArcGISRuntime::LocalGeoprocessingService* m_localGPService = nullptr;
  QStringListModel* m_servicesList = nullptr;
  QStringList m_services;
  QString m_serverStatus;
  bool m_isServerRunning = false;
  bool m_isServiceRunning = false;
};

#endif // LOCAL_SERVER_SERVICES_H

