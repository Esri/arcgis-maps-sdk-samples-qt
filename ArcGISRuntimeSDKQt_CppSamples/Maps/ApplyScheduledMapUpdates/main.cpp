// Copyright 2019 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include "ApplyScheduledMapUpdates.h"

#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#define STRINGIZE(x) #x
#define QUOTE(x) STRINGIZE(x)

int main(int argc, char *argv[])
{
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication app(argc, argv);
  app.setApplicationName(QStringLiteral("ApplyScheduledMapUpdates - C++"));

  // Initialize the sample
  ApplyScheduledMapUpdates::init();

  QString arcGISRuntimeImportPath = QUOTE(ARCGIS_RUNTIME_IMPORT_PATH);
  QString arcGISToolkitImportPath = QUOTE(ARCGIS_TOOLKIT_IMPORT_PATH);

#if defined(LINUX_PLATFORM_REPLACEMENT)
  // on some linux platforms the string 'linux' is replaced with 1
  // fix the replacement paths which were created
  QString replaceString = QUOTE(LINUX_PLATFORM_REPLACEMENT);
  arcGISRuntimeImportPath = arcGISRuntimeImportPath.replace(replaceString, "linux", Qt::CaseSensitive);
  arcGISToolkitImportPath = arcGISToolkitImportPath.replace(replaceString, "linux", Qt::CaseSensitive);
#endif

  // Initialize application view
  QQmlApplicationEngine engine;
  // Add the import Path
  engine.addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));
  // Add the Runtime and Extras path
  engine.addImportPath(arcGISRuntimeImportPath);
  // Add the Toolkit path
  engine.addImportPath(arcGISToolkitImportPath);

  // Set the source
  engine.load(QUrl("qrc:/Samples/Maps/ApplyScheduledMapUpdates/main.qml"));

  return app.exec();
}
