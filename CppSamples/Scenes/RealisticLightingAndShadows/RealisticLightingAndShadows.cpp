// [WriteFile Name=RealisticLightingAndShadows, Category=Scenes]
// [Legal]
// Copyright 2020 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "RealisticLightingAndShadows.h"

// ArcGIS Maps SDK headers
#include "ArcGISSceneLayer.h"
#include "ArcGISTiledElevationSource.h"
#include "Camera.h"
#include "ElevationSourceListModel.h"
#include "LayerListModel.h"
#include "MapTypes.h"
#include "Scene.h"
#include "LocalSceneQuickView.h"
#include "SceneViewTypes.h"
#include "Surface.h"
#include "SceneEnvironment.h"
#include "SceneLighting.h"
#include "SunLighting.h"
#include "VirtualLighting.h"

// Qt headers
#include <QDate>
#include <QDateTime>
#include <QFuture>
#include <QTime>
#include <QTimeZone>
#include <QUrl>

// STL headers
#include <cmath>

using namespace Esri::ArcGISRuntime;

RealisticLightingAndShadows::RealisticLightingAndShadows(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(BasemapStyle::ArcGISTopographic, SceneViewingMode::Local, this))
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
  qmlRegisterType<LocalSceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<RealisticLightingAndShadows>("Esri.Samples", 1, 0, "RealisticLightingAndShadowsSample");
}

LocalSceneQuickView* RealisticLightingAndShadows::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void RealisticLightingAndShadows::setSceneView(LocalSceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
    return;

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  // add camera and set scene viewpoint
  const Camera camera(45.54605153789073, -122.69033380511073, 941.0002111233771, 162.58544227544266, 60.0,0.0);
  m_sceneView->setViewpointCameraAsync(camera, 0);

  // set atmosphere effect to realistic
  // m_sceneView->setAtmosphereEffect(AtmosphereEffect::Realistic);

  // set the sun time to the calendar
  setSunTimeFromValue(8.5);

  // add sun lighting
  // m_sceneView->setSunLighting(LightingMode::LightAndShadows);

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
  // m_sceneView->setSunTime(sunTime);
  if (auto* sunLighting = dynamic_cast<SunLighting*>(m_scene->environment()->lighting()))
  {
    sunLighting->setSimulatedDate(sunTime);
  }

  // trigger the time in the settings column to update
  emit sunTimeChanged(selectedTime.toString("h:mm ap"));
}

void RealisticLightingAndShadows::setLightingMode(int lightingModeValue)
{
  if (!m_sceneView)
    return;

  qDebug() << lightingModeValue;

  if (lightingModeValue == 0)
  {
    // Virtual and no shadows
    // m_sceneView->setSunLighting(LightingMode::NoLight);
    m_lighting = new VirtualLighting(false, m_lightingParent.get());
  }
  else if (lightingModeValue == 1)
  {
    // Virtual and shadows
    // m_sceneView->setSunLighting(LightingMode::NoLight);
    m_lighting = new VirtualLighting(true, m_lightingParent.get());
  }
  else if (lightingModeValue == 2)
  {
    // m_sceneView->setSunLighting(LightingMode::Light);
    m_lighting = new SunLighting(QDateTime(QDate(2018, 8, 10), QTime(12, 0), QTimeZone(-25200)), false, m_lightingParent.get());
  }
  else
  {
    // m_sceneView->setSunLighting(LightingMode::LightAndShadows);
    m_lighting = new SunLighting(QDateTime(QDate(2018, 8, 10), QTime(12, 0), QTimeZone(-25200)), true, m_lightingParent.get());
  }

  m_scene->environment()->setLighting(m_lighting);
}
