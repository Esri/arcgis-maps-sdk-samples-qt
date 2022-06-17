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

#include "GAnalytics.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QOperatingSystemVersion>
#include <QQmlEngine>
#include <QSettings>
#include <QUuid>

#if defined (GANALYTICS_API_KEY) && defined (GANALYTICS_STREAM_ID)
GAnalytics::GAnalytics(QObject* parent /* = nullptr */):
  QObject(parent),
  m_apiSecret(QUOTE(GANALYTICS_API_KEY)),
  m_measurementId(QUOTE(GANALYTICS_STREAM_ID))
{
}
#endif // GANALYTICS_API_KEY && GANALYTICS_STREAM_ID

GAnalytics::GAnalytics(QObject* parent /* = nullptr */):
  QObject(parent),
  m_apiSecret(""),
  m_measurementId("")
{
}

GAnalytics::~GAnalytics()
{
  endSession();
}

void GAnalytics::init()
{
  if (m_apiSecret == "" || m_measurementId == "")
  {
    m_telemetryEnabled = false;
    setIsVisible(false);
    return;
  }

  generateClientId();

  if (!m_settings.contains("GAnalytics-telemetry-enabled"))
  {
    m_telemetryEnabled = true;
    m_settings.setValue("GAnalytics-telemetry-enabled", m_telemetryEnabled);
    setIsVisible(true);
  }
  else
  {
    m_telemetryEnabled = m_settings.value("GAnalytics-telemetry-enabled").toBool();
  }

  m_startTime = QDateTime::currentSecsSinceEpoch();

  m_networkAccessManager = new QNetworkAccessManager();

#ifdef CPP_VIEWER
  m_defaultParameters.insert("sample_viewer_type", "Cpp");
#else // QML_VIEWER
  m_defaultParameters.insert("sample_viewer_type", "QML");
#endif // CPP_VIEWER / QML_VIEWER

  auto os = QOperatingSystemVersion::current();
  m_defaultParameters.insert("operating_system", QString{"%1 %2.%3"}.arg(os.name(), os.majorVersion(), os.minorVersion()));
}

void GAnalytics::postEvent(const QString& eventName, QVariantMap parameters)
{
  if (!m_telemetryEnabled || m_apiSecret == "" || m_measurementId == "")
    return;

  int sessionTime = (QDateTime::currentSecsSinceEpoch() - m_startTime) * 1000;

  // Google will not accept the POST request if the engagement time is 0 or undefined
  if (sessionTime <= 0)
    sessionTime = 1;

  parameters.insert("engagement_time_msec", sessionTime);

  QJsonObject event;
  event.insert("name", eventName);
  parameters.unite(m_defaultParameters);
  event.insert("params", QJsonObject::fromVariantMap(parameters));

  QJsonObject body;
  body.insert("client_id", m_clientId);
  body.insert("events", event);
  const QUrl url(QString{"https://google-analytics.com/mp/collect?api_secret=%1&measurement_id=%2"}.arg(m_apiSecret, m_measurementId));

  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application, json");
  QJsonDocument data(body);

  m_networkAccessManager->post(request, data.toJson());
}

void GAnalytics::startSession()
{
  postEvent("open_sample_viewer", QVariantMap());
  // Prevent additional start session posts from being made
  m_appSessionStarted = true;
}

void GAnalytics::endSession()
{
  m_defaultParameters.insert("session_duration_sec", QDateTime::currentSecsSinceEpoch() - m_startTime);
  postEvent("close_sample_viewer", m_defaultParameters);
}

void GAnalytics::generateClientId()
{
  if (!m_settings.contains("GAnalytics-clientId"))
  {
    m_clientId= QUuid::createUuid().toString();
    m_settings.setValue("GAnalytics-clientId", m_clientId);
  }
  else
  {
    m_clientId = m_settings.value("GAnalytics-clientId").toString();
  }
}

bool GAnalytics::telemetryEnabled() const
{
  return m_telemetryEnabled;
}

void GAnalytics::setTelemetryEnabled(const bool telemetryEnabled)
{
  m_telemetryEnabled = telemetryEnabled;

  m_settings.setValue("GAnalytics-telemetry-enabled", m_telemetryEnabled);

  if (telemetryEnabled && !m_appSessionStarted)
    startSession();
}

bool GAnalytics::isVisible() const
{
  return m_isVisible;
}

void GAnalytics::setIsVisible(bool isVisible)
{
  m_isVisible = isVisible;
}
