#-------------------------------------------------
# Copyright 2019 Esri.

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
TARGET = PlayAKmlTour

ARCGIS_RUNTIME_VERSION = 200.7.0
include($$PWD/arcgisruntime.pri)

#-------------------------------------------------------------------------------

HEADERS += \
    PlayAKmlTour.h

SOURCES += \
    main.cpp \
    PlayAKmlTour.cpp

RESOURCES += PlayAKmlTour.qrc

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

    # workaround for https://bugreports.qt.io/browse/QTBUG-129651
    # ArcGIS Maps SDK for Qt adds 'QMAKE_RPATHDIR = @executable_path/Frameworks'
    # and ffmpeg frameworks have embedded '@rpath/Frameworks' path.
    # so in order for them to be found, we need to add @executable_path to the
    # search path.
    QMAKE_LFLAGS += -F$$(QTDIR)/lib/ffmpeg -Wl,-rpath,@executable_path
    LIBS += -framework libavcodec \
            -framework libavformat \
            -framework libavutil \
            -framework libswresample \
            -framework libswscale
    ffmpeg.files = $$(QTDIR)/lib/ffmpeg/libavcodec.framework \
                   $$(QTDIR)/lib/ffmpeg/libavformat.framework \
                   $$(QTDIR)/lib/ffmpeg/libavutil.framework \
                   $$(QTDIR)/lib/ffmpeg/libswresample.framework \
                   $$(QTDIR)/lib/ffmpeg/libswscale.framework
    ffmpeg.path = Frameworks
    QMAKE_BUNDLE_DATA += ffmpeg
}

android {
    INCLUDEPATH += $$PWD
    DEPENDPATH += $$PWD
}
