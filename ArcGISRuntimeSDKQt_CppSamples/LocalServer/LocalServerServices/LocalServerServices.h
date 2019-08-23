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

class QTemporaryDir;

#include <memory>
#include <QQuickItem>

class LocalServerServices : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(QStringList servicesList MEMBER m_services NOTIFY servicesChanged)
  Q_PROPERTY(QString serverStatus MEMBER m_serverStatus NOTIFY serverStatusChanged)
  Q_PROPERTY(bool isServerRunning MEMBER m_isServerRunning NOTIFY isServerRunningChanged)
  Q_PROPERTY(bool isServiceRunning MEMBER m_isServiceRunning NOTIFY isServiceRunningChanged)
  Q_PROPERTY(QString dataPath MEMBER m_dataPath NOTIFY dataPathChanged)

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
  void dataPathChanged();

private:
  void connectSignals();
  bool isAnyServiceRunning();
  void updateStatus(Esri::ArcGISRuntime::LocalService* service, const QString& serviceName);
  void getCurrentServices();
  static QString shortestTempPath();

private:
  Esri::ArcGISRuntime::LocalMapService* m_localMapService = nullptr;
  Esri::ArcGISRuntime::LocalFeatureService* m_localFeatureService = nullptr;
  Esri::ArcGISRuntime::LocalGeoprocessingService* m_localGPService = nullptr;
  QStringList m_services;
  QString m_serverStatus;
  QString m_dataPath;
  bool m_isServerRunning = false;
  bool m_isServiceRunning = false;
  QHash<QUrl, Esri::ArcGISRuntime::LocalService*> m_servicesHash;
  std::unique_ptr<QTemporaryDir> m_tempDir;
};

#endif // LOCAL_SERVER_SERVICES_H

