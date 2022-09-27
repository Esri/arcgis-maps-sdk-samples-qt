// [WriteFile Name=RealisticLightingAndShadows, Category=Scenes]
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

#include "RealisticLightingAndShadows.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "ArcGISSceneLayer.h"
#include "Camera.h"
#include "MapTypes.h"
#include "TaskWatcher.h"
#include "Surface.h"
#include "ElevationSourceListModel.h"
#include "LayerListModel.h"
#include "SceneViewTypes.h"

#include <QUrl>
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QTimeZone>

#include <cmath>

using namespace Esri::ArcGISRuntime;

RealisticLightingAndShadows::RealisticLightingAndShadows(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(BasemapStyle::ArcGISTopographic, this))
{
  // create a new elevation source from Terrain3D REST service
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
        QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);

  // add 3D building shells with a scene layer
  ArcGISSceneLayer* scenelayer = new ArcGISSceneLayer(
        QUrl("https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/DevA_BuildingShells/SceneServer/layers/0"), this);

  m_scene->operationalLayers()->append(scenelayer);
}

RealisticLightingAndShadows::~RealisticLightingAndShadows() = default;

void RealisticLightingAndShadows::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<RealisticLightingAndShadows>("Esri.Samples", 1, 0, "RealisticLightingAndShadowsSample");
}

SceneQuickView* RealisticLightingAndShadows::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void RealisticLightingAndShadows::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
    return;

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  // add camera and set scene viewpoint
  const Camera camera(45.54605153789073, -122.69033380511073, 941.0002111233771, 162.58544227544266, 60.0,0.0);
  m_sceneView->setViewpointCamera(camera, 0);

  // set atmosphere effect to realistic
  m_sceneView->setAtmosphereEffect(AtmosphereEffect::Realistic);

  // set the sun time to the calendar
  setSunTimeFromValue(8.5);

  // add sun lighting
  m_sceneView->setSunLighting(LightingMode::LightAndShadows);

  emit sceneViewChanged();
}

void RealisticLightingAndShadows::setSunTimeFromValue(double sunTimeValue)
{
  if (!m_sceneView)
    return;

  // convert a double from 0.0 to 23.99 into hours and minutes
  double remainder = std::fmod(sunTimeValue, 1);
  int minute = remainder * 60;
  int hour = sunTimeValue - remainder;

  const QTime selectedTime = QTime(hour, minute);

  // set a calendar with a date, time, and timezone
  const QDateTime sunTime(QDate(2018, 8, 10), selectedTime, QTimeZone(-25200));
  // To represent the PST time zone during daylight savings time (-7 hours), we use the number of seconds of offset.
  // 3600 seconds/hour * -7 hours = -25,200 seconds.

  // set the sun time to the calendar
  m_sceneView->setSunTime(sunTime);

  // trigger the time in the settings column to update
  emit sunTimeChanged(selectedTime.toString("h:mm ap"));
}

void RealisticLightingAndShadows::setLightingMode(int lightingModeValue)
{
  if (!m_sceneView)
    return;

  if (lightingModeValue == 0)
  {
    m_sceneView->setSunLighting(LightingMode::NoLight);
  }
  else if (lightingModeValue == 1)
  {
    m_sceneView->setSunLighting(LightingMode::Light);
  }
  else
  {
    m_sceneView->setSunLighting(LightingMode::LightAndShadows);
  }
}
