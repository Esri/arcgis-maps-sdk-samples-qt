#-------------------------------------------------
# Copyright 2017 Esri.

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

ARCGIS_RUNTIME_VERSION = 200.8.0
include($$PWD/arcgisruntime.pri)

TEMPLATE = app
TARGET = LocalServerMapImageLayer

#-------------------------------------------------------------------------------

HEADERS += \
    LocalServerMapImageLayer.h

SOURCES += \
    main.cpp \
    LocalServerMapImageLayer.cpp

RESOURCES += LocalServerMapImageLayer.qrc

#-------------------------------------------------------------------------------

win32 {
    LIBS += \
        Ole32.lib
}

ios:android:macx {
    message("Local Server not supported on this platform")
}
