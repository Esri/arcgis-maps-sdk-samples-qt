ECHO OFF

SET "ARCH=x64"
SET "VC_ARCH=amd64"
SET "QT_KIT=C:\Qt\5.6\msvc2015_64"
SET "DIR=%~dp0"
SET "BUILD_TYPE=release"
SET "BUILD_DIR=%DIR%..\build-OAuthRedirectExample-Desktop_Qt_5_6_2_MSVC2015_64bit-Release\%BUILD_TYPE%"
SET "ARCGIS_SDK_DIR=C:\Program Files (x86)\ArcGIS SDKs\Qt100.5\sdk\windows\%ARCH%\bin\%BUILD_TYPE%"

call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" %VC_ARCH%
call %QT_KIT%\bin\windeployqt.exe %BUILD_DIR%\OAuthRedirectExample.exe --qmldir %DIR% -sensors -positioning

copy "%ARCGIS_SDK_DIR%\EsriCommonQt.dll" "%BUILD_DIR%\EsriCommonQt.dll"
copy "%ARCGIS_SDK_DIR%\runtimecore.dll" "%BUILD_DIR%\runtimecore.dll"
