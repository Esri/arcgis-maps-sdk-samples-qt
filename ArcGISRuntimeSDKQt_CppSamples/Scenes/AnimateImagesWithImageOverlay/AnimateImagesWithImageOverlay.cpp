// [WriteFile Name=AnimateImagesWithImageOverlay, Category=Scenes]
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

#include "AnimateImagesWithImageOverlay.h"

#include "ArcGISTiledElevationSource.h"
#include "ArcGISTiledLayer.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "ImageFrame.h"
#include "ImageOverlay.h"
#include "ImageOverlayListModel.h"
#include "SceneView.h"
#include "Envelope.h"
#include "Point.h"
#include "Camera.h"
#include "Viewpoint.h"

// Qt headers
#include <QString>
#include <QDir>
#include <QFile>
#include <QtCore/qglobal.h>
#include <QTimer>

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

AnimateImagesWithImageOverlay::AnimateImagesWithImageOverlay(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(this))
{
  // create a new elevation source from Terrain3D REST service
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
        QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // create a new tiled layer from World_Dark_Gray_Base REST service
  ArcGISTiledLayer* worldDarkGrayBasemap = new ArcGISTiledLayer(QUrl("https://services.arcgisonline.com/arcgis/rest/services/Canvas/World_Dark_Gray_Base/MapServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);

  // add the tiled layer to the operational layers to display the basemap
  m_scene->operationalLayers()->append(worldDarkGrayBasemap);
}

AnimateImagesWithImageOverlay::~AnimateImagesWithImageOverlay() = default;

void AnimateImagesWithImageOverlay::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<AnimateImagesWithImageOverlay>("Esri.Samples", 1, 0, "AnimateImagesWithImageOverlaySample");
}

SceneQuickView* AnimateImagesWithImageOverlay::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void AnimateImagesWithImageOverlay::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
    return;

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  emit sceneViewChanged();

  setupImageOverlay();
}

void AnimateImagesWithImageOverlay::setupImageOverlay()
{
  // create an envelope of the pacific southwest sector for displaying the image frame
  const Point pointForImageFrame(-120.0724273439448, 35.131016955536694, SpatialReference(4326));
  m_pacificSouthwestEnvelope = Envelope(pointForImageFrame, 15.09589635986124, -14.3770441522488);

  // create a camera, looking at the pacific southwest sector
  const Point observationPoint(-116.621, 24.7773, 856977);
  const Camera camera(observationPoint, 353.994, 48.5495, 0);
  const Viewpoint pacificSouthwestViewpoint(observationPoint, camera);

  m_scene->setInitialViewpoint(pacificSouthwestViewpoint);

  // create an image overlay
  m_imageOverlay = new ImageOverlay(this);

  // append the image overlay to the scene view
  m_sceneView->imageOverlays()->append(m_imageOverlay);

  // Create new Timer and set the timeout interval to 68
  // 68 ms interval timer equates to approximatey 15 frames a second
  m_timer = new QTimer(this);
  m_timer->setInterval(68);

  // connect to the timeout signal to load and display a new image each time
  connect(m_timer, &QTimer::timeout, this, &AnimateImagesWithImageOverlay::animateImageFrames);
}

void AnimateImagesWithImageOverlay::animateImageFrames()
{
  //  const QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/3D/ImageOverlay/PacificSouthWest";
  //  const QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/3D/ImageOverlay/PacificSouthWest1";
  //  const QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/3D/ImageOverlay/PacificSouthWest";
//  const QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/3D/ImageOverlay/newdata";
//  const QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/3D/ImageOverlay/200ishdata";
    const QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/3D/ImageOverlay/240_4_sec";

  // get the image files and store the names
  const QDir dir(dataPath);
  const QStringList images = dir.entryList(QStringList() << "*.png", QDir::Files);

  // create an image with the given path and use it to create an image frame
  const QImage image(dataPath + "/" + images[m_index]);
  ImageFrame* imageFrame = new ImageFrame(image, m_pacificSouthwestEnvelope, this);

  // connect to the doneLoading signal to set the image frame to the image overlay
  connect(imageFrame, &ImageFrame::doneLoading, this, [this, imageFrame, images](Error e)
  {
    if (!e.isEmpty())
    {
      qDebug() << e.message();
      return;
    }

    // set image frame to image overlay
    m_imageOverlay->setImageFrame(imageFrame);

    // delete the image frame
    delete imageFrame;

    // increment the index to keep track of which image to load next
    m_index++;

    // reset index once all files have been loaded
    if (m_index == images.size())
      m_index = 0;
  });
  imageFrame->load();
}

void AnimateImagesWithImageOverlay::setTimerInterval(int value)
{
  if (!m_timer)
    return;

  m_timer->setInterval(value);
}
void AnimateImagesWithImageOverlay::setOpacity(float value)
{
  if (!m_imageOverlay)
    return;

  m_imageOverlay->setOpacity(value);
}

void AnimateImagesWithImageOverlay::startTimer()
{
  if (!m_timer)
    return;

  m_isStopped = false;
  emit isStoppedChanged();
  m_timer->start();
}

void AnimateImagesWithImageOverlay::stopTimer()
{
  if (!m_timer)
    return;

  m_isStopped = true;
  emit isStoppedChanged();
  m_timer->stop();
}

void AnimateImagesWithImageOverlay::getInfo()
{
  auto temp = m_sceneView->currentViewpointCamera();
//  qDebug() << QString("here%1 %2").arg(1, 2);
  qDebug() << temp.location().x();
  qDebug() << temp.location().y();
  qDebug() << temp.location().z(); //QString("point x:%1, y:%2, z:%3").arg(t, , temp.location().z());
  qDebug() << temp.heading();
  qDebug() << temp.pitch();
  qDebug() << temp.roll();
}
