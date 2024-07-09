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

#include "ExportVectorTiles.h"

#include "ArcGISRuntimeEnvironment.h"

#include <QCommandLineParser>
#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);
  app.setApplicationName(QString("ExportVectorTiles - C++"));

  // Use of ArcGIS location services, such as basemap styles, geocoding, and routing services,
  // requires an access token. For more information see
  // https://links.esri.com/arcgis-runtime-security-auth.

  // The following methods grant an access token:

  // 1. User authentication: Grants a temporary access token associated with a user's ArcGIS account.
  // To generate a token, a user logs in to the app with an ArcGIS account that is part of an
  // organization in ArcGIS Online or ArcGIS Enterprise.

  // 2. API key authentication: Get a long-lived access token that gives your application access to
  // ArcGIS location services. Go to the tutorial at https://links.esri.com/create-an-api-key.
  // Copy the API Key access token.

  const QString accessToken = QString("");

  if (accessToken.isEmpty())
  {
      qWarning() << "Use of ArcGIS location services, such as the basemap styles service, requires" <<
                    "you to authenticate with an ArcGIS account or set the API Key property.";
  }
  else
  {
      Esri::ArcGISRuntime::ArcGISRuntimeEnvironment::setApiKey(accessToken);
  }

  // Initialize the sample
  ExportVectorTiles::init();

  // Initialize application view
  QQmlApplicationEngine engine;
  // Add the import Path
  engine.addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));

#ifdef ARCGIS_RUNTIME_IMPORT_PATH_2
  engine.addImportPath(ARCGIS_RUNTIME_IMPORT_PATH_2);
#endif

  // Set the source
  engine.load(QUrl("qrc:/Samples/Layers/ExportVectorTiles/main.qml"));

  return app.exec();
}

// Use of Esri location services, including basemaps and geocoding,
// requires authentication using either an ArcGIS identity or an API Key.
// 1. ArcGIS identity: An ArcGIS named user account that is a member of an
//    organization in ArcGIS Online or ArcGIS Enterprise.
// 2. API key: API key: a permanent key that grants access to
//    location services and premium content in your applications.
//    Visit your ArcGIS Developers Dashboard to create a new
//    API key or access an existing API key.

void setAPIKey(const QGuiApplication& app, QString apiKey)
{
  if (apiKey.isEmpty())
  {
    // Try parsing API key from command line argument, which uses the following syntax "-k <apiKey>".
    QCommandLineParser cmdParser;
    QCommandLineOption apiKeyArgument(QStringList{"k", "api"}, "The API Key property used to access Esri location services", "apiKeyInput");
    cmdParser.addOption(apiKeyArgument);
    cmdParser.process(app);

    apiKey = cmdParser.value(apiKeyArgument);

    if (apiKey.isEmpty())
    {
      qWarning() << "Use of Esri location services, including basemaps, requires" <<
                    "you to authenticate with an ArcGIS identity or set the API Key property.";
      return;
    }
  }

  Esri::ArcGISRuntime::ArcGISRuntimeEnvironment::setApiKey(apiKey);
}

