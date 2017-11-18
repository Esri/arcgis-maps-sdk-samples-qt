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
#include "OrbitLocationCameraController.h"
#include "Point.h"
#include "SimpleLineSymbol.h"
#include "SimpleFillSymbol.h"
#include "SimpleRenderer.h"
#include "RendererSceneProperties.h"


using namespace Esri::ArcGISRuntime;

bool showTotalPopulation = true;
double distance = 20000000;
double heading = 0;
double pitch = 55;
double roll = 0;

FeatureLayerExtrusion::FeatureLayerExtrusion(QQuickItem* parent /* = nullptr */):
    QQuickItem(parent)
{
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
    m_featureTable = new ServiceFeatureTable(QUrl("http://sampleserver6.arcgisonline.com/arcgis/rest/services/Census/MapServer/3"),this);

    // load all fields in the service feature table
    QueryParameters queryParams = QueryParameters();
    queryParams.setWhereClause("1+1");
    m_featureTable->queryFeatures(queryParams, QueryFeatureFields::LoadAll);

    // add the service feature table to a feature layer
    m_featureLayer = new FeatureLayer(m_featureTable, this);

    // set the feature layer to render dynamically to allow extrusion
    m_featureLayer->setRenderingMode(FeatureRenderingMode::Dynamic);

    // define line and fill symbols for a simple renderer
    m_lineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("Black"), 1.0f);
    m_fillSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("Blue"), m_lineSymbol);
    renderer = new SimpleRenderer(m_fillSymbol, this);

    // set renderer extrusion mode to base height, which includes base height of each vertex in calculating z values
    RendererSceneProperties props = renderer->sceneProperties();
    props.setExtrusionMode(ExtrusionMode::BaseHeight);
    renderer->setSceneProperties(props);

    // set the simple renderer to the feature layer
    m_featureLayer->setRenderer(renderer);


    // Create a scene and give it to the SceneView
    m_sceneView = findChild<SceneQuickView*>("sceneView");
    Scene* scene = new Scene(Basemap::imagery(this), this);
    Surface* surface = new Surface(this);
    surface->elevationSources()->append(
                new ArcGISTiledElevationSource(
                    QUrl("http://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"),
                    this));
    scene->setBaseSurface(surface);



    scene->operationalLayers()->append(m_featureLayer);
    m_sceneView->setArcGISScene(scene);

    m_lookAtPoint = Point(-10974490, 4814376, 0, SpatialReference::webMercator());

    m_camera = Camera(m_lookAtPoint, distance, heading, pitch, roll);

    m_orbitController = new OrbitLocationCameraController(m_lookAtPoint, distance);

    m_sceneView->setCameraController(m_orbitController);
    m_sceneView->setViewpointCamera(m_camera);
}

void FeatureLayerExtrusion::popDensity()
{
    if (showTotalPopulation) {
        // divide total population by 10 to make data legible
        RendererSceneProperties props = renderer->sceneProperties();
        props.setExtrusionExpression("[POP2007] / 10");
        renderer->setSceneProperties(props);
        showTotalPopulation = false;
      } else {
        // multiple population density by 5000 to make data legible
        RendererSceneProperties props = renderer->sceneProperties();
        props.setExtrusionExpression("[POP07_SQMI] * 5000");
        renderer->setSceneProperties(props);
        showTotalPopulation = true;
      }

}

