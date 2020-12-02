// [WriteFile Name=EditKmlGroundOverlay, Category=EditData]
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

#include "EditKmlGroundOverlay.h"

#include "Scene.h"
#include "SceneQuickView.h"
#include "KmlLayer.h"
#include "KmlGroundOverlay.h"
#include "KmlDataset.h"
#include "KmlIcon.h"
#include "Envelope.h"
#include "Camera.h"

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

EditKmlGroundOverlay::EditKmlGroundOverlay(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(Basemap::imagery(this), this))
{
  // Create Geometry
  const Envelope env(-123.066227926904, 44.04736963555683,
                     -123.0796942287304, 44.03878298600624,
                     SpatialReference(4326));

  // Create KML Icon
  KmlIcon* kmlIcon = new KmlIcon(QUrl(defaultDataPath() + "/ArcGIS/Runtime/Data/raster/1944.jpg"), this);

  // Create Ground Overlay
  m_groundOverlay = new KmlGroundOverlay(env, kmlIcon, this);
  m_groundOverlay->setRotation(-3.046024799346924);

  // Create Dataset
  KmlDataset* kmlDataset = new KmlDataset(m_groundOverlay, this);

  // Create Layer
  KmlLayer* kmlLayer = new KmlLayer(kmlDataset, this);

  // Connect to know when the layer loads
  connect(kmlLayer, &KmlLayer::doneLoading, this, [this, env](Error e)
  {
    if (!e.isEmpty() || !m_sceneView)
      return;

    const Camera camera(env.center(), 1250, 45, 60, 0);
    m_sceneView->setViewpointCamera(camera);
  });
  m_scene->operationalLayers()->append(kmlLayer);
}

EditKmlGroundOverlay::~EditKmlGroundOverlay() = default;

void EditKmlGroundOverlay::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<EditKmlGroundOverlay>("Esri.Samples", 1, 0, "EditKmlGroundOverlaySample");
}

void EditKmlGroundOverlay::setOpacity(int opacity)
{
  if (!m_groundOverlay)
    return;

  m_groundOverlay->setColor(QColor(0, 0, 0, opacity));
}

SceneQuickView* EditKmlGroundOverlay::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void EditKmlGroundOverlay::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
    return;   

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);  

  emit sceneViewChanged();
}

