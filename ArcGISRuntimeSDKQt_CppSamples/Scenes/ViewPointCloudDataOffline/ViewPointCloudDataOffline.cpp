// [WriteFile Name=ViewPointCloudDataOffline, Category=Scenes]
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

#include "ViewPointCloudDataOffline.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "PointCloudLayer.h"

#include <QDir>
#include <QtCore/qglobal.h>

#ifdef Q_OS_IOS
#include <QStandardPaths>
#endif // Q_OS_IOS

using namespace Esri::ArcGISRuntime;

ViewPointCloudDataOffline::ViewPointCloudDataOffline(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(Basemap::imagery(this), this))
{
  // create a new elevation source from Terrain3D service
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
        QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);  
}

ViewPointCloudDataOffline::~ViewPointCloudDataOffline() = default;

void ViewPointCloudDataOffline::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<ViewPointCloudDataOffline>("Esri.Samples", 1, 0, "ViewPointCloudDataOfflineSample");
}
SceneQuickView* ViewPointCloudDataOffline::sceneView() const
{
  return m_sceneView;
}

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

// Set the view (created in QML)
void ViewPointCloudDataOffline::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
  {
    return;
  }

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  // create the point cloud layer
  const QUrl pointCloudLyrUrl(defaultDataPath() + "/ArcGIS/Runtime/Data/slpk/sandiego-north-balboa-pointcloud.slpk");
  PointCloudLayer* pointCloudLyr = new PointCloudLayer(pointCloudLyrUrl, this);

  // zoom to layer once loaded
  connect(pointCloudLyr, &PointCloudLayer::doneLoading, this, [this, pointCloudLyr](Error e)
  {
    if (!e.isEmpty())
      return;

    m_sceneView->setViewpoint(Viewpoint(pointCloudLyr->fullExtent()));
  });

  // add the layer to the scene
  m_scene->operationalLayers()->append(pointCloudLyr);

  emit sceneViewChanged();
}


