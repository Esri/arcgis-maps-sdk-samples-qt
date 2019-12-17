// [WriteFile Name=GroupLayers, Category=Layers]
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "GroupLayers.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "GroupLayer.h"
#include "ArcGISSceneLayer.h"
#include "FeatureLayer.h"
#include "ServiceFeatureTable.h"
#include "Viewpoint.h"
#include "Envelope.h"

using namespace Esri::ArcGISRuntime;

GroupLayers::GroupLayers(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(Basemap::imagery(this), this))
{
  // create a new elevation source from Terrain3D REST service
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
        QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);

  // Create layers and append to a group layer
  ArcGISSceneLayer* layer1 = new ArcGISSceneLayer(QUrl("https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/DevA_BuildingShells/SceneServer"), this);
  ArcGISSceneLayer* layer2 = new ArcGISSceneLayer(QUrl("https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/DevB_BuildingShells/SceneServer"), this);
  GroupLayer* groupLayer = new GroupLayer(QList<Layer*>{layer1, layer2}, this);
  groupLayer->setName("Buildings group");

  // Create additional layers
  ArcGISSceneLayer* layer3 = new ArcGISSceneLayer(QUrl("https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/DevA_Trees/SceneServer"), this);
  ServiceFeatureTable* ft1 = new ServiceFeatureTable(QUrl("https://services.arcgis.com/P3ePLMYs2RVChkJx/arcgis/rest/services/DevelopmentProjectArea/FeatureServer/0"), this);
  FeatureLayer* layer4 = new FeatureLayer(ft1, this);
  ServiceFeatureTable* ft2 = new ServiceFeatureTable(QUrl("https://services.arcgis.com/P3ePLMYs2RVChkJx/arcgis/rest/services/DevA_Pathways/FeatureServer/1"), this);
  FeatureLayer* layer5 = new FeatureLayer(ft2, this);

  // Add layers to the scene
  m_scene->operationalLayers()->append(groupLayer);
  m_scene->operationalLayers()->append(layer3);
  m_scene->operationalLayers()->append(layer4);
  m_scene->operationalLayers()->append(layer5);

  connect(m_scene, &Scene::doneLoading, this, [this](Error e)
  {
    if (!e.isEmpty())
      return;

    m_layerListModel = m_scene->operationalLayers();
    emit layerListModelChanged();
  });
}

GroupLayers::~GroupLayers() = default;

void GroupLayers::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<GroupLayers>("Esri.Samples", 1, 0, "GroupLayersSample");
  qmlRegisterUncreatableType<LayerListModel>("Esri.Samples", 1, 0, "AbstractListModel", "AbstractListModel is uncreateable");
}

SceneQuickView* GroupLayers::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void GroupLayers::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
  {
    return;
  }

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  // create initial viewpoint
  Geometry env = Geometry::fromJson(R"({"spatialReference":{
                                    "wkid":4326
                                    },
                                    "xmax":-122.67960721754773,
                                    "xmin":-122.68647066116789,
                                    "ymax":45.53584958588318,
                                    "ymin":45.531539857343745
                                    })");
  // zoom to viewpoint
  m_sceneView->setViewpoint(Viewpoint(env.extent()));

  emit sceneViewChanged();
}

LayerListModel* GroupLayers::getGroupLayerListModel(int layerId)
{
  if (!m_scene)
    return nullptr;

  GroupLayer* groupLayer = static_cast<GroupLayer*>(m_scene->operationalLayers()->at(layerId));

  if (groupLayer)
    return groupLayer->layers();

  return nullptr;
}
