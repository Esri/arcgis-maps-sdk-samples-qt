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

#include <QGuiApplication>
#include <QQuickView>
#include <QUrl>
#include <QCoreApplication>
#include <QDir>
#include <QQmlEngine>
#include <QSurfaceFormat>

#define STRINGIZE(x) #x
#define QUOTE(x) STRINGIZE(x)

int main(int argc, char *argv[])
{
#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    // Linux requires 3.2 OpenGL Context
    // in order to instance 3D symbols
    QSurfaceFormat fmt = QSurfaceFormat::defaultFormat();
    fmt.setVersion(3, 2);
    QSurfaceFormat::setDefaultFormat(fmt);
#endif

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    app.setApplicationName("BasicSceneView - QML");

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
      qWarning() << "Use of Esri location services, including basemaps, requires "
                    "you to authenticate with an ArcGIS identity or set the API Key property.";
  }
  else
  {
      QCoreApplication::instance()->setProperty("Esri.ArcGISRuntime.apiKey", apiKey);
  }

    // Initialize application view
    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);

    // Add the import Path
    view.engine()->addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));

    QString arcGISRuntimeImportPath = QUOTE(ARCGIS_RUNTIME_IMPORT_PATH);

#if defined(LINUX_PLATFORM_REPLACEMENT)
    // on some linux platforms the string 'linux' is replaced with 1
    // fix the replacement paths which were created
    QString replaceString = QUOTE(LINUX_PLATFORM_REPLACEMENT);
    arcGISRuntimeImportPath = arcGISRuntimeImportPath.replace(replaceString, "linux", Qt::CaseSensitive);
#endif

    // Add the Runtime and Extras path
    view.engine()->addImportPath(arcGISRuntimeImportPath);

    // Set the source
    view.setSource(QUrl("qrc:/Samples/Scenes/BasicSceneView/BasicSceneView.qml"));

    view.show();

    return app.exec();
}
