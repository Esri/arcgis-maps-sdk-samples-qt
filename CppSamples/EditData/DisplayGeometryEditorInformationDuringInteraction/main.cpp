// [WriteFile Name=DisplayGeometryEditorInformationDuringInteraction, Category=EditData]
// [Legal]
// Copyright 2026 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

// sample headers
#include "DisplayGeometryEditorInformationDuringInteraction.h"

// ArcGIS Maps SDK headers
#include "ArcGISRuntimeEnvironment.h"

// Qt headers
#include <QCommandLineParser>
#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

void setAPIKey(const QGuiApplication& app, QString apiKey);

int main(int argc, char* argv[])
{
  QGuiApplication app(argc, argv);
  QGuiApplication::setApplicationName(QString("DisplayGeometryEditorInformationDuringInteraction"));

  const QString accessToken = QString("");
  setAPIKey(app, accessToken);

  DisplayGeometryEditorInformationDuringInteraction::init();

  QQmlApplicationEngine engine;
  engine.addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));

#ifdef ARCGIS_RUNTIME_IMPORT_PATH_2
  engine.addImportPath(ARCGIS_RUNTIME_IMPORT_PATH_2);
#endif

  engine.load(QUrl("qrc:/Samples/EditData/DisplayGeometryEditorInformationDuringInteraction/main.qml"));

  return QGuiApplication::exec();
}

void setAPIKey(const QGuiApplication& app, QString apiKey)
{
  if (apiKey.isEmpty())
  {
    QCommandLineParser commandLineParser;
    QCommandLineOption apiKeyArgument(QStringList{"k", "api"}, "The API Key property used to access Esri location services", "apiKeyInput");
    commandLineParser.addOption(apiKeyArgument);
    commandLineParser.process(app);

    apiKey = commandLineParser.value(apiKeyArgument);

    if (apiKey.isEmpty())
    {
      qWarning() << "Use of Esri location services, including basemaps, requires"
                 << "you to authenticate with an ArcGIS identity or set the API Key property.";
      return;
    }
  }

  Esri::ArcGISRuntime::ArcGISRuntimeEnvironment::setApiKey(apiKey);
}
