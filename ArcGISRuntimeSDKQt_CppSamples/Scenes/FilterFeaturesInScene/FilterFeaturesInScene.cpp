// [WriteFile Name=FilterFeaturesInScene, Category=Scenes]
// [Legal]
// Copyright 2023 Esri.

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

#include "FilterFeaturesInScene.h"

#include "ArcGISSceneLayer.h"
#include "ArcGISTiledElevationSource.h"
#include "ArcGISVectorTiledLayer.h"
#include "Basemap.h"
#include "Camera.h"
#include "ElevationSourceListModel.h"
#include "Envelope.h"
#include "Error.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "LayerListModel.h"
#include "MapTypes.h"
#include "Point.h"
#include "Polygon.h"
#include "PolygonBuilder.h"
#include "PortalItem.h"
#include "Scene.h"
#include "SceneLayerPolygonFilter.h"
#include "SceneQuickView.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SpatialReference.h"
#include "Surface.h"
#include "SymbolTypes.h"
#include "TaskWatcher.h"

using namespace Esri::ArcGISRuntime;

FilterFeaturesInScene::FilterFeaturesInScene(QObject* parent /* = nullptr */):
  QObject(parent)
{
  // Construct and set the basemap
  ArcGISVectorTiledLayer* vectorTileLayer = new ArcGISVectorTiledLayer(new PortalItem("1e7d1784d1ef4b79ba6764d0bd6c3150", this), this);
  m_osmBuildings = new ArcGISSceneLayer(new PortalItem("ca0470dbbddb4db28bad74ed39949e25", this), this);
  Basemap* basemap = new Basemap(this);
  basemap->baseLayers()->append(vectorTileLayer);
  basemap->baseLayers()->append(m_osmBuildings);
  m_scene = new Scene(basemap, this);

  // Construct and set the scene topography
  Surface* surface = new Surface(this);
  surface->elevationSources()->append(new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this));
  m_scene->setBaseSurface(surface);

  // Construct the detailed buildings scene layer
  m_detailedBuildingsSceneLayer = new ArcGISSceneLayer(QUrl("https://tiles.arcgis.com/tiles/z2tnIkrLQ2BRzr6P/arcgis/rest/services/SanFrancisco_Bldgs/SceneServer"), this);

  connect(m_detailedBuildingsSceneLayer, &ArcGISSceneLayer::doneLoading, this, [this](const Error error)
  {
    if (!error.isEmpty())
    {
      qWarning() << "San Fransisco Buildings Layer failed to load." << error.message() << error.additionalMessage();
      return;
    }

    // Construct a red polygon that shows the extent of the detailed buildings scene layer
    Envelope sfExtent = m_detailedBuildingsSceneLayer->fullExtent();

    PolygonBuilder* builder = new PolygonBuilder(m_sceneView->spatialReference(), this);
    builder->addPoint(sfExtent.xMin(), sfExtent.yMin());
    builder->addPoint(sfExtent.xMax(), sfExtent.yMin());
    builder->addPoint(sfExtent.xMax(), sfExtent.yMax());
    builder->addPoint(sfExtent.xMin(), sfExtent.yMax());

    m_sceneLayerExtentPolygon = builder->toPolygon();

    // Create the SceneLayerPolygonFilter to later apply to the OSM buildings layer
    m_sceneLayerPolygonFilter = new SceneLayerPolygonFilter(
          QList<Polygon>{m_sceneLayerExtentPolygon}, // polygons to filter by
          SceneLayerPolygonFilterSpatialRelationship::Disjoint, // hide all features within the polygons
          this);

    // Create the extent graphic so we can add it later with the detailed buildings scene layer
    SimpleFillSymbol* sfs = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor(Qt::transparent), new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(Qt::red), 5.0f, this), this);
    m_sanFransiscoExtentGraphic = new Graphic(m_sceneLayerExtentPolygon, sfs, this);
  });

  m_detailedBuildingsSceneLayer->load();
}

FilterFeaturesInScene::~FilterFeaturesInScene() = default;

void FilterFeaturesInScene::init()
{
  // Register the scene view for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<FilterFeaturesInScene>("Esri.Samples", 1, 0, "FilterFeaturesInSceneSample");
}

void FilterFeaturesInScene::loadScene()
{
  // Show the detailed buildings scene layer and the extent graphic
  m_scene->operationalLayers()->append(m_detailedBuildingsSceneLayer);
  m_sceneView->graphicsOverlays()->first()->graphics()->append(m_sanFransiscoExtentGraphic);
}

void FilterFeaturesInScene::filterScene()
{
  // Hide buildings within the detailed building extent so they don't clip

  // Initially, the building layer does not have a polygon filter, set it
  if (!m_osmBuildings->polygonFilter())
    m_osmBuildings->setPolygonFilter(m_sceneLayerPolygonFilter);

  // After the scene is reset, the layer will have a polygon filter, but that filter will not have polygons set
  // Add the polygon back to the polygon filter
  else
    m_sceneLayerPolygonFilter->setPolygons({m_sceneLayerExtentPolygon});
}

void FilterFeaturesInScene::reset()
{
  // Remove the detailed buildings layer from the scene
  m_scene->operationalLayers()->clear();

  // Set the OSM buildings polygon filter to an empty list of polygons to clear the filter
  m_osmBuildings->polygonFilter()->setPolygons(QList<Polygon>{});

  // Clear the graphics list in the graphics overlay to remove the red extent boundary graphic
  m_sceneView->graphicsOverlays()->first()->graphics()->clear();
}

SceneQuickView* FilterFeaturesInScene::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void FilterFeaturesInScene::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
    return;

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  // Set the viewpoint to San Fransisco
  m_sceneView->setViewpointCamera(Camera(Point(-122.421, 37.7041, 207), 60, 70, 0));

  // Add a graphics overlay to display the extent graphic
  m_sceneView->graphicsOverlays()->append(new GraphicsOverlay(this));

  emit sceneViewChanged();
}
