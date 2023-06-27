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

#include "Telemetry.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QOperatingSystemVersion>
#include <QQmlEngine>
#include <QSettings>
#include <QUuid>

// Allow retrieval of analytics keys
#include <cstdlib>
#define STRINGIZE(x) #x
#define QUOTE(x) STRINGIZE(x)

Telemetry::Telemetry(QObject* parent /* = nullptr */):
  QObject(parent)
{
}

Telemetry::~Telemetry()
{
  endSession();
}

void Telemetry::init()
{
  generateClientId();

  // Disable all telemetry
  m_telemetryEnabled = false;
  return;

  // If this is an Esri daily build or a local user build, disable analytics
  if (m_apiSecret.isEmpty() || m_measurementId.isEmpty())
  {
    qDebug() << "No analytics API key or measurement ID was provided, disabling telemetry";
    m_telemetryEnabled = false;
    return;
  }

  if (!m_settings.contains("Telemetry-enabled"))
  {
    m_telemetryEnabled = true;
    m_settings.setValue("Telemetry-enabled", m_telemetryEnabled);
    setIsVisible(true);
  }
  else
  {
    m_telemetryEnabled = m_settings.value("Telemetry-enabled").toBool();
  }

  m_startTime = QDateTime::currentSecsSinceEpoch();
  m_networkAccessManager = new QNetworkAccessManager(this);

#ifdef CPP_VIEWER
  m_defaultParameters.insert("sample_viewer_type", "Cpp");
#else // QML_VIEWER
  m_defaultParameters.insert("sample_viewer_type", "QML");
#endif // CPP_VIEWER / QML_VIEWER

  const auto os = QOperatingSystemVersion::current();
  m_defaultParameters.insert("operating_system", QString{"%1 %2.%3"}.arg(os.name(), QString::number(os.majorVersion()), QString::number(os.minorVersion())));
  m_defaultParameters.insert("language", QLocale::system().nativeLanguageName());

  m_AnalyticsUrl = QString{"www.example.com"}.arg(m_apiSecret, m_measurementId);
  m_networkRequest = QNetworkRequest(m_AnalyticsUrl);
  m_networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application, json");
}

void Telemetry::postEvent(const QString& eventName, QVariantMap parameters)
{
  if (!m_telemetryEnabled || m_apiSecret.isEmpty() || m_measurementId.isEmpty())
    return;

  int sessionTimeInSeconds = (QDateTime::currentSecsSinceEpoch() - m_startTime) * 1000;

  if (sessionTimeInSeconds <= 0)
    sessionTimeInSeconds = 1;

  parameters.insert("engagement_time_msec", sessionTimeInSeconds);

  QJsonObject event;
  event.insert("name", eventName);
  parameters.insert(m_defaultParameters);
  event.insert("params", QJsonObject::fromVariantMap(parameters));

  QJsonObject body;
  body.insert("client_id", m_clientId);
  body.insert("events", event);

  const QJsonDocument data(body);

  m_networkAccessManager->post(m_networkRequest, data.toJson());
}

void Telemetry::startSession()
{
  postEvent("open_sample_viewer", QVariantMap());
  // Prevent additional start session posts from being made
  m_appSessionStarted = true;
}

void Telemetry::endSession()
{
  m_defaultParameters.insert("session_duration_sec", QDateTime::currentSecsSinceEpoch() - m_startTime);
  postEvent("close_sample_viewer", m_defaultParameters);
}

void Telemetry::generateClientId()
{
  if (!m_settings.contains("Telemetry-clientId"))
  {
    m_clientId = QUuid::createUuid().toString();
    m_settings.setValue("Telemetry-clientId", m_clientId);
  }
  else
  {
    m_clientId = m_settings.value("Telemetry-clientId").toString();
  }
}

bool Telemetry::telemetryEnabled() const
{
  return m_telemetryEnabled;
}

void Telemetry::setTelemetryEnabled(const bool telemetryEnabled)
{
  m_telemetryEnabled = telemetryEnabled;

  m_settings.setValue("Telemetry-enabled", m_telemetryEnabled);

  if (telemetryEnabled && !m_appSessionStarted)
    startSession();
}

bool Telemetry::isVisible() const
{
  return m_isVisible;
}

void Telemetry::setIsVisible(bool isVisible)
{
  m_isVisible = isVisible;
}
