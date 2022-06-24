// [Legal]
// Copyright 2022 Esri.

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

#ifndef GANALYTICS_H
#define GANALYTICS_H

#include <QDateTime>
#include <QNetworkAccessManager>
#include <QObject>
#include <QSettings>
#include <QVariantMap>
#include <QUrl>
#include <QNetworkRequest>

class GAnalytics : public QObject
{
  Q_OBJECT

  Q_PROPERTY(bool telemetryEnabled READ telemetryEnabled WRITE setTelemetryEnabled NOTIFY telemetryEnabledChanged)
  Q_PROPERTY(bool isVisible READ isVisible WRITE setIsVisible NOTIFY isVisibleChanged)

public:
  explicit GAnalytics(QObject* parent = nullptr);

  ~GAnalytics() override;

  Q_INVOKABLE virtual void init();
  Q_INVOKABLE void postEvent(const QString& eventName, QVariantMap parameters);

signals:
  void telemetryEnabledChanged();
  void isVisibleChanged();

private:
  void setTelemetryEnabled(const bool telemetryEnabled);
  void setIsVisible(const bool isVisible);
  void startSession();
  void endSession();
  void generateClientId();
  bool telemetryEnabled() const;
  bool isVisible() const;

  const QString m_apiSecret;
  const QString m_measurementId;
  QString m_clientId;
  qint64 m_startTime;
  bool m_telemetryEnabled;
  bool m_isVisible = false;
  bool m_appSessionStarted = false;
  QNetworkAccessManager* m_networkAccessManager = nullptr;
  QSettings m_settings;
  QVariantMap m_defaultParameters;
  QUrl m_googleAnalyticsUrl;
  QNetworkRequest m_networkRequest;
};

#endif // GANALYTICS_H
