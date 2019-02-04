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
#include <QDateTime>
#include <QtCore/qglobal.h>

#ifdef Q_OS_IOS
#include <QStandardPaths>
#endif // Q_OS_IOS

using namespace Esri::ArcGISRuntime;

OpenMobileScenePackage::OpenMobileScenePackage(QObject* parent /* = nullptr */):
  QObject(parent)
{  
}

OpenMobileScenePackage::~OpenMobileScenePackage() = default;

// helper method to get cross platform data path
namespace {
  QString defaultHomePath()
  {
    QString homePath;

  #ifdef Q_OS_ANDROID
    homePath = "/sdcard";
  #elif defined Q_OS_IOS
    homePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  #else
    homePath = QDir::homePath();
  #endif

    return homePath;
  }
}

void OpenMobileScenePackage::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<OpenMobileScenePackage>("Esri.Samples", 1, 0, "OpenMobileScenePackageSample");
}

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

  openPackage();

  emit sceneViewChanged();
}

// Open a mobile scene package
void OpenMobileScenePackage::openPackage()
{
  // create the MSPK data path
  const QString dataPath = defaultHomePath() + "/ArcGIS/Runtime/Data/mspk/philadelphia.mspk";

  // connect to the Mobile Scene Package to determine if direct read is supported
  connect(MobileScenePackage::instance(), &MobileScenePackage::isDirectReadSupportedCompleted,
          this, [this, dataPath](QUuid, bool supported)
  {
    // if direct read is supported, load the package
    if (supported)
    {
      createScenePackage(dataPath);
    }
    // otherwise, the package needs to be unpacked
    else
    {
      m_unpackTempPath = QDir::tempPath() + QString("/MspkCpp_%1.mspk").arg(QDateTime().currentMSecsSinceEpoch());
      MobileScenePackage::unpack(dataPath, m_unpackTempPath);
    }
  });

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

  // Check if the MSPK can be read directly
  MobileScenePackage::isDirectReadSupported(dataPath);
}

// Slot for handling when the package loads
void OpenMobileScenePackage::packageLoaded(Error e)
{
  if (!e.isEmpty())
  {
    qDebug() << QString("Package load error: %1 %2").arg(e.message(), e.additionalMessage());
    return;
  }

  // obtain the first scene in the list of scenes
  Scene* scene = m_scenePackage->scenes().at(0);

  // set the scene on the scene view to display
  m_sceneView->setArcGISScene(scene);
}

// create scene package and connect to signals
void OpenMobileScenePackage::createScenePackage(const QString& path)
{
  m_scenePackage = new MobileScenePackage(path, this);
  connect(m_scenePackage, &MobileScenePackage::doneLoading, this, &OpenMobileScenePackage::packageLoaded);
  m_scenePackage->load();
}
