// [WriteFile Name=Symbols, Category=Scenes]
// [Legal]
// Copyright 2016 Esri.

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

#include "Symbols.h"

#include "Scene.h"
#include "SceneQuickView.h"
#include "Basemap.h"
#include "ArcGISTiledElevationSource.h"
#include "SimpleMarkerSceneSymbol.h"

using namespace Esri::ArcGISRuntime;

Symbols::Symbols(QQuickItem* parent) :
  QQuickItem(parent)
{
}

Symbols::~Symbols() = default;

void Symbols::init()
{
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<Symbols>("Esri.Samples", 1, 0, "SymbolsSample");
}

void Symbols::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML SceneView component
  m_sceneView = findChild<SceneQuickView*>("sceneView");

  // create a new basemap instance
  Basemap* basemap = Basemap::imagery(this);
  // create a new scene instance
  m_scene = new Scene(basemap, this);
  // set scene on the scene view
  m_sceneView->setArcGISScene(m_scene);

  // create a new elevation source
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);
  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);

  // create a camera
  Camera camera(34, 45, 6000, 0, 0, 0);
  // set the viewpoint
  m_sceneView->setViewpointCameraAndWait(camera);

  // create a graphics overlay
  GraphicsOverlay* graphicsOverlay = new GraphicsOverlay(this);
  graphicsOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::Absolute));
  // add the graphics overlay to the scene view
  m_sceneView->graphicsOverlays()->append(graphicsOverlay);

  // create a map of the symbol styles and color
  QMap<SimpleMarkerSceneSymbolStyle, QColor> symbolStyles;
  symbolStyles.insert(SimpleMarkerSceneSymbolStyle::Cone, QColor("red"));
  symbolStyles.insert(SimpleMarkerSceneSymbolStyle::Cube, QColor("white"));
  symbolStyles.insert(SimpleMarkerSceneSymbolStyle::Cylinder, QColor("purple"));
  symbolStyles.insert(SimpleMarkerSceneSymbolStyle::Diamond, QColor("turquoise"));
  symbolStyles.insert(SimpleMarkerSceneSymbolStyle::Sphere, QColor("blue"));
  symbolStyles.insert(SimpleMarkerSceneSymbolStyle::Tetrahedron, QColor("yellow"));
  QMapIterator<SimpleMarkerSceneSymbolStyle, QColor> i(symbolStyles);

  constexpr double x = 44.975;
  constexpr double y = 34.0;
  constexpr double z = 500.0;
  int counter = 0;

  // iterate over the map
  while (i.hasNext())
  {
    i.next();
    // create a scene symbol based on the current type
    SimpleMarkerSceneSymbol* smss = new SimpleMarkerSceneSymbol(i.key(), i.value(), 200, 200, 200, SceneSymbolAnchorPosition::Center, this);
    // create a graphic using the symbol above and a point location
    Graphic* graphic = new Graphic(Point(x + 0.01 * counter++, y, z, m_sceneView->spatialReference()), smss, this);
    // add the graphic to the graphics overlay
    graphicsOverlay->graphics()->append(graphic);
  }
}
