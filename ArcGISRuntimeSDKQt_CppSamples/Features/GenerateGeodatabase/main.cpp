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

#include <QSettings>
#include <QGuiApplication>
#include <QQuickView>
#include <QCommandLineParser>
#include <QDir>
#include <QQmlEngine>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include "MapQuickView.h"
#include "GenerateGeodatabase.h"
#include "ArcGISRuntimeEnvironment.h"

using namespace Esri::ArcGISRuntime;

int main(int argc, char *argv[])
{
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
    qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
    qmlRegisterType<GenerateGeodatabase>("Esri.Samples", 1, 0, "GenerateGeodatabaseSample");

    // Intialize application view
    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);

    // Add the import Path
    view.engine()->addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));

    // Set the source
    view.setSource(QUrl("qrc:/Samples/Features/GenerateGeodatabase/GenerateGeodatabase.qml"));

    view.show();

    return app.exec();
}


