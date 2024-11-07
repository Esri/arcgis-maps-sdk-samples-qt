#-------------------------------------------------
# Copyright 2022 Esri.

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#-------------------------------------------------

mac {
    cache()
}

#-------------------------------------------------------------------------------

CONFIG += c++17

# additional modules are pulled in via arcgisruntime.pri
QT += opengl qml quick
android|ios: QT += bluetooth

TEMPLATE = app
TARGET = ShowDeviceLocationUsingIndoorPositioning

ARCGIS_RUNTIME_VERSION = 200.6.0
include($$PWD/arcgisruntime.pri)

#-------------------------------------------------------------------------------

HEADERS += \
    ShowDeviceLocationUsingIndoorPositioning.h \
    IndoorsLocationDataSourceCreator.h

SOURCES += \
    IndoorsLocationDataSourceCreator.cpp \
    main.cpp \
    ShowDeviceLocationUsingIndoorPositioning.cpp

RESOURCES += ShowDeviceLocationUsingIndoorPositioning.qrc

#-------------------------------------------------------------------------------

win32 {
    LIBS += \
        Ole32.lib
}

ios {
    INCLUDEPATH += $$PWD
    DEPENDPATH += $$PWD

    OTHER_FILES += \
        $$PWD/Info.plist

    QMAKE_INFO_PLIST = $$PWD/ios/Info.plist
}

macx {
    QMAKE_INFO_PLIST = $$PWD/mac/Info.plist
}

android {
    INCLUDEPATH += $$PWD
    DEPENDPATH += $$PWD
}
