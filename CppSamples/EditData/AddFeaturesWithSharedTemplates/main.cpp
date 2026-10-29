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
#include "AddFeaturesWithSharedTemplates.h"

// ArcGIS Maps SDK headers
#include "ArcGISRuntimeEnvironment.h"

// Qt headers
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>

int main(int argc, char* argv[])
{
  QGuiApplication app(argc, argv);
  QGuiApplication::setApplicationName(QStringLiteral("AddFeaturesWithSharedTemplates"));

  QCommandLineParser parser;
  parser.addHelpOption();
  const QCommandLineOption apiKeyOption(QStringList{QStringLiteral("k"), QStringLiteral("api")},
                                        QStringLiteral("The API key used to access ArcGIS location services."), QStringLiteral("apiKey"));
  parser.addOption(apiKeyOption);
  parser.process(app);
  const QString accessToken = parser.value(apiKeyOption);
  if (accessToken.isEmpty())
  {
    qWarning() << "Use of ArcGIS location services requires an ArcGIS identity or an API key.";
  }
  else
  {
    Esri::ArcGISRuntime::ArcGISRuntimeEnvironment::setApiKey(accessToken);
  }

  AddFeaturesWithSharedTemplates::init();
  QQmlApplicationEngine engine;
  engine.addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("qml")));
#ifdef ARCGIS_RUNTIME_IMPORT_PATH_2
  engine.addImportPath(ARCGIS_RUNTIME_IMPORT_PATH_2);
#endif
  engine.load(QUrl(QStringLiteral("qrc:/Samples/EditData/AddFeaturesWithSharedTemplates/main.qml")));
  if (engine.rootObjects().isEmpty())
  {
    return 1;
  }
  return QGuiApplication::exec();
}
