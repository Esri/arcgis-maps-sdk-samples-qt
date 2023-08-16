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

#include "IntegratedWindowsAuthentication.h"
#include "ArcGISRuntimeEnvironment.h"

#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#ifdef QT_WEBVIEW_WEBENGINE_BACKEND
#include <QtWebEngineQuick>
#endif // QT_WEBVIEW_WEBENGINE_BACKEND

#define STRINGIZE(x) #x
#define QUOTE(x) STRINGIZE(x)

#include "Esri/ArcGISRuntime/Toolkit/register.h"

int main(int argc, char *argv[])
{
<<<<<<< HEAD
  // Enforce OpenGL
  qputenv("QSG_RHI_BACKEND", "opengl");

#ifdef QT_WEBVIEW_WEBENGINE_BACKEND
  QtWebEngineQuick::initialize();
#endif // QT_WEBVIEW_WEBENGINE_BACKEND
=======
#ifdef QT_WEBVIEW_WEBENGINE_BACKEND
  QtWebEngineQuick::initialize();
#endif // QT_WEBVIEW_WEBENGINE_BACKEND

  QGuiApplication app(argc, argv);
  app.setApplicationName(QStringLiteral("IntegratedWindowsAuthentication - C++"));

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
>>>>>>> v.next

  QGuiApplication app(argc, argv);
  app.setApplicationName(QStringLiteral("IntegratedWindowsAuthentication - C++"));

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
  IntegratedWindowsAuthentication::init();

  QString arcGISRuntimeImportPath = QUOTE(ARCGIS_RUNTIME_IMPORT_PATH);

#if defined(LINUX_PLATFORM_REPLACEMENT)
  // on some linux platforms the string 'linux' is replaced with 1
  // fix the replacement paths which were created
  QString replaceString = QUOTE(LINUX_PLATFORM_REPLACEMENT);
  arcGISRuntimeImportPath = arcGISRuntimeImportPath.replace(replaceString, "linux", Qt::CaseSensitive);
#endif

  // Initialize application view
  QQmlApplicationEngine engine;
  // Add the import Path
  engine.addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));
  // Add the Runtime and Extras path
  engine.addImportPath(arcGISRuntimeImportPath);

  Esri::ArcGISRuntime::Toolkit::registerComponents(engine);

  // Set the source
  engine.load(QUrl("qrc:/Samples/CloudAndPortal/IntegratedWindowsAuthentication/main.qml"));

  return app.exec();
}
