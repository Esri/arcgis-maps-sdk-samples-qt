# Install script for directory: /Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode_15.0.0.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/build/libminizip.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libminizip.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libminizip.a")
    execute_process(COMMAND "/Applications/Xcode_15.0.0.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libminizip.a")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/minizip/minizip.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/minizip/minizip.cmake"
         "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/build/CMakeFiles/Export/4fe77da0f4d2c94dd906efce3aa1c0aa/minizip.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/minizip/minizip-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/minizip/minizip.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/minizip" TYPE FILE FILES "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/build/CMakeFiles/Export/4fe77da0f4d2c94dd906efce3aa1c0aa/minizip.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/minizip" TYPE FILE FILES "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/build/CMakeFiles/Export/4fe77da0f4d2c94dd906efce3aa1c0aa/minizip-noconfig.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/minizip" TYPE FILE FILES
    "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/build/minizip-config-version.cmake"
    "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/build/minizip-config.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/minizip" TYPE FILE FILES
    "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/mz.h"
    "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/mz_os.h"
    "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/mz_crypt.h"
    "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/mz_strm.h"
    "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/mz_strm_buf.h"
    "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/mz_strm_mem.h"
    "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/mz_strm_split.h"
    "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/mz_strm_os.h"
    "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/mz_zip.h"
    "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/mz_zip_rw.h"
    "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/mz_strm_zlib.h"
    "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/mz_compat.h"
    "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/build/ioapi.h"
    "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/build/zip.h"
    "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/build/unzip.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/build/minizip.pc")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/Users/jare8800/applications/qt/sdk/samples/ArcGISRuntimeSDKQt_SampleViewers/ArcGISRuntimeSDKQt_CppSamples/minizip-ng/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
