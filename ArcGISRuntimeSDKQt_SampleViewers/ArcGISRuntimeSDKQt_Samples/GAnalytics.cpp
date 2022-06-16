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

#ifdef GANALYTICS_API_KEY
#ifdef GANALYTICS_STREAM_ID
GAnalytics::GAnalytics(QObject* parent /* = nullptr */):
  QObject(parent),
  m_apiSecret(QUOTE(GANALYTICS_API_KEY)),
  m_measurementId(QUOTE(GANALYTICS_STREAM_ID))
{
}
#endif // GANALYTICS_STREAM_ID
#endif // GANALYTICS_API_KEY

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

  QSettings settings;
  generateClientId();
  if (!settings.contains("GAnalytics-telemetry-enabled"))
  {
    m_telemetryEnabled = true;
    settings.setValue("GAnalytics-telemetry-enabled", m_telemetryEnabled);
    setIsVisible(true);
  }
  else
  {
    m_telemetryEnabled = settings.value("GAnalytics-telemetry-enabled").toBool();
  }
}

void GAnalytics::postEvent(const QString eventName, QVariantMap parameters)
{
  if (!m_telemetryEnabled || m_apiSecret == "" || m_measurementId == "")
    return;

#ifdef CPP_VIEWER
  parameters.insert("sample_viewer_type", "Cpp");
#else // QML_VIEWER
  parameters.insert("sample_viewer_type", "QML");
#endif // CPP_VIEWER / QML_VIEWER

  auto os = QOperatingSystemVersion::current();

  parameters.insert("operating_system", os.name() + " " + QString::number(os.majorVersion()) + "." + QString::number(os.minorVersion()));

  int sessionTime = (QDateTime::currentSecsSinceEpoch()-m_startTime)*1000;

  // Google will not accept the POST request if the engagement time is 0 or undefined
  if (sessionTime <= 0)
    sessionTime = 1;

  parameters.insert("engagement_time_msec", sessionTime);

  QJsonObject event;
  event.insert("name", eventName);
  event.insert("params", QJsonObject::fromVariantMap(parameters));

  QJsonObject body;
  body.insert("client_id", m_clientId);
  body.insert("events", event);
  QNetworkAccessManager* mgr = new QNetworkAccessManager();
  const QUrl url(QString("https://google-analytics.com/mp/collect?"
                         "api_secret=" + m_apiSecret +
                         "&measurement_id=" + m_measurementId));

  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application, json");
  QJsonDocument data(body);

  mgr->post(request, data.toJson());
}

void GAnalytics::startSession()
{
  m_startTime = QDateTime::currentSecsSinceEpoch();
  postEvent("open_sample_viewer", QVariantMap());
}

void GAnalytics::endSession()
{
  QVariantMap params;
  params.insert("session_duration_sec", QDateTime::currentSecsSinceEpoch() - m_startTime);
  postEvent("close_sample_viewer", params);
}

void GAnalytics::generateClientId()
{
  QSettings settings;
  if (!settings.contains("GAnalytics-clientId"))
  {
    m_clientId= QUuid::createUuid().toString();
    settings.setValue("GAnalytics-clientId", m_clientId);
  }
  else
  {
    m_clientId = settings.value("GAnalytics-clientId").toString();
  }
}

QString GAnalytics::apiSecret() const
{
  return m_apiSecret;
}

void GAnalytics::setApiSecret(const QString apiSecret)
{
  m_apiSecret = apiSecret;
}

QString GAnalytics::measurementId() const
{
  return m_measurementId;
}

void GAnalytics::setMeasurementId(const QString measurementId)
{
  m_measurementId = measurementId;
}

bool GAnalytics::telemetryEnabled() const
{
  return m_telemetryEnabled;
}

void GAnalytics::setTelemetryEnabled(const bool telemetryEnabled)
{
  m_telemetryEnabled = telemetryEnabled;

  QSettings settings;
  settings.setValue("GAnalytics-telemetry-enabled", m_telemetryEnabled);

  if (telemetryEnabled)
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

QString GAnalytics::clientId() const
{
  return m_clientId;
}
