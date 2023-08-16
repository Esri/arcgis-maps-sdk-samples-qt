#-------------------------------------------------

# [Legal]
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
# [Legal]

#-------------------------------------------------

TEMPLATE = app
QT += core gui xml network qml quick positioning sensors multimedia
QT += widgets quickcontrols2 opengl webview core5compat websockets texttospeech
TARGET = ArcGISQt_CppSamples
DEFINES += CPP_VIEWER
DEFINES += Qt_Version=\"$$QT_VERSION\"
SAMPLEPATHCPP = $$PWD/../../ArcGISRuntimeSDKQt_CppSamples
COMMONVIEWER = $$PWD/../ArcGISRuntimeSDKQt_Samples
PCH_HEADER = $$COMMONVIEWER/pch.hpp
ARCGIS_RUNTIME_VERSION = 200.2.0
DEFINES += ArcGIS_Runtime_Version=$$ARCGIS_RUNTIME_VERSION

# This block determines whether to build against the installed SDK or the local dev build area
exists($$PWD/../../../../DevBuildCpp.pri) {
  message("Building against the dev environment")
  DEFINES += ESRI_BUILD
  DEFINES += SAMPLE_VIEWER_API_KEY=$$(SAMPLEVIEWERAPIKEY_INTERNAL)

  # use the Esri dev build script
  include ($$PWD/../../../../DevBuildCpp.pri)
  # include the toolkitcpp.pri, which contains all the toolkit resources
  include($$PWD/../../../toolkit/uitools/toolkitcpp.pri)

  INCLUDEPATH += \
      $$COMMONVIEWER \
      $$COMMONVIEWER/SyntaxHighlighter \
      $$PWD/../../../../api/qt_cpp/Include \
      $$PWD/../../../../api/qt_cpp/Include/LocalServer/ \
} else {
  message("Building against the installed SDK")
  CONFIG += build_from_setup
  CONFIG += c++17

  # include the toolkitcpp.pri, which contains all the toolkit resources
  !include($$PWD/../../arcgis-maps-sdk-toolkit-qt/uitools/toolkitcpp.pri) {
    message("ERROR: Cannot find toolkitcpp.pri at path:" $$PWD/../../arcgis-maps-sdk-toolkit-qt/uitools/toolkitcpp.pri)
    message("Please ensure the Qt Toolkit repository is cloned and the path above is correct.")
  }

  contains(QMAKE_HOST.os, Windows):{
    iniPath = $$(ALLUSERSPROFILE)\\EsriRuntimeQt\\ArcGIS Runtime SDK for Qt $${ARCGIS_RUNTIME_VERSION}.ini
  } else {
    userHome = $$system(echo $HOME)
    iniPath = $${userHome}/.config/EsriRuntimeQt/ArcGIS Runtime SDK for Qt $${ARCGIS_RUNTIME_VERSION}.ini
  }
  iniLine = $$cat($${iniPath}, "lines")
  dirPath = $$find(iniLine, "InstallDir")
  cleanDirPath = $$replace(dirPath, "InstallDir=", "")
  priLocation = $$replace(cleanDirPath, '"', "")
  !include($$priLocation/sdk/ideintegration/arcgis_runtime_qml_cpp.pri) {
    message("Error. Cannot locate ArcGIS Runtime PRI file")
  }

  INCLUDEPATH += \
      $$COMMONVIEWER \
      $$COMMONVIEWER/SyntaxHighlighter \
      $$priLocation/sdk/include \
      $$priLocation/sdk/include/LocalServer

  DEFINES += BUILD_FROM_SETUP
}

qtHaveModule(webenginequick) {
  QT += webenginequick
  DEFINES += QT_WEBVIEW_WEBENGINE_BACKEND
}

