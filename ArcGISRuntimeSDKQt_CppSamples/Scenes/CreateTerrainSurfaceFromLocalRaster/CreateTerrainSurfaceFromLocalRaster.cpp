// [WriteFile Name=CreateTerrainSurfaceFromLocalRaster, Category=Scenes]
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

#include "CreateTerrainSurfaceFromLocalRaster.h"

#include "Scene.h"
#include "SceneQuickView.h"
#include "RasterElevationSource.h"

#include <QUrl>
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

CreateTerrainSurfaceFromLocalRaster::CreateTerrainSurfaceFromLocalRaster(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(Basemap::imagery(this), this))
{
  // create the MontereyElevation data path
  // data is downloaded automatically by the sample viewer app. Instructions to download
  // separately are specified in the readme.
  const auto montereyRasterElevationPath = QStringList{defaultDataPath() + "/ArcGIS/Runtime/Data/raster/MontereyElevation.dt2"};

  //Create the elevation source from the local raster(s). RasterElevationSource can take multiple files as inputs, but in this case only takes one.
  auto* elevationSrc = new RasterElevationSource(montereyRasterElevationPath, this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSrc);
}

CreateTerrainSurfaceFromLocalRaster::~CreateTerrainSurfaceFromLocalRaster() = default;

void CreateTerrainSurfaceFromLocalRaster::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<CreateTerrainSurfaceFromLocalRaster>("Esri.Samples", 1, 0, "CreateTerrainSurfaceFromLocalRasterSample");
}

SceneQuickView* CreateTerrainSurfaceFromLocalRaster::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void CreateTerrainSurfaceFromLocalRaster::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
  {
    return;
  }

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  // Create a camera, looking at the Monterey, California.
  constexpr double latitude = 36.51;
  constexpr double longitude = -121.80;
  constexpr double altitude = 300.0;
  constexpr double heading = 0.0;
  constexpr double pitch = 70.0;
  constexpr double roll = 0.0;
  Camera camera(latitude, longitude, altitude, heading, pitch, roll);

  // Set the sceneview to use above camera, waits for load so scene is immediately displayed in appropriate place.
  m_sceneView->setViewpointCameraAndWait(camera);

  emit sceneViewChanged();
}

