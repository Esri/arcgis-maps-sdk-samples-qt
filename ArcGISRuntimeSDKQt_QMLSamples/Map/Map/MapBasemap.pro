# Copyright 2015 ESRI
#
# All rights reserved under the copyright laws of the United States
# and applicable international laws, treaties, and conventions.
#
# You may freely redistribute and use this sample code, with or
# without modification, provided you include the original copyright
# notice and use restrictions.
#
# See the Sample code usage restrictions document for further information.
#

TEMPLATE = app

QT += qml quick

SOURCES += main.cpp

RESOURCES += MapBasemap.qrc

ios {
    QMAKE_INFO_PLIST = $$PWD/Info.plist
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
