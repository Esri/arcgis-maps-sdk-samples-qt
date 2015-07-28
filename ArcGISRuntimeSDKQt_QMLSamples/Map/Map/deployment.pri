# Copyright 2015 ESRI
#
# All rights reserved under the copyright laws of the United States
# and applicable international laws, treaties, and conventions.
#
# You may freely redistribute and use this sample code, with or
# without modification, provided you include the original copyright
# notice and use restrictions.
#
# See the Sample code usage restrictions document for further information.
#

android-no-sdk {
    target.path = /data/user/qt
    export(target.path)
    INSTALLS += target
} else:android {
    x86 {
        target.path = /libs/x86
    } else: armeabi-v7a {
        target.path = /libs/armeabi-v7a
    } else {
        target.path = /libs/armeabi
    }
    export(target.path)
    INSTALLS += target
} else:unix {
    isEmpty(target.path) {
        qnx {
            target.path = /tmp/$${TARGET}/bin
        } else {
            target.path = /opt/$${TARGET}/bin
        }
        export(target.path)
    }
    INSTALLS += target
}

export(INSTALLS)
