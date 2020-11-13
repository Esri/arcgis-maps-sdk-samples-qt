// Copyright 2020 Esri.

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

#include "IdentifyKmlFeatures.h"

#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "Esri/ArcGISRuntime/Toolkit/register.h"

int main(int argc, char *argv[])
{
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication app(argc, argv);
  app.setApplicationName(QStringLiteral("IdentifyKmlFeatures - C++"));

  // Initialize the sample
  IdentifyKmlFeatures::init();

  // Initialize application view
  QQmlApplicationEngine engine;
  // Add the import Path
  engine.addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));

#ifdef ARCGIS_RUNTIME_IMPORT_PATH_2
  engine.addImportPath(ARCGIS_RUNTIME_IMPORT_PATH_2);
#endif

  Esri::ArcGISRuntime::Toolkit::registerComponents(engine);

  // Set the source
  engine.load(QUrl("qrc:/Samples/Layers/IdentifyKmlFeatures/main.qml"));

  return app.exec();
}
