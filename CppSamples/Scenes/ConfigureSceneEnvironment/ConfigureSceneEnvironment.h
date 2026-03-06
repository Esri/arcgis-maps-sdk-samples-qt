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

#ifndef CONFIGURESCENEENVIRONMENT_H
#define CONFIGURESCENEENVIRONMENT_H

// Qt headers
#include <QDateTime>
#include <QObject>

namespace Esri::ArcGISRuntime
{
  class Scene;
  class LocalSceneQuickView;
} // namespace Esri::ArcGISRuntime

Q_MOC_INCLUDE("LocalSceneQuickView.h");

class ConfigureSceneEnvironment : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::LocalSceneQuickView* localSceneView READ localSceneView WRITE setLocalSceneView NOTIFY localSceneViewChanged)

  // Sky properties.
  Q_PROPERTY(bool starsEnabled READ starsEnabled WRITE setStarsEnabled NOTIFY starsEnabledChanged)
  Q_PROPERTY(bool atmosphereEnabled READ atmosphereEnabled WRITE setAtmosphereEnabled NOTIFY atmosphereEnabledChanged)

  // Background color property.
  Q_PROPERTY(QString backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)

  // Lighting properties.
  Q_PROPERTY(bool sunLighting READ sunLighting WRITE setSunLighting NOTIFY sunLightingChanged)
  Q_PROPERTY(bool directShadowsEnabled READ directShadowsEnabled WRITE setDirectShadowsEnabled NOTIFY directShadowsEnabledChanged)
  Q_PROPERTY(int lightingHour READ lightingHour WRITE setLightingHour NOTIFY lightingHourChanged)

public:
  explicit ConfigureSceneEnvironment(QObject* parent = nullptr);
  ~ConfigureSceneEnvironment() override;

  static void init();

  // Sky accessors.
  bool starsEnabled() const;
  void setStarsEnabled(bool enabled);

  bool atmosphereEnabled() const;
  void setAtmosphereEnabled(bool enabled);

  // Background color accessors.
  QString backgroundColor() const;
  void setBackgroundColor(const QString& color);

  // Lighting accessors.
  bool sunLighting() const;
  void setSunLighting(bool isSun);

  bool directShadowsEnabled() const;
  void setDirectShadowsEnabled(bool enabled);

  int lightingHour() const;
  void setLightingHour(int hour);

signals:
  void localSceneViewChanged();
  void starsEnabledChanged();
  void atmosphereEnabledChanged();
  void backgroundColorChanged();
  void sunLightingChanged();
  void directShadowsEnabledChanged();
  void lightingHourChanged();

private:
  Esri::ArcGISRuntime::LocalSceneQuickView* localSceneView() const;
  void setLocalSceneView(Esri::ArcGISRuntime::LocalSceneQuickView* localSceneView);

  void readInitialEnvironmentState();

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::LocalSceneQuickView* m_localSceneView = nullptr;

  // Tracked environment state.
  bool m_starsEnabled = true;
  bool m_atmosphereEnabled = true;
  QString m_backgroundColor = "transparent";
  bool m_sunLighting = true;
  bool m_directShadowsEnabled = false;
  QDateTime m_lightingDateTime;
  int m_lightingTimeZoneOffsetHours = 0;
  int m_lightingHour = 12;
};

#endif // CONFIGURESCENEENVIRONMENT_H
