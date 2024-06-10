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

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/adler32_fold.h \
    $$PWD/adler32_p.h \
    $$PWD/chunkset_tpl.h \
    $$PWD/compare256_rle.h \
    $$PWD/cpu_features.h \
    $$PWD/crc32_braid_comb_p.h \
    $$PWD/crc32_braid_p.h \
    $$PWD/crc32_braid_tbl.h \
    $$PWD/crc32_fold.h \
    $$PWD/deflate.h \
    $$PWD/deflate_p.h \
    $$PWD/fallback_builtins.h \
    $$PWD/functable.h \
    $$PWD/inffast_tpl.h \
    $$PWD/inffixed_tbl.h \
    $$PWD/inflate.h \
    $$PWD/inflate_p.h \
    $$PWD/inftrees.h \
    $$PWD/insert_string_tpl.h \
    $$PWD/ioapi.h \
    $$PWD/match_tpl.h \
    $$PWD/mz.h \
    $$PWD/mz_compat.h \
    $$PWD/mz_crypt.h \
    $$PWD/mz_os.h \
    $$PWD/mz_strm.h \
    $$PWD/mz_strm_mem.h \
    $$PWD/mz_strm_os.h \
    $$PWD/mz_strm_zlib.h \
    $$PWD/mz_zip.h \
    $$PWD/trees.h \
    $$PWD/trees_emit.h \
    $$PWD/trees_tbl.h \
    $$PWD/unzip.h \
    $$PWD/zip.h \
    $$PWD/zbuild.h \
    $$PWD/zconf.h \
    $$PWD/zendian.h \
    $$PWD/zlib.h \
    $$PWD/zlib_name_mangling.h \
    $$PWD/zutil.h \
    $$PWD/zutil_p.h

SOURCES += \
    $$PWD/adler32.c \
    $$PWD/adler32_fold.c \
    $$PWD/chunkset.c \
    $$PWD/compare256.c \
    $$PWD/cpu_features.c \
    $$PWD/crc32_braid.c \
    $$PWD/crc32_braid_comb.c \
    $$PWD/crc32_fold.c \
    $$PWD/deflate.c \
    $$PWD/deflate_fast.c \
    $$PWD/deflate_huff.c \
    $$PWD/deflate_medium.c \
    $$PWD/deflate_quick.c \
    $$PWD/deflate_rle.c \
    $$PWD/deflate_slow.c \
    $$PWD/deflate_stored.c \
    $$PWD/functable.c \
    $$PWD/inflate.c \
    $$PWD/inftrees.c \
    $$PWD/insert_string.c \
    $$PWD/insert_string_roll.c \
    $$PWD/mz_compat.c \
    $$PWD/mz_crypt.c \
    $$PWD/mz_os.c \
    $$PWD/mz_strm.c \
    $$PWD/mz_strm_mem.c \
    $$PWD/mz_strm_zlib.c \
    $$PWD/mz_zip.c \
    $$PWD/slide_hash.c \
    $$PWD/trees.c \
    $$PWD/zutil.c

DEFINES += ZLIB_COMPAT HAVE_ZLIB MZ_ZIP_NO_ENCRYPTION MZ_ZIP_NO_CRYPTO

win32 {
    SOURCES += \
        $$PWD/mz_os_win32.c \
        $$PWD/mz_strm_os_win32.c

    DEFINES += RTC_WINDOWS_FAMILY
} else {
    SOURCES += \
        $$PWD/mz_os_posix.c \
        $$PWD/mz_strm_os_posix.c

    DEFINES += HAVE_ATTRIBUTE_ALIGNED
}
