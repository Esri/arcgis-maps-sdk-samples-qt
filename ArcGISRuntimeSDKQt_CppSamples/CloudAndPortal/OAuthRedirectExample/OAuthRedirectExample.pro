TEMPLATE = app

# additional modules are pulled in via arcgisruntime.pri
QT += opengl qml quick

CONFIG += c++14

ARCGIS_RUNTIME_VERSION = 100.9
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
}

ios {
    include (iOS/iOS.pri)
}

android {
    include (Android/Android.pri)
}

# Update CLIENT_ID to the Client ID of the app you registered online
DEFINES +="CLIENT_ID=\"enter client id\""
# Update URL_SCHEME to the custom url which you have added to the Redirect URI for the app
# and registered for the Operating System
DEFINES +="URL_SCHEME=\"exampleapp\""

win32|unix:!macx {
    message("Using QtSingleApplication")
    # Update QT_SINGLEAPP_DIR to the location where you cloned the qt-solutions repository
    QT_SINGLEAPP_DIR = "$$PWD/../../../../../../../qt-solutions/qtsingleapplication"
    include("$$QT_SINGLEAPP_DIR/src/qtsingleapplication.pri")
}
