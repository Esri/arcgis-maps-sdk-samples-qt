// [WriteFile Name=FeatureLayerExtrusion, Category=Scenes]
// [Legal]
// Copyright 2017 Esri.

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

#include "FeatureLayerExtrusion.h"

#include "FeatureLayer.h"
#include "ServiceFeatureTable.h"
#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "Point.h"
#include "SimpleLineSymbol.h"
#include "SimpleFillSymbol.h"
#include "SimpleRenderer.h"
#include "RendererSceneProperties.h"

using namespace Esri::ArcGISRuntime;

FeatureLayerExtrusion::FeatureLayerExtrusion(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  // define line and fill symbols for a simple renderer
  m_lineSymbol(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("Black"), 1.0f, this)),
  m_fillSymbol(new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("Blue"), m_lineSymbol, this)),
  m_renderer(new SimpleRenderer(m_fillSymbol, this))
{
  // set renderer extrusion mode to absolute to prevent clipping
  RendererSceneProperties props = m_renderer->sceneProperties();
  props.setExtrusionMode(ExtrusionMode::AbsoluteHeight);
  props.setExtrusionExpression("[POP2007] / 10");
  m_renderer->setSceneProperties(props);
}

void FeatureLayerExtrusion::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<FeatureLayerExtrusion>("Esri.Samples", 1, 0, "FeatureLayerExtrusionSample");
}

void FeatureLayerExtrusion::componentComplete()
{
  QQuickItem::componentComplete();

  // Create the feature service to use
  m_featureTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/Census/MapServer/3"), this);

  // add the service feature table to a feature layer
  m_featureLayer = new FeatureLayer(m_featureTable, this);

  // set the feature layer to render dynamically to allow extrusion
  m_featureLayer->setRenderingMode(FeatureRenderingMode::Dynamic);

  // set the simple renderer to the feature layer
  m_featureLayer->setRenderer(m_renderer);

  // Create a scene and give it to the SceneView
  m_sceneView = findChild<SceneQuickView*>("sceneView");
  Scene* scene = new Scene(Basemap::imagery(this), this);
  Surface* surface = new Surface(this);
  surface->elevationSources()->append(
        new ArcGISTiledElevationSource(
          QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"),
          this));
  scene->setBaseSurface(surface);
  scene->operationalLayers()->append(m_featureLayer);

  // set initial viewpoint
  const double distance = 12940924;
  const Point lookAtPoint(-99.659448, 20.513652, distance, SpatialReference::wgs84());
  const Camera camera(lookAtPoint, 0, 15, 0);
  const Viewpoint initialVp(lookAtPoint, distance, camera);
  scene->setInitialViewpoint(initialVp);

  // apply initial extrusion
  totalPopulation();

  m_sceneView->setArcGISScene(scene);
}

void FeatureLayerExtrusion::popDensity()
{
  // multiply population density by 5000 to make data legible
  RendererSceneProperties props = m_renderer->sceneProperties();
  props.setExtrusionExpression("([POP07_SQMI] * 5000) + 100000");
  m_renderer->setSceneProperties(props);
}

void FeatureLayerExtrusion::totalPopulation()
{
  // divide total population by 10 to make data legible
  RendererSceneProperties props = m_renderer->sceneProperties();
  props.setExtrusionExpression("[POP2007] / 10");
  m_renderer->setSceneProperties(props);
}



