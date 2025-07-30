// [Legal]
// Copyright 2024 Esri.
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
#include "ArcGISRuntimeEnvironment.h"
#include "ValidateUtilityNetworkTopology.h"

// Qt headers
#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

// Platform specific headers
#ifdef Q_OS_WIN
#include <Windows.h>
#endif

int main(int argc, char *argv[])
{
  Esri::ArcGISRuntime::ArcGISRuntimeEnvironment::setUseLegacyAuthentication(false);
  QGuiApplication app(argc, argv);
  app.setApplicationName(QString("ValidateUtilityNetworkTopology"));

  // Initialize the sample
  ValidateUtilityNetworkTopology::init();

  // Initialize application view
  QQmlApplicationEngine engine;
  // Add the import Path
  engine.addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));

#ifdef ARCGIS_RUNTIME_IMPORT_PATH_2
  engine.addImportPath(ARCGIS_RUNTIME_IMPORT_PATH_2);
#endif

  // Set the source
  engine.load(QUrl("qrc:/Samples/UtilityNetwork/ValidateUtilityNetworkTopology/main.qml"));

  return app.exec();
}
