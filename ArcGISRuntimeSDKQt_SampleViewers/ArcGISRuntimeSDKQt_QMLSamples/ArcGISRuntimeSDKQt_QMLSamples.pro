# Copyright 2017 ESRI
#
# All rights reserved under the copyright laws of the United States
# and applicable international laws, treaties, and conventions.
#
# You may freely redistribute and use this sample code, with or
# without modification, provided you include the original copyright
# notice and use restrictions.
#
# See the Sample code usage restrictions document for further information.

TEMPLATE = app
QT += core gui xml network qml quick positioning sensors multimedia \
      widgets quickcontrols2 opengl webview texttospeech
TARGET = ArcGISQt_QMLSamples
DEFINES += QML_VIEWER
DEFINES += Qt_Version=\"$$QT_VERSION\"
SAMPLEPATHQML = $$PWD/../../ArcGISRuntimeSDKQt_QMLSamples
COMMONVIEWER = $$PWD/../ArcGISRuntimeSDKQt_Samples
ARCGIS_RUNTIME_VERSION = 100.15
DEFINES += ArcGIS_Runtime_Version=$$ARCGIS_RUNTIME_VERSION

# This block determines whether to build against the SDK dev build area or the installed SDK
exists($$PWD/../../../../DevBuildQml.pri) {
  message("Building against the dev environment")
  DEFINES += ESRI_BUILD
  DEFINES += SAMPLE_VIEWER_API_KEY=$$(SAMPLEVIEWERAPIKEY_INTERNAL)
  DEFINES += GANALYTICS_API_KEY=$$(GANALYTICS_API_KEY)
  DEFINES += GANALYTICS_STREAM_ID=$$(GANALYTICS_STREAM_ID)

  # use the Esri dev build script
  include ($$PWD/../../../../DevBuildQml.pri)
  # include the toolkitqml.pri, which contains all the toolkit resources
  include($$PWD/../../../toolkit/uitools/toolkitqml.pri)
} else {
  message("Building against the installed SDK")
  CONFIG+=build_from_setup
  CONFIG += c++14

  # include the toolkitqml.pri, which contains all the toolkit resources
  !include($$PWD/../../arcgis-runtime-toolkit-qt/uitools/toolkitqml.pri) {
    message("ERROR: Cannot find toolkitqml.pri at path:" $$PWD/../../arcgis-runtime-toolkit-qt/uitools/toolkitqml.pri)
    message("Please ensure the Qt Toolkit repository is cloned and the path above is correct.")
  }

  contains(QMAKE_HOST.os, Windows):{
    iniPath = $$(ALLUSERSPROFILE)\\EsriRuntimeQt\\ArcGIS Runtime SDK for Qt $${ARCGIS_RUNTIME_VERSION}.ini
  }
  else {
    userHome = $$system(echo $HOME)
    iniPath = $${userHome}/.config/EsriRuntimeQt/ArcGIS Runtime SDK for Qt $${ARCGIS_RUNTIME_VERSION}.ini
  }
  iniLine = $$cat($${iniPath}, "lines")
  dirPath = $$find(iniLine, "InstallDir")
  cleanDirPath = $$replace(dirPath, "InstallDir=", "")
  priLocation = $$replace(cleanDirPath, '"', "")
  !include($$priLocation/sdk/ideintegration/arcgis_runtime_qml.pri) {
    message("Error. Cannot locate ArcGIS Runtime PRI file")
  }

  DEFINES += BUILD_FROM_SETUP
}

qtHaveModule(webengine) {
  QT += webengine
  DEFINES += QT_WEBVIEW_WEBENGINE_BACKEND
}

QMAKE_TARGET_COMPANY = Esri, Inc.
QMAKE_TARGET_PRODUCT = ArcGIS Runtime SDK for Qt Samples - QML
QMAKE_TARGET_DESCRIPTION = ArcGIS Runtime SDK for Qt Samples - QML
QMAKE_TARGET_COPYRIGHT = Copyright 2017 Esri Inc.


# include the samples.pri, which contains all the sample resources
include(samples.pri)

PRECOMPILED_HEADER = $$COMMONVIEWER/pch.hpp
CONFIG += precompile_header
DEFINES += PCH_BUILD

android {
  QMAKE_CXXFLAGS_USE_PRECOMPILE = -include-pch \${QMAKE_PCH_OUTPUT}
  PRECOMPILED_DIR = pch
}

INCLUDEPATH += \
    $$COMMONVIEWER \
    $$COMMONVIEWER/SyntaxHighlighter

