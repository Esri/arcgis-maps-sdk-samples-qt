// Copyright 2015 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ManageBookmarks.h"
#include "ArcGISRuntimeEnvironment.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
  // Enforce OpenGL
  qputenv("QSG_RHI_BACKEND", "opengl");

  QApplication application(argc, argv);

  // Use of Esri location services, including basemaps and geocoding,
  // requires authentication using either an ArcGIS identity or an API Key.
  // 1. ArcGIS identity: An ArcGIS named user account that is a member of an
  //    organization in ArcGIS Online or ArcGIS Enterprise.
  // 2. API key: A permanent key that gives your application access to Esri
  //    location services. Visit your ArcGIS Developers Dashboard create a new
  //    API keys or access an existing API key.
  const QString apiKey = QString("");
  if (apiKey.isEmpty())
  {
      qWarning() << "Use of Esri location services, including basemaps, requires" <<
                    "you to authenticate with an ArcGIS identity or set the API Key property.";
  }
  else
  {
      Esri::ArcGISRuntime::ArcGISRuntimeEnvironment::setApiKey(apiKey);
  }

  ManageBookmarks applicationWindow;
  applicationWindow.setMinimumWidth(800);
  applicationWindow.setMinimumHeight(600);
  applicationWindow.show();

  return application.exec();
}
