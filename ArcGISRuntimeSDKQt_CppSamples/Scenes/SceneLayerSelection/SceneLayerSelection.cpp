// [WriteFile Name=SceneLayerSelection, Category=Scenes]
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "SceneLayerSelection.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "Surface.h"
#include "Basemap.h"
#include "ArcGISSceneLayer.h"
#include "Point.h"
#include "Camera.h"
#include "SpatialReference.h"
#include "Viewpoint.h"

using namespace Esri::ArcGISRuntime;

SceneLayerSelection::SceneLayerSelection(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void SceneLayerSelection::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<SceneLayerSelection>("Esri.Samples", 1, 0, "SceneLayerSelectionSample");
}

void SceneLayerSelection::componentComplete()
{
  QQuickItem::componentComplete();

  m_sceneView = findChild<SceneQuickView*>("sceneView");

  // Create a scene with the imagery basemap
  Scene* scene = new Scene(Basemap::imagery(this), this);

  // add a surface
  Surface* surface = new Surface(this);
  surface->elevationSources()->append(
        new ArcGISTiledElevationSource(
          QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"),
          this));
  scene->setBaseSurface(surface);

  // add a scene layer
  m_sceneLayer = new ArcGISSceneLayer(QUrl("https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/Buildings_Brest/SceneServer/layers/0"), this);
  scene->operationalLayers()->append(m_sceneLayer);

  // Set an initial viewpoint
  Point pt(-4.49779155626782, 48.38282454039932, 62.013264927081764, SpatialReference(4326));
  Camera camera(pt, 41.64729875588979, 71.2017391571523, 2.194677223e-314);
  Viewpoint initViewpoint(pt, camera);
  scene->setInitialViewpoint(initViewpoint);

  // connect signals
  connectSignals();

  // set the scene on the scene view
  m_sceneView->setArcGISScene(scene);
}

void SceneLayerSelection::connectSignals()
{
  // handle the identifyLayerCompleted signal
  connect(m_sceneView, &SceneQuickView::identifyLayerCompleted, this, [this](QUuid, IdentifyLayerResult* result)
  {
    // get the results
    QList<GeoElement*> geoElements = result->geoElements();

    // make sure we have at least 1 GeoElement
    if (geoElements.isEmpty())
      return;

    // get the first GeoElement
    GeoElement* geoElement = geoElements.at(0);

    // cast the GeoElement to a Feature
    Feature* feature = static_cast<Feature*>(geoElement);

    // select the Feature in the SceneLayer
    if (feature)
      feature->setParent(this);
      m_sceneLayer->selectFeature(feature);
  });

  // when the scene is clicked, identify the clicked feature and select it
  connect(m_sceneView, &SceneQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    // clear any previous selection
    m_sceneLayer->clearSelection();

    // identify from the click
    m_sceneView->identifyLayer(m_sceneLayer, mouseEvent.x(), mouseEvent.y(), 10, false);
  });
}


