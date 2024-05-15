project "zlib-ng"

dofile(_BUILD_DIR .. "/static_library.lua")

configuration { "*" }

uuid "76FC1997-C8C8-4D03-9853-A02FEBFC0043"

includedirs {
  ".",
}

defines {
    "ZLIB_COMPAT",
    -- Support for gzfileops was included by default in the prev zlib library, but this functionality is not used in rtc, so safe to turn off.
    --"WITH_GZFILEOP"
}

files  {
    "adler32.c",
    "adler32_fold.c",
    "chunkset.c",
    "compare256.c",
    "cpu_features.c",
    "compress.c",
    "crc32_fold.c",
    "crc32_braid.c",
    "deflate.c",
    "deflate_fast.c",
    "deflate_huff.c",
    "deflate_medium.c",
    "deflate_quick.c",
    "deflate_rle.c",
    "deflate_slow.c",
    "deflate_stored.c",
    "functable.c",
--  "gzlib.c",
--  "gzread.c",
--  "gzwrite.c",
--  "infback.c",
--  "inffast.c",
    "inflate.c",
    "inftrees.c",
    "insert_string.c",
    "insert_string_roll.c",
    "slide_hash.c",
    "trees.c",
    "uncompr.c",
    "zutil.c",
}

if (_PLATFORM_ANDROID) then
  defines {
    "HAVE_ATTRIBUTE_ALIGNED"
  }
end

if (_PLATFORM_IOS) then
  defines {
    "HAVE_ATTRIBUTE_ALIGNED"
  }
end

if (_PLATFORM_LINUX) then
  defines {
    "HAVE_ATTRIBUTE_ALIGNED"
  }
end

if (_PLATFORM_MACOS) then
  defines {
    "HAVE_ATTRIBUTE_ALIGNED"
  }
end

if (_PLATFORM_WINDOWS) then
end

if (_PLATFORM_WINUWP) then
end
