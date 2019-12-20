// [WriteFile Name=HonorMobileMapPackageExpiration, Category=Maps]
// [Legal]
// Copyright 2019 Esri.

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

#include "HonorMobileMapPackageExpiration.h"

#include "MobileMapPackage.h"
#include "MapQuickView.h"

#include <QtGlobal>
#include <QDir>

#ifdef Q_OS_IOS
#include <QStandardPaths>
#endif

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace {
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
}

// sample MMPK location
const QString sampleMmpk {"/ArcGIS/Runtime/Data/mmpk/LothianRiversAnno.mmpk"};

HonorMobileMapPackageExpiration::HonorMobileMapPackageExpiration(QObject* parent /* = nullptr */):
  QObject(parent),
  m_dataPath(defaultDataPath() + sampleMmpk)
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

HonorMobileMapPackageExpiration::~HonorMobileMapPackageExpiration() = default;

void HonorMobileMapPackageExpiration::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<HonorMobileMapPackageExpiration>("Esri.Samples", 1, 0, "HonorMobileMapPackageExpirationSample");
}

MapQuickView* HonorMobileMapPackageExpiration::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void HonorMobileMapPackageExpiration::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;

  emit mapViewChanged();

  createMapPackage(m_dataPath);
}

void HonorMobileMapPackageExpiration::createMapPackage(const QString& path)
{
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

    // Access Expiration Info
    Expiration expiration = m_mobileMapPackage->expiration();
    if (!expiration.isEmpty())
    {
      if (expiration.isExpired())
      {
        const QString message = expiration.message();
        const QString date = expiration.dateTime().toString(Qt::ISODate).split("T")[0];
        const QString time = expiration.dateTime().time().toString();
        m_expirationString = QString("%1\nExpired on %2 at %3 UTC").arg(message, date, time);
        emit expirationStringChanged();

        // return if access after expiration is not allowed
        if (expiration.type() == ExpirationType::PreventExpiredAccess)
          return;
      }
    }

    // set the map view's map to the first map in the mobile map package
    m_mapView->setMap(m_mobileMapPackage->maps().at(0));
  });

  m_mobileMapPackage->load();
}
