// [WriteFile Name=ExploreScenesInFlyoverAR, Category=AR]
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

#include "ExploreScenesInFlyoverAR.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "IntegratedMeshLayer.h"

using namespace Esri::ArcGISRuntime;
namespace toolkit = Esri::ArcGISRuntime::Toolkit;

ExploreScenesInFlyoverAR::ExploreScenesInFlyoverAR(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(Basemap::imagery(this), this))
{
  // create a new elevation source from Terrain3D REST service
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
        QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);
  m_scene->baseSurface()->setOpacity(0.0f);

  // create the integrated mesh layer
  const QUrl meshLyrUrl("https://www.arcgis.com/home/item.html?id=dbc72b3ebb024c848d89a42fe6387a1b");
  m_integratedMeshLayer = new IntegratedMeshLayer(meshLyrUrl, this);

  // add the layer to the scene
  m_scene->operationalLayers()->append(m_integratedMeshLayer);

  connect(m_integratedMeshLayer, &IntegratedMeshLayer::doneLoading, this, [this](Error e)
  {
    if (!e.isEmpty())
    {
      qDebug() << e.code() << e.message() << " - " << e.additionalMessage();
      return;
    }

    // Enable subsurface navigation. This allows you to look at the scene from below.
    m_scene->baseSurface()->setNavigationConstraint(NavigationConstraint::StayAbove);
    m_scene->baseSurface()->setOpacity(1.0f);

    const Point centerPoint = m_integratedMeshLayer->fullExtent().center();

    // Start with the camera at the center of the mesh layer.
    m_originCamera = Camera(centerPoint.y(), centerPoint.x(), 250, 0, 90, 0);
    arcGISArView()->setOriginCamera(m_originCamera);

    // Set the translation factor to enable rapid movement through the scene.
    arcGISArView()->setTranslationFactor(1000);

    // Enable atmosphere and space effects for a more immersive experience.
    m_sceneView->setSpaceEffect(SpaceEffect::Stars);
    m_sceneView->setAtmosphereEffect(AtmosphereEffect::Realistic);
    emit arcGISArViewChanged();
    emit sceneViewChanged();
  });
}

ExploreScenesInFlyoverAR::~ExploreScenesInFlyoverAR() = default;

void ExploreScenesInFlyoverAR::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<ExploreScenesInFlyoverAR>("Esri.Samples", 1, 0, "ExploreScenesInFlyoverARSample");
}

SceneQuickView* ExploreScenesInFlyoverAR::sceneView() const
{
  return m_sceneView;
}

toolkit::ArcGISArView* ExploreScenesInFlyoverAR::arcGISArView() const
{
  return m_arcGISArView;
}

// Set the view (created in QML)
void ExploreScenesInFlyoverAR::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
    return;

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  emit sceneViewChanged();
}

// Set the AR view
void ExploreScenesInFlyoverAR::setArcGISArView(toolkit::ArcGISArView* arcGISArView)
{
  if (!arcGISArView || arcGISArView == m_arcGISArView)
    return;

  m_arcGISArView = arcGISArView;
  m_arcGISArView->setTracking(true);

  emit arcGISArViewChanged();
}

