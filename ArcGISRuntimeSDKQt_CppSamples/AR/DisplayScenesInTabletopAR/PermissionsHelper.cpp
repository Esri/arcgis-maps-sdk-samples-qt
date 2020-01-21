// Copyright 2019 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#include "PermissionsHelper.h"

#include <QtGlobal>

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <algorithm>
namespace {
  constexpr const char* s_android_fs_read_permission = "android.permission.READ_EXTERNAL_STORAGE";
  constexpr const char* s_android_fs_write_permission = "android.permission.WRITE_EXTERNAL_STORAGE";
} // anonymous namespace
#endif

PermissionsHelper::PermissionsHelper(QObject* parent) :
  QObject(parent)
{
}

PermissionsHelper::~PermissionsHelper() = default;

bool PermissionsHelper::fileSystemAccessGranted() const
{
#ifdef Q_OS_ANDROID
  return QtAndroid::checkPermission(s_android_fs_read_permission) == QtAndroid::PermissionResult::Granted &&
         QtAndroid::checkPermission(s_android_fs_write_permission) == QtAndroid::PermissionResult::Granted;
#else
  return true;
#endif
}

void PermissionsHelper::requestFilesystemAccess()
{
#ifdef Q_OS_ANDROID
  if (!fileSystemAccessGranted())
  {
    const QtAndroid::PermissionResultCallback callback = [this](const QtAndroid::PermissionResultMap& resultsMap)
    {
      const bool anyDenied = std::any_of(resultsMap.cbegin(), resultsMap.cend(),
      [](QtAndroid::PermissionResult result)
      {
        return result == QtAndroid::PermissionResult::Denied;
      });

      if (!anyDenied)
        emit fileSystemAccessGrantedChanged();

      emit requestFilesystemAccessCompleted();
    };

    QtAndroid::requestPermissions({s_android_fs_read_permission, s_android_fs_write_permission}, callback);
  }
  else
  {
    emit requestFilesystemAccessCompleted();
  }
#else
  emit requestFilesystemAccessCompleted();
#endif
}
