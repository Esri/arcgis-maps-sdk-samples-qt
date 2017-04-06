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

#include <QSettings>
#include <QGuiApplication>
#include <QQuickView>
#include <QCommandLineParser>
#include <QDir>
#include <QQmlEngine>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include "SearchDictionarySymbolStyle.h"

#define STRINGIZE(x) #x
#define QUOTE(x) STRINGIZE(x)

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);

#ifdef Q_OS_WIN
  // Force usage of OpenGL ES through ANGLE on Windows
  QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
#endif

  // Initialize the sample
  SearchDictionarySymbolStyle::init();

  // Intialize application view
  QQuickView view;
  view.setResizeMode(QQuickView::SizeRootObjectToView);

  // Add the import Path
  view.engine()->addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));
  // Add the Extras path
  view.engine()->addImportPath(QUOTE(ARCGIS_RUNTIME_IMPORT_PATH));
  // Add the Toolkit path
  view.engine()->addImportPath(QUOTE(ARCGIS_TOOLKIT_IMPORT_PATH));

  // Set the source
  view.setSource(QUrl("qrc:/Samples/Search/SearchDictionarySymbolStyle/SearchDictionarySymbolStyle.qml"));

  view.show();

  return app.exec();
}
