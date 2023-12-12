#-------------------------------------------------
# Copyright 2020 Esri.

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

TEMPLATE = app

# additional modules are pulled in via arcgisruntime.pri
QT += opengl qml quick

CONFIG += c++17

ARCGIS_RUNTIME_VERSION = 200.4.0
include($$PWD/arcgisruntime.pri)

SOURCES += \
    main.cpp

RESOURCES += \
    ExploreScenesInFlyoverAR.qrc

ios {
    QMAKE_INFO_PLIST = $$PWD/Info.plist
}

#-------------------------------------------------------------------------------
# AR configuration

# The path to the ArcGIS runtime toolkit for Qt sources, corresponding to the files downloaded
# from the GitHub repo: https://github.com/Esri/arcgis-maps-sdk-toolkit-qt

# path of the toolkit relative to the sample
ARCGIS_TOOLKIT_PATH = $$PWD/../../../arcgis-maps-sdk-toolkit-qt

exists($$ARCGIS_TOOLKIT_PATH/augmentedreality/QmlApi/ArQmlApi.pri) {
    include($$ARCGIS_TOOLKIT_PATH/augmentedreality/QmlApi/ArQmlApi.pri)
} else {
    error(ARCGIS_TOOLKIT_PATH is missing which is required to build this application.)
}

#-------------------------------------------------------------------------------

android {
    ANDROID_ABIS = arm64-v8a
}

contains(ANDROID_TARGET_ARCH,arm64-v8a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}
