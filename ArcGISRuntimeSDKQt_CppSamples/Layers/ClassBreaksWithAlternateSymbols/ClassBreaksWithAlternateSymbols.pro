#-------------------------------------------------
#  Copyright 2022 ESRI
#
#  All rights reserved under the copyright laws of the United States
#  and applicable international laws, treaties, and conventions.
#
#  You may freely redistribute and use this sample code, with or
#  without modification, provided you include the original copyright
#  notice and use restrictions.
#
#  See the Sample code usage restrictions document for further information.
#-------------------------------------------------

TEMPLATE = app

CONFIG += c++14

# additional modules are pulled in via arcgisruntime.pri
QT += opengl qml quick

TARGET = ClassBreaksWithAlternateSymbols

equals(QT_MAJOR_VERSION, 5) {
    lessThan(QT_MINOR_VERSION, 15) { 
        error("$$TARGET requires Qt 5.15.2")
    }
	equals(QT_MINOR_VERSION, 15) : lessThan(QT_PATCH_VERSION, 2) {
		error("$$TARGET requires Qt 5.15.2")
	}
}

ARCGIS_RUNTIME_VERSION = 100.13.1
include($$PWD/arcgisruntime.pri)

HEADERS += \
    ClassBreaksWithAlternateSymbols.h

SOURCES += \
    main.cpp \
    ClassBreaksWithAlternateSymbols.cpp

RESOURCES += \
    qml/qml.qrc \
    Resources/Resources.qrc

#-------------------------------------------------------------------------------

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
