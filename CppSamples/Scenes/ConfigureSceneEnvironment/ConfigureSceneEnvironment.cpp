// [WriteFile Name=ConfigureSceneEnvironment, Category=Scenes]
// [Legal]
// Copyright 2026 Esri.
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
#include "ConfigureSceneEnvironment.h"

// ArcGIS Maps SDK headers
#include "LocalSceneQuickView.h"
#include "Scene.h"
#include "SceneEnvironment.h"
#include "SunLighting.h"
#include "TimeZoneOffset.h"
#include "VirtualLighting.h"

// Qt headers
#include <QColor>

using namespace Esri::ArcGISRuntime;

ConfigureSceneEnvironment::ConfigureSceneEnvironment(QObject* parent /* = nullptr */) :
  QObject(parent),
  m_scene(new Scene(QUrl("https://www.arcgis.com/home/item.html?id=fcebd77958634ac3874bbc0e6b0677a4"), this))
{
  // Read the initial environment state once the scene has loaded.
  connect(m_scene, &Scene::doneLoading, this, &ConfigureSceneEnvironment::readInitialEnvironmentState);
}

ConfigureSceneEnvironment::~ConfigureSceneEnvironment() = default;

void ConfigureSceneEnvironment::init()
{
  qmlRegisterType<LocalSceneQuickView>("Esri.Samples", 1, 0, "LocalSceneView");
  qmlRegisterType<ConfigureSceneEnvironment>("Esri.Samples", 1, 0, "ConfigureSceneEnvironmentSample");
}

LocalSceneQuickView* ConfigureSceneEnvironment::localSceneView() const
{
  return m_localSceneView;
}

void ConfigureSceneEnvironment::setLocalSceneView(LocalSceneQuickView* localSceneView)
{
  if (!localSceneView || localSceneView == m_localSceneView)
  {
    return;
  }

  m_localSceneView = localSceneView;
  m_localSceneView->setArcGISScene(m_scene);

  emit localSceneViewChanged();
}

void ConfigureSceneEnvironment::readInitialEnvironmentState()
{
  if (!m_scene || !m_scene->environment())
  {
    return;
  }

  SceneEnvironment* env = m_scene->environment();

  // Read sky state.
  m_starsEnabled = env->areStarsEnabled();
  emit starsEnabledChanged();

  m_atmosphereEnabled = env->isAtmosphereEnabled();
  emit atmosphereEnabledChanged();

  // Read lighting state.
  if (SunLighting* sun = qobject_cast<SunLighting*>(env->lighting()))
  {
    m_sunLighting = true;
    m_directShadowsEnabled = sun->areDirectShadowsEnabled();
    m_lightingDateTime = sun->simulatedDate();

    // Time zone offset
    if (TimeZoneOffset* tz = sun->displayTimeZone())
    {
      m_lightingTimeZoneOffsetHours = tz->hours();
    }

    m_lightingHour = m_lightingDateTime.addSecs(m_lightingTimeZoneOffsetHours * 3600LL).time().hour();
  }
  else
  {
    m_sunLighting = false;
    m_directShadowsEnabled = env->lighting()->areDirectShadowsEnabled();
  }

  emit sunLightingChanged();
  emit directShadowsEnabledChanged();
  emit lightingHourChanged();
}

bool ConfigureSceneEnvironment::starsEnabled() const
{
  return m_starsEnabled;
}

void ConfigureSceneEnvironment::setStarsEnabled(bool enabled)
{
  if (m_starsEnabled == enabled)
  {
    return;
  }

  if (!m_scene || !m_scene->environment())
  {
    return;
  }

  m_starsEnabled = enabled;
  m_scene->environment()->setStarsEnabled(enabled);
  emit starsEnabledChanged();
}

bool ConfigureSceneEnvironment::atmosphereEnabled() const
{
  return m_atmosphereEnabled;
}

void ConfigureSceneEnvironment::setAtmosphereEnabled(bool enabled)
{
  if (m_atmosphereEnabled == enabled)
  {
    return;
  }

  if (!m_scene || !m_scene->environment())
  {
    return;
  }

  m_atmosphereEnabled = enabled;
  m_scene->environment()->setAtmosphereEnabled(enabled);
  emit atmosphereEnabledChanged();
}

QString ConfigureSceneEnvironment::backgroundColor() const
{
  return m_backgroundColor;
}

void ConfigureSceneEnvironment::setBackgroundColor(const QString& color)
{
  if (m_backgroundColor == color)
  {
    return;
  }

  if (!m_scene || !m_scene->environment())
  {
    return;
  }

  m_backgroundColor = color;
  m_scene->environment()->setBackgroundColor(QColor(color));

  // Disable atmosphere and stars so the background color is visible.
  setAtmosphereEnabled(false);
  setStarsEnabled(false);

  emit backgroundColorChanged();
}

bool ConfigureSceneEnvironment::sunLighting() const
{
  return m_sunLighting;
}

void ConfigureSceneEnvironment::setSunLighting(bool isSun)
{
  if (m_sunLighting == isSun)
  {
    return;
  }

  if (!m_scene || !m_scene->environment())
  {
    return;
  }

  m_sunLighting = isSun;

  if (isSun)
  {
    SunLighting* sun = new SunLighting(m_lightingDateTime, m_directShadowsEnabled, this);
    m_scene->environment()->setLighting(sun);
  }
  else
  {
    VirtualLighting* virtualLight = new VirtualLighting(m_directShadowsEnabled, this);
    m_scene->environment()->setLighting(virtualLight);
  }

  emit sunLightingChanged();
}

bool ConfigureSceneEnvironment::directShadowsEnabled() const
{
  return m_directShadowsEnabled;
}

void ConfigureSceneEnvironment::setDirectShadowsEnabled(bool enabled)
{
  if (m_directShadowsEnabled == enabled)
  {
    return;
  }

  if (!m_scene || !m_scene->environment() || !m_scene->environment()->lighting())
  {
    return;
  }

  m_directShadowsEnabled = enabled;
  m_scene->environment()->lighting()->setDirectShadowsEnabled(enabled);
  emit directShadowsEnabledChanged();
}

int ConfigureSceneEnvironment::lightingHour() const
{
  return m_lightingHour;
}

void ConfigureSceneEnvironment::setLightingHour(int hour)
{
  if (m_lightingHour == hour)
  {
    return;
  }

  if (!m_scene || !m_scene->environment() || !m_scene->environment()->lighting())
  {
    return;
  }

  const int hourDiff = hour - m_lightingHour;
  m_lightingHour = hour;
  m_lightingDateTime = m_lightingDateTime.addSecs(hourDiff * 3600LL);

  if (SunLighting* sun = qobject_cast<SunLighting*>(m_scene->environment()->lighting()))
  {
    sun->setSimulatedDate(m_lightingDateTime);
  }

  emit lightingHourChanged();
}