HEADERS += \
    QmlSampleManager.h \
    $$COMMONVIEWER/pch.hpp \
    $$COMMONVIEWER/SyntaxHighlighter/syntax_highlighter.h \
    $$COMMONVIEWER/SyntaxHighlighter/QMLHighlighter.h \
    $$COMMONVIEWER/CategoryListModel.h \
    $$COMMONVIEWER/DataItem.h \
    $$COMMONVIEWER/DataItemListModel.h \
    $$COMMONVIEWER/DownloadSampleManager.h \
    $$COMMONVIEWER/GAnalytics.h \
    $$COMMONVIEWER/Sample.h \
    $$COMMONVIEWER/SampleCategory.h \
    $$COMMONVIEWER/SampleListModel.h \
    $$COMMONVIEWER/SampleManager.h \
    $$COMMONVIEWER/SampleManager_definitions.h \
    $$COMMONVIEWER/SampleSearchFilterModel.h \
    $$COMMONVIEWER/SearchFilterCriteria.h \
    $$COMMONVIEWER/SearchFilterKeywordCriteria.h \
    $$COMMONVIEWER/SearchFilterSimpleKeywordCriteria.h \
    $$COMMONVIEWER/SourceCode.h \
    $$COMMONVIEWER/SourceCodeListModel.h \
    $$COMMONVIEWER/PermissionsHelper.h

SOURCES += \
    QmlSampleManager.cpp \
    $$COMMONVIEWER/SyntaxHighlighter/syntax_highlighter.cpp \
    $$COMMONVIEWER/SyntaxHighlighter/QMLHighlighter.cpp \
    $$COMMONVIEWER/CategoryListModel.cpp \
    $$COMMONVIEWER/DataItem.cpp \
    $$COMMONVIEWER/DataItemListModel.cpp \
    $$COMMONVIEWER/DownloadSampleManager.cpp \
    $$COMMONVIEWER/GAnalytics.cpp \
    $$COMMONVIEWER/Sample.cpp \
    $$COMMONVIEWER/SampleCategory.cpp \
    $$COMMONVIEWER/SampleListModel.cpp \
    $$COMMONVIEWER/SampleManager.cpp \
    $$COMMONVIEWER/SampleSearchFilterModel.cpp \
    $$COMMONVIEWER/SearchFilterCriteria.cpp \
    $$COMMONVIEWER/SearchFilterKeywordCriteria.cpp \
    $$COMMONVIEWER/SearchFilterSimpleKeywordCriteria.cpp \
    $$COMMONVIEWER/SourceCode.cpp \
    $$COMMONVIEWER/SourceCodeListModel.cpp \
    $$COMMONVIEWER/mainSample.cpp \
    $$COMMONVIEWER/PermissionsHelper.cpp

RESOURCES += \
    imports.qrc \
    qml/qmlsamples.qrc \
    $$COMMONVIEWER/qml/qml.qrc \
    $$COMMONVIEWER/images/images.qrc \
    $$COMMONVIEWER/resources/application.qrc \
    $$COMMONVIEWER/Samples/Categories.qrc

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
      RC_FILE = ArcGISRuntimeSDKQt_QMLSamples.rc

      OTHER_FILES += \
          $$COMMONVIEWER/images/ArcGISRuntimeSDKQtSamples.ico \
          ArcGISRuntimeSDKQt_QMLSamples.rc
  }
}

mac {
    ICON = $$COMMONVIEWER/images/ArcGISRuntimeSDKQtSamples.icns

    OTHER_FILES += \
        $$COMMONVIEWER/ArcGISRuntimeSDKQtSamples.icns

    QMAKE_INFO_PLIST = $$PWD/mac/Info.plist
}

android {
    QT += androidextras

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
                message($${PWD}/../../scripts/windows/deploy_windows.bat -bin $${DESTDIR} -dep ArcGISRuntimeSDKQt_QMLSamples -exe ArcGISQt_QMLSamples.exe -qt $${QMAKESPEC}/../.. -b x86 -toolkit -tk_qml)
                QMAKE_POST_LINK +=$$quote(cmd /c $${PWD}/../../scripts/windows/deploy_windows.bat -bin $${DESTDIR} -dep ArcGISRuntimeSDKQt_QMLSamples -exe ArcGISQt_QMLSamples.exe -qt $${QMAKESPEC}/../.. -b x86 -toolkit -tk_qml $$escape_expand(\n\t))
            } else {
                message($${PWD}/../../scripts/windows/deploy_windows.bat -bin $${DESTDIR} -dep ArcGISRuntimeSDKQt_QMLSamples -exe ArcGISQt_QMLSamples.exe -qt $${QMAKESPEC}/../.. -b x64 -toolkit -tk_qml)
                QMAKE_POST_LINK +=$$quote(cmd /c $${PWD}/../../scripts/windows/deploy_windows.bat -bin $${DESTDIR} -dep ArcGISRuntimeSDKQt_QMLSamples -exe ArcGISQt_QMLSamples.exe -qt $${QMAKESPEC}/../.. -b x64 -toolkit -tk_qml $$escape_expand(\n\t))
            }
        }
    }
}
