// Copyright 2015 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <QApplication>
#include <QQuickView>
#include <QCommandLineParser>
#include <QDir>
#include <QQmlEngine>
#include <QQmlApplicationEngine>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include "RasterLayerFile.h"

#define STRINGIZE(x) #x
#define QUOTE(x) STRINGIZE(x)

int main(int argc, char *argv[])
{
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication app(argc, argv);
  app.setApplicationName("RasterLayerFile - C++");

  // Initialize the sample
  RasterLayerFile::init();

  // Initialize application engine
  QQmlApplicationEngine engine;

  // Add the import Path
  engine.addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));
  // Add the Extras path
  engine.addImportPath(QUOTE(ARCGIS_RUNTIME_IMPORT_PATH));
  // Add the Toolkit path
  engine.addImportPath(QUOTE(ARCGIS_TOOLKIT_IMPORT_PATH));

  // Set the source
  engine.load(QUrl("qrc:/Samples/Layers/RasterLayerFile/main.qml"));

  return app.exec();
}
