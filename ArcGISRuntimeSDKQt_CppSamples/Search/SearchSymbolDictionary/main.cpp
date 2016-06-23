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
#include <QSurfaceFormat>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include "SearchSymbolDictionary.h"
#include "StyleSymbolSearchResultListModel.h"

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    if (QT_VERSION_MAJOR == 5 && QT_VERSION_MINOR > 6)
    {
        // Workaround for Qt versions greater than 5.6
        // Force to OpenGL ES 3
        QSurfaceFormat fmt = QSurfaceFormat::defaultFormat();
        fmt.setVersion(3, 0);
        QSurfaceFormat::setDefaultFormat(fmt);
    }
#endif
    
    QGuiApplication app(argc, argv);

#if defined(Q_OS_WIN)
    if (QT_VERSION_MAJOR == 5 && QT_VERSION_MINOR > 6)
    {
        // Workaround for Qt versions greater than 5.6
        // Force to OpenGL ES 3
        QSurfaceFormat fmt = QSurfaceFormat::defaultFormat();
        fmt.setVersion(3, 0);
        QSurfaceFormat::setDefaultFormat(fmt);
    }
#endif

#ifdef Q_OS_WIN
    // Force usage of OpenGL ES through ANGLE on Windows
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
#endif

    // Register the map view for QML
    qmlRegisterType<SearchSymbolDictionary>("Esri.Samples", 1, 0, "SearchSymbolDictionarySample");

    // Register the list model
    qmlRegisterUncreatableType<Esri::ArcGISRuntime::StyleSymbolSearchResultListModel>("Esri.Samples", 1, 0,
                                                       "StyleSymbolSearchResultListModel",
                                                       "StyleSymbolSearchResultListModel is an uncreatable type");

    // Intialize application view
    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);

    // Add the import Path
    view.engine()->addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));

    // Set the source
    view.setSource(QUrl("qrc:/Samples/Search/SearchSymbolDictionary/SearchSymbolDictionary.qml"));

    view.show();

    return app.exec();
}
