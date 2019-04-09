// [WriteFile Name=LocalServerServices, Category=LocalServer]
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

#ifndef LOCAL_SERVER_SERVICES_H
#define LOCAL_SERVER_SERVICES_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class LocalServer;
    class LocalMapService;
    class LocalFeatureService;
    class LocalGeoprocessingService;
    class LocalService;
  }
}

#include <QQuickItem>

class LocalServerServices : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(QStringList servicesList READ servicesList NOTIFY servicesChanged)
  Q_PROPERTY(QString serverStatus READ serverStatus NOTIFY serverStatusChanged)
  Q_PROPERTY(bool isServerRunning READ isServerRunning NOTIFY isServerRunningChanged)
  Q_PROPERTY(bool isServiceRunning READ isServiceRunning NOTIFY isServiceRunningChanged)

public:
  explicit LocalServerServices(QQuickItem* parent = nullptr);
  ~LocalServerServices() override;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void startLocalServer();
  Q_INVOKABLE void stopLocalServer();
  Q_INVOKABLE void startService(const QString& serviceName, const QUrl& filePath = QUrl(""));
  Q_INVOKABLE void stopService(const QUrl& serviceUrl);
  Q_INVOKABLE void openURL(const QString& serviceURL);

signals:
  void servicesChanged();
  void serverStatusChanged();
  void isServerRunningChanged();
  void isServiceRunningChanged();

private:
  void connectSignals();
  QStringList servicesList() const { return m_services; }
  QString serverStatus() const { return m_serverStatus; }
  bool isServerRunning() const { return m_isServerRunning; }
  bool isServiceRunning() const { return m_isServiceRunning; }
  bool isAnyServiceRunning();
  void updateStatus(Esri::ArcGISRuntime::LocalService* service, const QString& serviceName);
  void getCurrentServices();

private:
  Esri::ArcGISRuntime::LocalMapService* m_localMapService = nullptr;
  Esri::ArcGISRuntime::LocalFeatureService* m_localFeatureService = nullptr;
  Esri::ArcGISRuntime::LocalGeoprocessingService* m_localGPService = nullptr;
  QStringList m_services;
  QString m_serverStatus;
  bool m_isServerRunning = false;
  bool m_isServiceRunning = false;
  QHash<QUrl, Esri::ArcGISRuntime::LocalService*> m_servicesHash;
};

#endif // LOCAL_SERVER_SERVICES_H

