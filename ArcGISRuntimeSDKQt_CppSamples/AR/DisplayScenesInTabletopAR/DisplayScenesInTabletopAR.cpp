// [WriteFile Name=DisplayScenesInTabletopAR, Category=AR]
// [Legal]
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
// [Legal]

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "DisplayScenesInTabletopAR.h"

#include "ArcGISSceneLayer.h"
#include "ArcGISTiledElevationSource.h"
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

using namespace Esri::ArcGISRuntime;

using namespace Esri::ArcGISRuntime::Toolkit;

ArcGISArView* DisplayScenesInTabletopAR::arcGISArView() const
{
  return m_arcGISArView;
}

void DisplayScenesInTabletopAR::setArcGISArView(ArcGISArView* arcGISArView)
{
  if (!arcGISArView || arcGISArView == m_arcGISArView)
    return;

  m_arcGISArView = arcGISArView;
  emit arcGISArViewChanged();
}

DisplayScenesInTabletopAR::DisplayScenesInTabletopAR(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(SceneViewTilingScheme::Geographic, this)),
  m_permissionsHelper(new PermissionsHelper(this))
{

  const QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/mspk/philadelphia.mspk";

  // Connect to the Mobile Scene Package instance to know when errors occur
  connect(MobileScenePackage::instance(), &MobileScenePackage::errorOccurred, [](Error e)
  {
    if (e.isEmpty())
      return;

    qDebug() << QString("Error: %1 %2").arg(e.message(), e.additionalMessage());
  });

  // Create the Scene Package
  createScenePackage(dataPath);
}

DisplayScenesInTabletopAR::~DisplayScenesInTabletopAR() = default;

void DisplayScenesInTabletopAR::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<DisplayScenesInTabletopAR>("Esri.Samples", 1, 0, "DisplayScenesInTabletopARSample");
}

SceneQuickView* DisplayScenesInTabletopAR::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void DisplayScenesInTabletopAR::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
    return;

  m_sceneView = sceneView;

  // Connect the mouse clicked events.
  connect(m_sceneView, &SceneQuickView::mouseClicked, this, &DisplayScenesInTabletopAR::onMouseClicked);

  emit sceneViewChanged();
}

// Slot for handling when the package loads
void DisplayScenesInTabletopAR::packageLoaded(const Error& e)
{
  if (!e.isEmpty())
  {
    qDebug() << QString("Package load error: %1 %2").arg(e.message(), e.additionalMessage());
    return;
  }

  if (m_scenePackage->scenes().isEmpty())
    return;

  // Get the first scene
  m_scene = m_scenePackage->scenes().at(0);

  // Create a camera at the bottom and center of the scene.
  // This camera is the point at which the scene is pinned to the real-world surface.
  m_originCamera = Camera(39.95787000283599, -75.16996728256345, 8.813445091247559, 0, 90, 0);
}

// Create scene package and connect to signals
void DisplayScenesInTabletopAR::createScenePackage(const QString& path)
{
  m_scenePackage = new MobileScenePackage(path, this);
  connect(m_scenePackage, &MobileScenePackage::doneLoading, this, &DisplayScenesInTabletopAR::packageLoaded);

  connect(m_permissionsHelper, &PermissionsHelper::requestFilesystemAccessCompleted, this, [this]()
  {
    m_scenePackage->load();
  });

  if (!m_permissionsHelper->fileSystemAccessGranted())
    m_permissionsHelper->requestFilesystemAccess();
  else
    m_scenePackage->load();
}

void DisplayScenesInTabletopAR::onMouseClicked(QMouseEvent& event)
{
  Q_CHECK_PTR(m_arcGISArView);

  if (!m_scene && !m_sceneView)
    return;

  // Display the scene
  if (m_sceneView->arcGISScene() != m_scene)
  {
    m_sceneView->setArcGISScene(m_scene);
    m_dialogVisible = false;
    emit dialogVisibleChanged();
  }

  // Set the clipping distance for the scene.
  m_arcGISArView->setClippingDistance(400);

  // Set the surface opacity to 0.
  m_arcGISArView->sceneView()->arcGISScene()->baseSurface()->setOpacity(0.0f);

  // Enable subsurface navigation. This allows you to look at the scene from below.
  m_sceneView->arcGISScene()->baseSurface()->setNavigationConstraint(NavigationConstraint::None);

  // Set the initial transformation using the point clicked on the screen
  const QPoint screenPoint = event.localPos().toPoint();
  m_arcGISArView->setInitialTransformation(screenPoint);

  // Set the origin camera.
  m_arcGISArView->setOriginCamera(m_originCamera);

  // Set the translation factor based on the scene content width and desired physical size.
  m_arcGISArView->setTranslationFactor(m_sceneWidth/m_tableTopWidth);

  emit sceneViewChanged();
  emit arcGISArViewChanged();
}