QMAKE_TARGET_COMPANY = Esri, Inc.
QMAKE_TARGET_PRODUCT = ArcGIS Runtime SDK for Qt Samples - Cpp
QMAKE_TARGET_DESCRIPTION = ArcGIS Runtime SDK for Qt Samples - Cpp
QMAKE_TARGET_COPYRIGHT = Copyright 2017 Esri Inc.

# include the samples.pri, which contains all the sample resources
include(samples.pri)

CONFIG(precompile_header): DEFINES += PCH_BUILD

android {
  QMAKE_CXXFLAGS_USE_PRECOMPILE = -include-pch \${QMAKE_PCH_OUTPUT}
  PRECOMPILED_DIR = pch
}

HEADERS += \
    CppSampleManager.h \
    $$PCH_HEADER \
    $$COMMONVIEWER/SyntaxHighlighter/syntax_highlighter.h \
    $$COMMONVIEWER/SyntaxHighlighter/QMLHighlighter.h \
    $$COMMONVIEWER/CategoryListModel.h \
    $$COMMONVIEWER/DataItem.h \
    $$COMMONVIEWER/DataItemListModel.h \
    $$COMMONVIEWER/DownloadSampleManager.h \
    $$COMMONVIEWER/Sample.h \
    $$COMMONVIEWER/SampleCategory.h \
    $$COMMONVIEWER/SampleListModel.h \
    $$COMMONVIEWER/SampleManager.h \
    $$COMMONVIEWER/SampleManager_definitions.h \
    $$COMMONVIEWER/SampleSearchFilterModel.h \
    $$COMMONVIEWER/SearchFilterCriteria.h \
    $$COMMONVIEWER/SearchFilterSimpleKeywordCriteria.h \
    $$COMMONVIEWER/SourceCode.h \
    $$COMMONVIEWER/SourceCodeListModel.h

SOURCES += \
    CppSampleManager.cpp \
    $$COMMONVIEWER/SyntaxHighlighter/syntax_highlighter.cpp \
    $$COMMONVIEWER/SyntaxHighlighter/QMLHighlighter.cpp \
    $$COMMONVIEWER/CategoryListModel.cpp \
    $$COMMONVIEWER/DataItem.cpp \
    $$COMMONVIEWER/DataItemListModel.cpp \
    $$COMMONVIEWER/DownloadSampleManager.cpp \
    $$COMMONVIEWER/Sample.cpp \
    $$COMMONVIEWER/SampleCategory.cpp \
    $$COMMONVIEWER/SampleListModel.cpp \
    $$COMMONVIEWER/SampleManager.cpp \
    $$COMMONVIEWER/SampleSearchFilterModel.cpp \
    $$COMMONVIEWER/SearchFilterCriteria.cpp \
    $$COMMONVIEWER/SearchFilterSimpleKeywordCriteria.cpp \
    $$COMMONVIEWER/SourceCode.cpp \
    $$COMMONVIEWER/SourceCodeListModel.cpp \
    $$COMMONVIEWER/mainSample.cpp

RESOURCES += \
    $$COMMONVIEWER/qml/qml.qrc \
    $$COMMONVIEWER/images/images.qrc \
    $$COMMONVIEWER/resources/application.qrc \
    $$COMMONVIEWER/Samples/Categories.qrc \
    $$PWD/imports.qrc

DEFINES += QT_DEPRECATED_WARNINGS

#-------------------------------------------------
# Application Icon
#-------------------------------------------------

CONFIG(build_from_setup) {
  win32 {
      OTHER_FILES += \
          $$COMMONVIEWER/images/ArcGISRuntimeSDKQtSamples.ico
  }
} else {
  win32 {
      RC_FILE = ArcGISRuntimeSDKQt_CppSamples.rc

      OTHER_FILES += \
          $$COMMONVIEWER/images/ArcGISRuntimeSDKQtSamples.ico \
          ArcGISRuntimeSDKQt_CppSamples.rc
  }
}
mac {
    ICON = $$COMMONVIEWER/images/ArcGISRuntimeSDKQtSamples.icns

    OTHER_FILES += \
        $$COMMONVIEWER/ArcGISRuntimeSDKQtSamples.icns

    QMAKE_INFO_PLIST = $$PWD/mac/Info.plist
}

