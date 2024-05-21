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
    $$files(zlib-ng/*.h) \
    $$files(minizip-ng/zip.h) \
    $$files(minizip-ng/unzip.h) \
    $$files(minizip-ng/ioapi.h) \
    ZipHelper.h

SOURCES += \
    $$files(zlib-ng/*.c) \
    $$files(minizip-ng/mz_compat.c) \
    $$files(minizip-ng/mz_crypt.c) \
    $$files(minizip-ng/mz_os.c) \
    $$files(minizip-ng/mz_strm.c) \
    $$files(minizip-ng/mz_strm_mem.c) \
    $$files(minizip-ng/mz_strm_zlib.c) \
    $$files(minizip-ng/mz_zip.c) \
    ZipHelper.cpp

DEFINES += ZLIB_COMPAT
DEFINES += HAVE_ZLIB

win32 {
    SOURCES += \
        $$files(minizip-ng/mz_os_win32.c) \
        $$files(minizip-ng/mz_strm_os_win32.c)

    DEFINES += RTC_WINDOWS_FAMILY
}

!win32 {
    SOURCES += \
        $$files(minizip-ng/mz_os_posix.c) \
        $$files(minizip-ng/mz_strm_os_posix.c)

    DEFINES += HAVE_ATTRIBUTE_ALIGNED
}
