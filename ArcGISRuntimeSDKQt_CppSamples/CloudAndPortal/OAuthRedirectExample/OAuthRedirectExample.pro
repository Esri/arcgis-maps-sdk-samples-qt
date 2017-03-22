TEMPLATE = app

QT += core gui opengl xml network positioning sensors
QT += qml quick
CONFIG += c++11

ARCGIS_RUNTIME_VERSION = 100.1
include($$PWD/arcgisruntime.pri)

HEADERS += OAuthRedirectExample.h \
    OAuthRedirectHandler.h \
    MyApplication.h

SOURCES += main.cpp \
    OAuthRedirectExample.cpp \
    OAuthRedirectHandler.cpp \
    MyApplication.cpp

RESOURCES += OAuthRedirectExample.qrc

win32 {
    include (Win/Win.pri)
}

macx {
    include (Mac/Mac.pri)
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11
}

ios {
    include (iOS/iOS.pri)
}

android {
    include (Android/Android.pri)
}

DEFINES +="CLIENT_ID=\"enter client id\""

win32|unix:!macx {
    message("Using QtSingleApplication")
    QT_SINGLEAPP_DIR = "$$PWD/../../../../../../../qt-solutions/qtsingleapplication"
    include("$$QT_SINGLEAPP_DIR/src/qtsingleapplication.pri")
}
