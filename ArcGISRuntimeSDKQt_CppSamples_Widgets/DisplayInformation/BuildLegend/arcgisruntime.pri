#-------------------------------------------------
#  Copyright 2018 ESRI
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

contains(QMAKE_HOST.os, Windows):{
  iniPath = $$(ALLUSERSPROFILE)\EsriRuntimeQt\ArcGIS Runtime SDK for Qt $${ARCGIS_RUNTIME_VERSION}.ini
}
else {
  userHome = $$system(echo $HOME)
  iniPath = $${userHome}/.config/EsriRuntimeQt/ArcGIS Runtime SDK for Qt $${ARCGIS_RUNTIME_VERSION}.ini
}
iniLine = $$cat($${iniPath}, "lines")
dirPath = $$find(iniLine, "InstallDir")
cleanDirPath = $$replace(dirPath, "InstallDir=", "")
priLocation = $$replace(cleanDirPath, '"', "")
!include($$priLocation/sdk/ideintegration/esri_runtime_qt.pri) {
  message("Error. Cannot locate ArcGIS Runtime PRI file")
}
