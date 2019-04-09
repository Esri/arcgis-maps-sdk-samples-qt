// [WriteFile Name=ChangeAtmosphereEffect, Category=Scenes]
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

#ifndef CHANGEATMOSPHEREEFFECT_H
#define CHANGEATMOSPHEREEFFECT_H

namespace Esri
{
namespace ArcGISRuntime
{
class Scene;
class SceneQuickView;
}
}

#include <QObject>

#include "SceneViewTypes.h"

class ChangeAtmosphereEffect : public QObject
{
  Q_OBJECT
  Q_PROPERTY(AtmosphereEnum atmosphereEffect READ atmosphereEffect WRITE setAtmosphereEffect NOTIFY atmosphereEffectChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)
public:
  enum AtmosphereEnum {
    None = static_cast<int>(Esri::ArcGISRuntime::AtmosphereEffect::None),
    HorizonOnly = static_cast<int>(Esri::ArcGISRuntime::AtmosphereEffect::HorizonOnly),
    Realistic = static_cast<int>(Esri::ArcGISRuntime::AtmosphereEffect::Realistic)
  };
  Q_ENUM(AtmosphereEnum)

  explicit ChangeAtmosphereEffect(QObject* parent = nullptr);
  ~ChangeAtmosphereEffect() override;

  static void init();

signals:
  void sceneViewChanged();
  void atmosphereEffectChanged();

private:
  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);

  void setAtmosphereEffect(AtmosphereEnum atmosphereEffect);
  AtmosphereEnum atmosphereEffect() const;

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
};

#endif // CHANGEATMOSPHEREEFFECT_H
