// [WriteFile Name=FilterFeaturesInSceneView, Category=Scenes]
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

#include "FilterFeaturesInSceneView.h"

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

FilterFeaturesInSceneView::FilterFeaturesInSceneView(QObject* parent /* = nullptr */):
  QObject(parent)
{
  ArcGISVectorTiledLayer* vectorTileLayer = new ArcGISVectorTiledLayer(new PortalItem("1e7d1784d1ef4b79ba6764d0bd6c3150", this), this);
  m_osmBuildings = new ArcGISSceneLayer(new PortalItem("ca0470dbbddb4db28bad74ed39949e25", this), this);
  Basemap* basemap = new Basemap(this);
  basemap->baseLayers()->append(vectorTileLayer);
  basemap->baseLayers()->append(m_osmBuildings);

  m_scene = new Scene(basemap, this);

  Surface* surface = new Surface(this);
  surface->elevationSources()->append(new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this));

  m_scene->setBaseSurface(surface);

  m_sceneLayer = new ArcGISSceneLayer(QUrl("https://tiles.arcgis.com/tiles/z2tnIkrLQ2BRzr6P/arcgis/rest/services/SanFrancisco_Bldgs/SceneServer"), this);

  connect(m_sceneLayer, &ArcGISSceneLayer::doneLoading, this, [this](const Error error)
  {
    if (!error.isEmpty())
    {
      qWarning() << "San Fransisco Buildings Layer failed to load." << error.message() << error.additionalMessage();
      return;
    }

    Envelope sfExtent = m_sceneLayer->fullExtent();

    PolygonBuilder* builder = new PolygonBuilder(m_sceneView->spatialReference(), this);
    builder->addPoint(sfExtent.xMin(), sfExtent.yMin());
    builder->addPoint(sfExtent.xMax(), sfExtent.yMin());
    builder->addPoint(sfExtent.xMax(), sfExtent.yMax());
    builder->addPoint(sfExtent.xMin(), sfExtent.yMax());

    m_sceneLayerExtentPolygon = builder->toPolygon();

    SimpleFillSymbol* sfs = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor(Qt::transparent), new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(Qt::red), 5.0f, this), this);
    m_sanFransiscoExtentGraphic = new Graphic(m_sceneLayerExtentPolygon, sfs, this);
  });

  m_sceneLayer->load();
}

FilterFeaturesInSceneView::~FilterFeaturesInSceneView() = default;

void FilterFeaturesInSceneView::init()
{
  // Register the scene view for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<FilterFeaturesInSceneView>("Esri.Samples", 1, 0, "FilterFeaturesInSceneViewSample");
}

void FilterFeaturesInSceneView::loadScene()
{
  m_scene->operationalLayers()->append(m_sceneLayer);
  m_sceneView->graphicsOverlays()->first()->graphics()->append(m_sanFransiscoExtentGraphic);
}

void FilterFeaturesInSceneView::filterScene()
{
  m_osmBuildings->setPolygonFilter(
        new SceneLayerPolygonFilter(QList<Polygon>{m_sceneLayerExtentPolygon}, SceneLayerPolygonFilterSpatialRelationship::Disjoint, this));
}

void FilterFeaturesInSceneView::reset()
{
  m_scene->operationalLayers()->clear();

  if (m_osmBuildings->polygonFilter())
    m_osmBuildings->polygonFilter()->setPolygons(QList<Polygon>{});

  m_sceneView->graphicsOverlays()->first()->graphics()->clear();
}

SceneQuickView* FilterFeaturesInSceneView::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void FilterFeaturesInSceneView::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
    return;

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  m_sceneView->setViewpointCamera(Camera(Point(-122.421, 37.7041, 207), 60, 70, 0));

  m_sceneView->graphicsOverlays()->append(new GraphicsOverlay(this));

  emit sceneViewChanged();
}
