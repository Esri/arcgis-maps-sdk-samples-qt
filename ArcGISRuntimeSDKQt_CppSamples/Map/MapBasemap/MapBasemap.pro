#-------------------------------------------------
#  Copyright 2015 ESRI
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

mac {
    cache()
}

CONFIG += c++11 esri_runtime_qt100_0_0

QT += core gui opengl xml network positioning sensors

win32:CONFIG += \
  embed_manifest_exe

TARGET = MapBasemap
TEMPLATE = app

SOURCES += \
    main.cpp \
    MapBasemap.cpp

HEADERS  += \
    MapBasemap.h