android {
    CONFIG(build_from_setup) {
        for (ABI, ANDROID_ABIS): {
            ANDROID_SSL_DIR = $$dirname(QMAKE_QMAKE)/../lib/ssl/$${ABI}
            LIST_SSL_LIBS = libcrypto_1_1.so libssl_1_1.so

            for(lib, LIST_SSL_LIBS) {
                exists($$ANDROID_SSL_DIR/$${lib}) {
                    ANDROID_EXTRA_LIBS += $$ANDROID_SSL_DIR/$${lib}
                }
                else {
                    error($$ANDROID_SSL_DIR is missing the $${lib} library which is required to build this Android application.)
                }
            }
        }
    }

    DISTFILES += \
        android/AndroidManifest.xml \
        android/res/values/libs.xml

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
}

ios {
    ios_icon.files = $$files($$PWD/iOS/Images.xcassets/AppIcon.appiconset/iOS_*.png)
    QMAKE_BUNDLE_DATA += ios_icon

    OTHER_FILES += \
        $$PWD/iOS/Info.plist \
        $$PWD/iOS/Images.xcassets/AppIcon.appiconset/Contents.json \
        $$PWD/iOS/Images.xcassets/LaunchImage.launchimage/Contents.json

    QMAKE_INFO_PLIST = $$PWD/iOS/Info.plist

    TARGET2 = $$replace(TARGET, "_", "-") # xcode replaces _ with - in provisioning profiles
    APP_PROVISIONED = $$system(if grep -sq $$TARGET2 ~/Library/MobileDevice/Provisioning\ Profiles/* ; then echo true ; fi)
    !equals(APP_PROVISIONED, "true") {
        XCODE_PROJ = $$OUT_PWD/$${TARGET}.xcodeproj
        exists($$XCODE_PROJ) {
            system(open -g -a Xcode --args $${XCODE_PROJ})
            system(for i in {1..90}; do if grep -sq $$TARGET2 ~/Library/MobileDevice/Provisioning\ Profiles/* ; then break; else sleep 1; fi ; done)
            system(osascript -e \'quit app \"Xcode\"\')
        }
    }
}

CONFIG(daily){
    CONFIG(release, debug | release){
        win32 {
            contains(QT_ARCH, i386) {
                message($${PWD}/../../../scripts/windows/deploy_windows.bat -bin $${DESTDIR} -dep ArcGISRuntimeSDKQt_CppSamples -exe ArcGISQt_CppSamples.exe -qt $${QMAKESPEC}/../.. -b x86 -toolkit -tk_qml)
                QMAKE_POST_LINK +=$$quote(cmd /c $${PWD}/../../../scripts/windows/deploy_windows.bat -bin $${DESTDIR} -dep ArcGISRuntimeSDKQt_CppSamples -exe ArcGISQt_CppSamples.exe -qt $${QMAKESPEC}/../.. -b x86 -toolkit -tk_qml $$escape_expand(\n\t))
            } else {
                message($${PWD}/../../../scripts/windows/deploy_windows.bat -bin $${DESTDIR} -dep ArcGISRuntimeSDKQt_QMLSamples -exe ArcGISQt_CppSamples.exe -qt $${QMAKESPEC}/../.. -b x64 -toolkit -tk_qml)
                QMAKE_POST_LINK +=$$quote(cmd /c $${PWD}/../../../scripts/windows/deploy_windows.bat -bin $${DESTDIR} -dep ArcGISRuntimeSDKQt_CppSamples -exe ArcGISQt_CppSamples.exe -qt $${QMAKESPEC}/../.. -b x64 -toolkit -tk_qml $$escape_expand(\n\t))
            }
        }
    }
}
