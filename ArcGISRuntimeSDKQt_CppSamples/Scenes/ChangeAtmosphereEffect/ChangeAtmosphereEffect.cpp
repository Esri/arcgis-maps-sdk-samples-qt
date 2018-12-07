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

#include "ChangeAtmosphereEffect.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "SceneQuickView.h"

using namespace Esri::ArcGISRuntime;

ChangeAtmosphereEffect::ChangeAtmosphereEffect(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(Basemap::imagery(this), this))
{
  // create a new elevation source from %{ElevationOption} rest service
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
        QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);

}

ChangeAtmosphereEffect::~ChangeAtmosphereEffect() = default;

void ChangeAtmosphereEffect::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<ChangeAtmosphereEffect>("Esri.Samples", 1, 0, "ChangeAtmosphereEffectSample");
}
SceneQuickView* ChangeAtmosphereEffect::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void ChangeAtmosphereEffect::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
  {
    return;
  }

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  Camera camera(64.416919, -14.483728, 100, 318, 105, 0);
  sceneView->setViewpointCamera(camera, 0);

  emit sceneViewChanged();
}

void ChangeAtmosphereEffect::setAtmosphereEffect(AtmosphereEnum atmosphereEffect)
{
  if (m_sceneView)
  {
    m_sceneView->setAtmosphereEffect(static_cast<AtmosphereEffect>(atmosphereEffect));
    emit atmosphereEffectChanged();
  }
}

ChangeAtmosphereEffect::AtmosphereEnum ChangeAtmosphereEffect::atmosphereEffect() const
{
  if (m_sceneView)
    return static_cast<AtmosphereEnum>(m_sceneView->atmosphereEffect());
  return static_cast<AtmosphereEnum>(AtmosphereEffect::None);
}


