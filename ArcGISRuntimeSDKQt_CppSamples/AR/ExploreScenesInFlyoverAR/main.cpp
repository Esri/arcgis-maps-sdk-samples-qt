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

#include "ExploreScenesInFlyoverAR.h"
#include "ArcGISRuntimeEnvironment.h"

#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QSurfaceFormat>

// Include the AR view from toolkit
#include "ArcGISArView.h"

int main(int argc, char *argv[])
{
  // There are some conflicts between the AR frameworks and Qt's rendering thread.
  // See Qt's documentation about non-threaded render loops for more information.
  // https://doc.qt.io/qt-5/qtquick-visualcanvas-scenegraph.html#non-threaded-render-loops-basic-and-windows
#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
  qputenv("QSG_RENDER_LOOP", "basic");
#endif

  // register the ArcGISArView and LocationDataSource with the QML engine before it's creation
  Esri::ArcGISRuntime::Toolkit::ArcGISArView::qmlRegisterTypes();

#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
  // Linux requires 3.2 OpenGL Context
  // in order to instance 3D symbols
  QSurfaceFormat fmt = QSurfaceFormat::defaultFormat();
  fmt.setVersion(3, 2);
  QSurfaceFormat::setDefaultFormat(fmt);
#endif

  QGuiApplication app(argc, argv);
  app.setApplicationName(QStringLiteral("ExploreScenesInFlyoverAR - C++"));

  // Use of Esri location services, including basemaps and geocoding,
  // requires authentication using either an ArcGIS identity or an API Key.
  // 1. ArcGIS identity: An ArcGIS named user account that is a member of an
  //    organization in ArcGIS Online or ArcGIS Enterprise.
  // 2. API key: A permanent key that gives your application access to Esri
  //    location services. Visit your ArcGIS Developers Dashboard create a new
  //    API keys or access an existing API key.
  const QString apiKey = QString("");
  if (apiKey.isEmpty())
  {
      qWarning() << "Use of Esri location services, including basemaps, requires" <<
                    "you to authenticate with an ArcGIS identity or set the API Key property.";
  }
  else
  {
      Esri::ArcGISRuntime::ArcGISRuntimeEnvironment::setApiKey(apiKey);
  }

  // Initialize the sample
  ExploreScenesInFlyoverAR::init();

  // Initialize application view
  QQmlApplicationEngine engine;
  // Add the import Path
  engine.addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));

#ifdef ARCGIS_RUNTIME_IMPORT_PATH_2
  engine.addImportPath(ARCGIS_RUNTIME_IMPORT_PATH_2);
#endif

  // Set the source
  engine.load(QUrl("qrc:/Samples/AR/ExploreScenesInFlyoverAR/main.qml"));

  return app.exec();
}
