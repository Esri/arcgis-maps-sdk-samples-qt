
// Copyright 2016 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//
#include <QQuickView>

#include <QDir>
#include <QQmlEngine>
#include <QScopedPointer>

#if defined(Q_OS_WIN) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
#include <QCommandLineParser>
#include <QSettings>
#endif

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include "MyApplication.h"
#include "OAuthRedirectExample.h"
#include "ArcGISRuntimeEnvironment.h"

#define STRINGIZE(x) #x
#define QUOTE(x) STRINGIZE(x)

using namespace Esri::ArcGISRuntime;

int main(int argc, char *argv[])
{
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  MyApplication app(argc, argv);
  app.setApplicationName("OAuth Redirect Example - C++");

  // Use of Esri location services, including basemaps and geocoding,
  // requires authentication using either an ArcGIS identity or an API Key.
  // 1. ArcGIS identity: An ArcGIS named user account that is a member of an
  //    organization in ArcGIS Online or ArcGIS Enterprise.
  // 2. API key: A permanent key that gives your application access to Esri
  //    location services. Visit your ArcGIS Developers Dashboard create a new
  //    API keys or access an existing API key.
  const QString apiKey = "";
  if (apiKey.isEmpty())
  {
      qWarning() << "Use of Esri location services, including basemaps, requires"
                    "you to authenticate with an ArcGIS identity or set the API Key property.";
  }
  else
  {
      Esri::ArcGISRuntime::ArcGISRuntimeEnvironment::setApiKey(apiKey);
  }
  app.setApplicationName("OAuth Redirect Example - C++");

#if defined(Q_OS_WIN) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
  QCommandLineParser commandLineParser;
  commandLineParser.process(app);

  QStringList argList = commandLineParser.positionalArguments();

  if (app.sendMessage(argList.join(",")))
    return 0;
#endif

#ifdef Q_OS_WIN
  const QString urlScheme = OAuthRedirectExample::customUrlProtocol();
  const QString appPath(QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
  const QString regPath = QStringLiteral("HKEY_CURRENT_USER\\Software\\Classes\\") + urlScheme;

  QScopedPointer<QSettings> reg(new QSettings(regPath, QSettings::NativeFormat));

  reg->setValue(QStringLiteral("Default"), QString("URL:%1 protocol").arg(urlScheme));
  reg->setValue(QStringLiteral("URL Protocol"), QString());

  reg->beginGroup(QStringLiteral("DefaultIcon"));
  reg->setValue(QStringLiteral("Default"), QString("%1,1").arg(appPath));
  reg->endGroup();

  reg->beginGroup(QStringLiteral("shell"));
  reg->beginGroup(QStringLiteral("open"));
  reg->beginGroup(QStringLiteral("command"));
  reg->setValue(QStringLiteral("Default"), appPath + QLatin1String(" %1"));
#endif

  // Register the OAuthRedirectTest (QQuickItem) for QML
  qmlRegisterType<OAuthRedirectExample>("Esri.Samples", 1, 0, "OAuthRedirectExample");

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
  view.setSource(QUrl("qrc:/Samples/CloudAndPortal/OAuthRedirectExample/OAuthRedirectExample.qml"));

#if defined(Q_OS_WIN) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
  QObject::connect(&app, &MyApplication::messageReceived, [&app, &view](const QString& msg)
  {
    Q_UNUSED(msg);
    view.showMinimized();
    app.activateWindow();
    view.show();
    view.raise();
  });
#endif

  view.show();

  return app.exec();
}
