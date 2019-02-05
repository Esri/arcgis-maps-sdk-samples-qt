// Copyright 2016 ESRI
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

#ifndef MYAPPLICATION_H
#define MYAPPLICATION_H

#include <QtGlobal>

#if defined(Q_OS_WIN) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
#include  <QtSingleApplication>
#else
#include <QApplication>
#endif

#if defined(Q_OS_WIN) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
class MyApplication : public QtSingleApplication
#else
class MyApplication : public QApplication
#endif
{
  Q_OBJECT

public:
  MyApplication(int& argc, char** argv, bool GUIenabled = true);
  MyApplication(const QString& id, int& argc, char** argv);
  ~MyApplication() override;

  bool event(QEvent* event) override;
};

#endif // MYAPPLICATION_H
