// [WriteFile Name=OpenMobileMap_MapPackage, Category=Maps]
// [Legal]
// Copyright 2016 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "OpenMobileMap_MapPackage.h"

#include "Basemap.h"
#include "Error.h"
#include "MapQuickView.h"
#include "MobileMapPackage.h"

#include <QDir>
#include <QtCore/qglobal.h>

#ifdef Q_OS_IOS
#include <QStandardPaths>
#endif // Q_OS_IOS

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
QString defaultDataPath()
{
  QString dataPath;

#ifdef Q_OS_ANDROID
  dataPath = "/sdcard";
#elif defined Q_OS_IOS
  dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
  dataPath = QDir::homePath();
#endif

  return dataPath;
}

// sample MMPK location
const QString sampleFileYellowstone {"/ArcGIS/Runtime/Data/mmpk/Yellowstone.mmpk"};

} // namespace

OpenMobileMap_MapPackage::OpenMobileMap_MapPackage(QQuickItem* parent) :
  QQuickItem(parent)
{    
  // connect to the Mobile Map Package instance to know when errors occur
  connect(MobileMapPackage::instance(), &MobileMapPackage::errorOccurred,
          [](Error e)
  {
    if (e.isEmpty())
    {
      return;
    }

    qDebug() << QString("Error: %1 %2").arg(e.message(), e.additionalMessage());
  });
}

OpenMobileMap_MapPackage::~OpenMobileMap_MapPackage() = default;

void OpenMobileMap_MapPackage::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<OpenMobileMap_MapPackage>("Esri.Samples", 1, 0, "OpenMobileMap_MapPackageSample");
}

void OpenMobileMap_MapPackage::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // create the MMPK data path
  // data is downloaded automatically by the sample viewer app. Instructions to download
  // separately are specified in the readme.
  const QString dataPath = defaultDataPath() + sampleFileYellowstone;

  // Load the MMPK
  createMapPackage(dataPath);
}

// create map package
void OpenMobileMap_MapPackage::createMapPackage(const QString& path)
{
  //! [open mobile map package cpp snippet]
  // instatiate a mobile map package
  m_mobileMapPackage = new MobileMapPackage(path, this);

  // wait for the mobile map package to load
  connect(m_mobileMapPackage, &MobileMapPackage::doneLoading, this, [this](Error error)
  {
    if (!error.isEmpty())
    {
      qDebug() << QString("Package load error: %1 %2").arg(error.message(), error.additionalMessage());
      return;
    }

    if (!m_mobileMapPackage || !m_mapView || m_mobileMapPackage->maps().isEmpty())
    {
      return;
    }

    // The package contains a list of maps that could be shown in the UI for selection.
    // For simplicity, obtain the first map in the list of maps.
    // set the map on the map view to display
    m_mapView->setMap(m_mobileMapPackage->maps().at(0));

  });

  m_mobileMapPackage->load();
  //! [open mobile map package cpp snippet]
}
