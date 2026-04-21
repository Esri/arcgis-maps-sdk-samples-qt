// [WriteFile Name=FilterFeaturesInScene, Category=Scenes]
// [Legal]
// Copyright 2023 Esri.
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "FilterFeaturesInScene.h"

// ArcGIS Maps SDK headers
#include "ArcGISSceneLayer.h"
#include "ArcGISTiledElevationSource.h"
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
#include "Scene.h"
#include "SceneLayerPolygonFilter.h"
#include "SceneQuickView.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SpatialReference.h"
#include "Surface.h"
#include "SymbolTypes.h"

// Qt headers
#include <QFuture>

using namespace Esri::ArcGISRuntime;

FilterFeaturesInScene::FilterFeaturesInScene(QObject* parent /* = nullptr */) :
  QObject(parent),
  m_scene(new Scene(this))
{
  // Construct and set the scene topography
  Surface* surface = new Surface(this);
  surface->elevationSources()->append(
    new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this));
  m_scene->setBaseSurface(surface);

  // Construct a basemap with the ArcGIS Navigation 3D Basemap then set it on the Scene
  Basemap* basemap = new Basemap(QUrl("https://arcgisruntime.maps.arcgis.com/home/item.html?id=00a5f468dda941d7bf0b51c144aae3f0"), this);
  m_scene->setBasemap(basemap);

  connect(basemap, &Basemap::doneLoading, this, [basemap, this](const Error& error)
  {
    if (!error.isEmpty())
    {
      qWarning() << "Basemap failed to load." << error.message() << error.additionalMessage();
      return;
    }

    // Loop through all layers in the basemap and look for the buildings layer
    for (Layer* baseLayer : *basemap->baseLayers())
    {
      // As of 2025, the 3D basemap contains a layer called "Esri 3D Buildings" -- we'll search for a layer
      // that contains 'building' in case the name is ever updated
      if (baseLayer->name().contains("building", Qt::CaseInsensitive))
      {
        m_3dBuildings = dynamic_cast<ArcGISSceneLayer*>(baseLayer);
        if (m_3dBuildings)
        {
          // Buildings layer found, we can return out of the lambda
          return;
        }
      }
    }
    if (!m_3dBuildings)
    {
      qWarning() << "Buildings layer not found in base layers. Please check that your basemap"
                 << "contains an ArcGIS Scene Layer with 'building' in the name";
    }
  });

  // Construct the detailed buildings scene layer
  m_sanFrancisco3DOSceneLayer =
    new ArcGISSceneLayer(QUrl("https://tiles.arcgis.com/tiles/z2tnIkrLQ2BRzr6P/arcgis/rest/services/SanFrancisco_Bldgs/SceneServer"), this);

  // Initially hide the detailed buildings so they don't clip into the 3D basemap buildings
  m_sanFrancisco3DOSceneLayer->setVisible(false);

  // When the detailed building scene layer finishes loading, get its extent for the polygon filter
  connect(m_sanFrancisco3DOSceneLayer, &ArcGISSceneLayer::doneLoading, this, [this](const Error& error)
  {
    if (!error.isEmpty())
    {
      qWarning() << "San Francisco Buildings Layer failed to load." << error.message() << error.additionalMessage();
      return;
    }

    // Construct a red polygon that shows the extent of the detailed buildings scene layer
    const Envelope sfExtent = m_sanFrancisco3DOSceneLayer->fullExtent();

    PolygonBuilder* polygonBuilder = new PolygonBuilder(m_sceneView->spatialReference(), this);
    polygonBuilder->addPoint(sfExtent.xMin(), sfExtent.yMin());
    polygonBuilder->addPoint(sfExtent.xMax(), sfExtent.yMin());
    polygonBuilder->addPoint(sfExtent.xMax(), sfExtent.yMax());
    polygonBuilder->addPoint(sfExtent.xMin(), sfExtent.yMax());

    m_sceneLayerExtentPolygon = polygonBuilder->toPolygon();

    // Create the SceneLayerPolygonFilter to later apply to the OSM buildings layer
    m_sceneLayerPolygonFilter =
      new SceneLayerPolygonFilter(QList<Polygon>{m_sceneLayerExtentPolygon}, // polygons to filter by
                                  SceneLayerPolygonFilterSpatialRelationship::Disjoint, // hide all features within the polygons
                                  this);

    // Create the extent graphic so we can add it later with the detailed buildings scene layer
    SimpleFillSymbol* simpleFillSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor(Qt::transparent),
                                                              new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(Qt::red), 5.0f, this), this);
    m_sanFranciscoExtentGraphic = new Graphic(m_sceneLayerExtentPolygon, simpleFillSymbol, this);
  });

  m_scene->operationalLayers()->append(m_sanFrancisco3DOSceneLayer);
}

FilterFeaturesInScene::~FilterFeaturesInScene() = default;

void FilterFeaturesInScene::init()
{
  // Register the scene view for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<FilterFeaturesInScene>("Esri.Samples", 1, 0, "FilterFeaturesInSceneSample");
}

// Show the detailed buildings scene layer
void FilterFeaturesInScene::showDetailedBuildings()
{
  if (!m_sanFrancisco3DOSceneLayer)
  {
    qWarning() << "Detailed San Francisco Buildings layer not loaded";
    return;
  }
  m_sanFrancisco3DOSceneLayer->setVisible(true);
}

// Hide buildings within the detailed building extent so they don't clip
void FilterFeaturesInScene::filterScene()
{
  if (!m_sceneView || !m_sceneView->graphicsOverlays()->first())
  {
    return;
  }

  // Show the extent graphic to visualize the polygon filter
  m_sceneView->graphicsOverlays()->first()->graphics()->append(m_sanFranciscoExtentGraphic);

  if (!m_3dBuildings || !m_sceneLayerPolygonFilter)
  {
    return;
  }

  // Initially, the building layer does not have a polygon filter, set it
  if (!m_3dBuildings->polygonFilter())
  {
    m_3dBuildings->setPolygonFilter(m_sceneLayerPolygonFilter);
  }

  // After the scene is reset, the layer will have a polygon filter, but that filter will not have polygons set
  // Add the polygon back to the polygon filter
  else
  {
    m_sceneLayerPolygonFilter->setPolygons({m_sceneLayerExtentPolygon});
  }
}

void FilterFeaturesInScene::reset()
{
  if (!m_sanFrancisco3DOSceneLayer)
  {
    return;
  }

  // Hide the detailed buildings layer from the scene
  m_sanFrancisco3DOSceneLayer->setVisible(false);

  if (!m_3dBuildings || !m_3dBuildings->polygonFilter())
  {
    return;
  }
  // Set the 3D buildings polygon filter to an empty list of polygons to clear the filter
  m_3dBuildings->polygonFilter()->setPolygons(QList<Polygon>{});

  if (!m_sceneView || !m_sceneView->graphicsOverlays()->first())
  {
    return;
  }

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
  {
    return;
  }

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  // Set the viewpoint to San Francisco
  m_sceneView->setViewpointCameraAsync(Camera(Point(-122.421, 37.7041, 207), 60, 70, 0));

  // Add a graphics overlay to display the extent graphic
  m_sceneView->graphicsOverlays()->append(new GraphicsOverlay(this));

  emit sceneViewChanged();
}
