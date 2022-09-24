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

#include <QSettings>
#include <QGuiApplication>
#include <QQuickView>
#include <QCommandLineParser>
#include <QDir>
#include <QQmlEngine>

void setAPIKey(const QGuiApplication& app, QString apiKey);

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);
  app.setApplicationName(QString("BrowseBuildingFloors - QML"));

  // Access to Esri location services requires an API key. This can be copied below or used as a command line argument.
  const QString apiKey = QString("");
  setAPIKey(app, apiKey);

  // Enable OpenGL
  qputenv("QSG_RHI_BACKEND", "opengl");

  // Initialize application view
  QQuickView view;
  view.setResizeMode(QQuickView::SizeRootObjectToView);

  // Add the import Path
  view.engine()->addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));

#ifdef ARCGIS_RUNTIME_IMPORT_PATH_2
  view.engine()->addImportPath(ARCGIS_RUNTIME_IMPORT_PATH_2);
#endif

  // Set the source
  view.setSource(QUrl("qrc:/Samples/Maps/BrowseBuildingFloors/BrowseBuildingFloors.qml"));

  view.show();

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
      qWarning() << "Use of Esri location services, including basemaps, requires"
                    " you to authenticate with an ArcGIS identity or set the API Key property.";
      return;
    }
  }

  QCoreApplication::instance()->setProperty("Esri.ArcGISRuntime.apiKey", apiKey);
}
