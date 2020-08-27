#-------------------------------------------------
# Copyright 2015 Esri.

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at:
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

CONFIG += c++14

# additional modules are pulled in via arcgisruntime.pri
QT += opengl widgets

ARCGIS_RUNTIME_VERSION = 100.9
include($$PWD/arcgisruntime.pri)

win32:CONFIG += \
  embed_manifest_exe

TARGET = ManageBookmarks
TEMPLATE = app

SOURCES += \
    main.cpp \
    ManageBookmarks.cpp

HEADERS  += \
    ManageBookmarks.h

