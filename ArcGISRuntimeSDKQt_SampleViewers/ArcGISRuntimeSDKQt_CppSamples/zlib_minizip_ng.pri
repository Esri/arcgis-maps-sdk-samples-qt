 # Copyright 2024 Esri.

 # Licensed under the Apache License, Version 2.0 (the "License");
 # you may not use this file except in compliance with the License.
 # You may obtain a copy of the License at
 # http://www.apache.org/licenses/LICENSE-2.0

 # Unless required by applicable law or agreed to in writing, software
 # distributed under the License is distributed on an "AS IS" BASIS,
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and
 # limitations under the License.


INCLUDEPATH += \
    $$PWD/zlib-ng \
    $$PWD/minizip-ng

HEADERS += \
    $$files($$PWD/zlib-ng/*.h)) \
    $$PWD/minizip-ng/zip.h \
    $$PWD/minizip-ng/unzip.h \
    $$PWD/minizip-ng/ioapi.h \
    $$PWD/ZipHelper.h

SOURCES += \
    $$files($$PWD/zlib-ng/*.c)) \
    $$PWD/minizip-ng/mz_compat.c \
    $$PWD/minizip-ng/mz_crypt.c \
    $$PWD/minizip-ng/mz_os.c \
    $$PWD/minizip-ng/mz_strm.c \
    $$PWD/minizip-ng/mz_strm_mem.c \
    $$PWD/minizip-ng/mz_strm_zlib.c \
    $$PWD/minizip-ng/mz_zip.c \
    $$PWD/ZipHelper.cpp

DEFINES += ZLIB_COMPAT
DEFINES += HAVE_ZLIB

win32 {
    SOURCES += \
        $$PWD/minizip-ng/mz_os_win32.c) \
        $$PWD/minizip-ng/mz_strm_os_win32.c)

    DEFINES += RTC_WINDOWS_FAMILY
}

!win32 {
    SOURCES += \
        $$PWD/minizip-ng/mz_os_posix.c \
        $$PWD/minizip-ng/mz_strm_os_posix.c

    DEFINES += HAVE_ATTRIBUTE_ALIGNED
}
