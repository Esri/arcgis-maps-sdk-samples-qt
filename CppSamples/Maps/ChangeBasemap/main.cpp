// Copyright 2021 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ChangeBasemap.h"

#include "ArcGISRuntimeEnvironment.h"
#include "Esri/ArcGISRuntime/Toolkit/register.h"

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
  app.setApplicationName(QString("ChangeBasemap - C++"));

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
  ChangeBasemap::init();

  // Initialize application view
  QQmlApplicationEngine engine;
  // Add the import Path
  engine.addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));

  // Register the application view with the toolkit
  Esri::ArcGISRuntime::Toolkit::registerComponents(engine);

#ifdef ARCGIS_RUNTIME_IMPORT_PATH_2
  engine.addImportPath(ARCGIS_RUNTIME_IMPORT_PATH_2);
#endif

  // Set the source
  engine.load(QUrl("qrc:/Samples/Maps/ChangeBasemap/main.qml"));

  return app.exec();
}
