project "minizip-ng"

dofile(_BUILD_DIR .. "/static_library.lua")

configuration { "*" }

uuid "35C1536A-E063-477B-A3F1-0B8BB87900FD"

includedirs {
  ".",
  _3RDPARTY_DIR .. "/zlib-ng",
}

defines {
  "ZLIB_COMPAT",
  "HAVE_ZLIB",
  -- Support for PKWARE encryption was included by default in the prev minizip library, but this functionality is not used in rtc, so safe to turn off.
  --"HAVE_PKCRYPT"
  "MZ_ZIP_NO_ENCRYPTION",
  "MZ_ZIP_NO_CRYPTO"
}

files  {
  "mz_compat.c",
  "mz_crypt.c",
  "mz_os.c",
  "mz_strm.c",
  "mz_strm_mem.c",
  "mz_strm_zlib.c",
  "mz_zip.c",
}

if (_PLATFORM_ANDROID) then
  files {
    "mz_os_posix.c",
    "mz_strm_os_posix.c",
  }
end

if (_PLATFORM_IOS) then
  files {
    "mz_os_posix.c",
    "mz_strm_os_posix.c",
  }
end

if (_PLATFORM_LINUX) then
  files {
    "mz_os_posix.c",
    "mz_strm_os_posix.c",
  }
end

if (_PLATFORM_MACOS) then
  files {
    "mz_os_posix.c",
    "mz_strm_os_posix.c",
  }
end

if (_PLATFORM_WINDOWS) then
  files {
    "mz_os_win32.c",
    "mz_strm_os_win32.c",
  }
end

if (_PLATFORM_WINUWP) then
  files {
    "mz_os_win32.c",
    "mz_strm_os_win32.c",
  }
end
