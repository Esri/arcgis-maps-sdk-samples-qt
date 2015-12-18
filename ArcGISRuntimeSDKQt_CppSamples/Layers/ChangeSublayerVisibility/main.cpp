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

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include "MapQuickView.h"
#include "ArcGISSublayerListModel.h"
#include "ChangeSublayerVisibility.h"
#include "ArcGISRuntimeEnvironment.h"

using namespace Esri::ArcGISRuntime;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

#ifdef Q_OS_WIN
    // Force usage of OpenGL ES through ANGLE on Windows
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
#endif

    // Register the map view for QML
    qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
    qmlRegisterUncreatableType<ArcGISSublayerListModel>("Esri.Samples", 1, 0, "ArcGISSublayerListModel", "ArcGISSublayerListModel is an uncreatable type");
    qmlRegisterType<ChangeSublayerVisibility>("Esri.Samples", 1, 0, "ChangeSublayerVisibilitySample");

    // Intialize application view
    QQuickView view(QUrl("qrc:/Samples/Layers/ChangeSublayerVisibility/ChangeSublayerVisibility.qml"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.show();

    return app.exec();
}


