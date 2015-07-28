// Copyright 2015 ESRI
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

#include "MapViewViewpoint.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
  QApplication application(argc, argv);

  MapViewViewpoint applicationWindow;
  applicationWindow.setMinimumWidth(800);
  applicationWindow.setMinimumHeight(600);
  applicationWindow.show();

  return application.exec();
}
