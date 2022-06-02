#-------------------------------------------------
#  Copyright 2022 ESRI
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

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

ios_icon.files = $$files($$PWD/Images.xcassets/AppIcon.appiconset/Icon_*.png)
QMAKE_BUNDLE_DATA += ios_icon

OTHER_FILES +=     $$PWD/Info.plist     $$PWD/Images.xcassets/AppIcon.appiconset/Contents.json

QMAKE_INFO_PLIST = $$PWD/Info.plist

