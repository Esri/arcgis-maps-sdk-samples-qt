// [WriteFile Name=OpenMobileScenePackage, Category=Scenes]
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

#include "OpenMobileScenePackage.h"

#include "Error.h"
#include "MobileScenePackage.h"
#include "Scene.h"
#include "SceneQuickView.h"

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
} // namespace

OpenMobileScenePackage::OpenMobileScenePackage(QObject* parent /* = nullptr */):
  QObject(parent)
{  
  // create the MSPK data path
  // data is downloaded automatically by the sample viewer app. Instructions to download
  // separately are specified in the readme.
  const QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/mspk/philadelphia.mspk";

  // connect to the Mobile Scene Package instance to know when errors occur
  connect(MobileScenePackage::instance(), &MobileScenePackage::errorOccurred,
          [](Error e)
  {
    if (e.isEmpty())
      return;

    qDebug() << QString("Error: %1 %2").arg(e.message(), e.additionalMessage());
  });

  // Create the Scene Package
  createScenePackage(dataPath);
}

OpenMobileScenePackage::~OpenMobileScenePackage() = default;

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

  // set the scene on the scene view to display
  if (m_scene && m_sceneView)
    m_sceneView->setArcGISScene(m_scene);

  emit sceneViewChanged();
}

// Slot for handling when the package loads
void OpenMobileScenePackage::packageLoaded(const Error& e)
{
  if (!e.isEmpty())
  {
    qDebug() << QString("Package load error: %1 %2").arg(e.message(), e.additionalMessage());
    return;
  }

  if (m_scenePackage->scenes().isEmpty())
    return;

  // The package contains a list of scenes that could be show in a UI for selection.
  // For simplicity, obtain the first scene in the list of scenes
  m_scene = m_scenePackage->scenes().at(0);

  // set the scene on the scene view to display
  if (m_scene && m_sceneView)
    m_sceneView->setArcGISScene(m_scene);
}

// create scene package and connect to signals
void OpenMobileScenePackage::createScenePackage(const QString& path)
{
  m_scenePackage = new MobileScenePackage(path, this);
  connect(m_scenePackage, &MobileScenePackage::doneLoading, this, &OpenMobileScenePackage::packageLoaded);
  m_scenePackage->load();
}
