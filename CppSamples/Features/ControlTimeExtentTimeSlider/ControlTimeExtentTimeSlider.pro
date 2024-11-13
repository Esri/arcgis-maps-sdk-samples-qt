#-------------------------------------------------
# Copyright 2021 Esri.

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

TEMPLATE = app
TARGET = ControlTimeExtentTimeSlider

ARCGIS_RUNTIME_VERSION = 200.5.0
include($$PWD/arcgisruntime.pri)


# path of the toolkit relative to the sample
TOOLKIT_PRI_PATH = $$PWD/../../../arcgis-maps-sdk-toolkit-qt

exists($$TOOLKIT_PRI_PATH/uitools/toolkitcpp.pri) {
    include($$TOOLKIT_PRI_PATH/uitools/toolkitcpp.pri)
} else {
    error(TOOLKIT_PRI_PATH is missing which is required to build this application.)
}
#-------------------------------------------------------------------------------

HEADERS += \
    ControlTimeExtentTimeSlider.h

SOURCES += \
    main.cpp \
    ControlTimeExtentTimeSlider.cpp

RESOURCES += ControlTimeExtentTimeSlider.qrc

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

    QMAKE_INFO_PLIST = $$PWD/Info.plist
}

android {
    INCLUDEPATH += $$PWD
    DEPENDPATH += $$PWD
}
