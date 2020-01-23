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
