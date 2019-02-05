// [WriteFile Name=OpenMobileScenePackage, Category=Scenes]
// [Legal]
// Copyright 2018 Esri.

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

#include "OpenMobileScenePackage.h"

#include "MobileScenePackage.h"
#include "Scene.h"
#include "SceneQuickView.h"

#include <QDir>
#include <QTemporaryDir>
#include <QtCore/qglobal.h>

#ifdef Q_OS_IOS
#include <QStandardPaths>
#endif // Q_OS_IOS

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

void OpenMobileScenePackage::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<OpenMobileScenePackage>("Esri.Samples", 1, 0, "OpenMobileScenePackageSample");
}

OpenMobileScenePackage::OpenMobileScenePackage(QObject* parent /* = nullptr */):
  QObject(parent)
{
  // create the MSPK data path
  const QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/mspk/test_mspk_1.mspk";
  createScenePackage(dataPath);
}

OpenMobileScenePackage::~OpenMobileScenePackage() = default;

SceneQuickView* OpenMobileScenePackage::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void OpenMobileScenePackage::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
  {
    return;
  }

  m_sceneView = sceneView;

  setSceneToView();

  emit sceneViewChanged();
}

// Slot for handling when the package loads
void OpenMobileScenePackage::packageLoaded(Error e)
{
  if (!e.isEmpty())
  {
    // For a full list of error codes, see https://developers.arcgis.com/qt/latest/cpp/guide/platform-error-codes.htm
    constexpr int packageExpiredCode = 26;
    constexpr int requiresUnpackCode = 7012;

    const int errorCode = e.code();

    // If a mobile scene package contains a raster file it must be unpacked to a new directory before loading
    if (errorCode == requiresUnpackCode)
    {
      unpackScenePackage();
    }
    // If a mobile scene package is authored to prevent loading after the expiration date, report to the user
    else if (errorCode == packageExpiredCode && !m_scenePackage->expiration().isEmpty())
    {
      qDebug() << QString("Package Expired: %1").arg(m_scenePackage->expiration().message());
    }
    else
    {
      qDebug() << QString("Package load error: %1 %2").arg(e.message(), e.additionalMessage());
    }

    return;
  }

  if (m_scenePackage->scenes().isEmpty())
    return;

  // obtain the first scene in the list of scenes
  m_scene = m_scenePackage->scenes().at(0);

  setSceneToView();
}

// create scene package and connect to signals
void OpenMobileScenePackage::createScenePackage(const QString& path)
{
  if (m_scenePackage)
  {
    delete m_scenePackage;
    m_scenePackage = nullptr;
  }

  m_scenePackage = new MobileScenePackage(path, this);
  connect(m_scenePackage, &MobileScenePackage::doneLoading, this, &OpenMobileScenePackage::packageLoaded);
  m_scenePackage->load();
}

void OpenMobileScenePackage::unpackScenePackage()
{
  if (!m_scenePackage)
  {
    qDebug() << "Invalid scene package to unpack.";
    return;
  }

  if (!m_unpackTempPath.isEmpty())
  {
    // Only perform 1 unpack operation
    qDebug() << "Already unpacked.";
    return;
  }

  // connect to the Mobile Scene Package to know when the data is unpacked
  connect(MobileScenePackage::instance(), &MobileScenePackage::unpackCompleted,
          this, [this](QUuid, bool success)
  {
    // if the unpack was successful, load the unpacked package
    if (success)
    {
      createScenePackage(m_unpackTempPath);
    }
    // log that the upack failed
    else
    {
      qDebug() << "failed to unpack";
    }
  });

  m_unpackTempPath = QTemporaryDir().path();
  MobileScenePackage::unpack(m_scenePackage->path(), m_unpackTempPath);
}

void OpenMobileScenePackage::setSceneToView()
{
  if (!m_scene || !m_sceneView)
    return;

  // set the scene on the scene view to display
  m_sceneView->setArcGISScene(m_scene);
}
