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

#ifndef PERMISSIONSHELPER_H
#define PERMISSIONSHELPER_H

#include <QObject>

class PermissionsHelper : public QObject
{  
  Q_OBJECT

  Q_PROPERTY(bool fileSystemAccessGranted READ fileSystemAccessGranted NOTIFY fileSystemAccessGrantedChanged)

public:
  explicit PermissionsHelper(QObject* parent = nullptr);
  ~PermissionsHelper();

  Q_INVOKABLE void requestFilesystemAccess();

  bool fileSystemAccessGranted() const;

signals:
  void fileSystemAccessGrantedChanged();
  void requestFilesystemAccessCompleted(); // notifies even if request denied
};

#endif // PERMISSIONSHELPER_H
