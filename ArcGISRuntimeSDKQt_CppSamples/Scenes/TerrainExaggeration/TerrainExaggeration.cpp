// [WriteFile Name=TerrainExaggeration, Category=Scenes]
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

#include "TerrainExaggeration.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "SceneQuickView.h"

using namespace Esri::ArcGISRuntime;

TerrainExaggeration::TerrainExaggeration(QQuickItem* parent /* = nullptr */):
    QQuickItem(parent)
{
}

void TerrainExaggeration::init()
{
    // Register classes for QML
    qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
    qmlRegisterType<TerrainExaggeration>("Esri.Samples", 1, 0, "TerrainExaggerationSample");
}

void TerrainExaggeration::componentComplete()
{
    QQuickItem::componentComplete();

    // Create a scene and give it to the SceneView
    m_sceneView = findChild<SceneQuickView*>("sceneView");
    Scene* scene = new Scene(Basemap::imagery(this), this);
    Surface* surface = new Surface(this);
    surface->elevationSources()->append(
                new ArcGISTiledElevationSource(
                    QUrl("http://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"),
                    this));
    scene->setBaseSurface(surface);
    m_sceneView->setArcGISScene(scene);
}
