// Copyright 2020 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
