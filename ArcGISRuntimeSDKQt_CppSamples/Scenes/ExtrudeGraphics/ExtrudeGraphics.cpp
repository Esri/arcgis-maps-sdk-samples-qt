// [WriteFile Name=ExtrudeGraphics, Category=Scenes]
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

#include "ExtrudeGraphics.h"

#include "Scene.h"
#include "SceneQuickView.h"
#include "Basemap.h"
#include "ArcGISTiledElevationSource.h"
#include "SimpleRenderer.h"
#include "PointBuilder.h"
#include "Point.h"
#include "PolygonBuilder.h"
#include "Polygon.h"
#include "SimpleFillSymbol.h"

#include "cmath"
#include "time.h"

using namespace Esri::ArcGISRuntime;

ExtrudeGraphics::ExtrudeGraphics(QQuickItem* parent) :
    QQuickItem(parent),
    m_scene(nullptr),
    m_sceneView(nullptr),
    m_elevationSourceUrl("http://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"),
    m_maxZ(1000),
    m_size(0.01)
{
    srand(time(nullptr));
}

ExtrudeGraphics::~ExtrudeGraphics()
{
}

void ExtrudeGraphics::componentComplete()
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
    ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(m_elevationSourceUrl, this);
    // add the elevation source to the scene to display elevation
    m_scene->baseSurface()->elevationSources()->append(elevationSource);

    // create a camera
    Camera camera(28.4, 83.9, 10010.0, 10.0, 80.0, 300.0);
    // set the viewpoint
    m_sceneView->setViewpointCamera(camera);

    // graphics location
    double lon = camera.location().x() - 0.03;
    double lat = camera.location().y() + 0.2;

    GraphicsOverlay* graphicsOverlay = new GraphicsOverlay(this);

    // set renderer with extrusion property
    SimpleRenderer* renderer = new SimpleRenderer(this);
    RendererSceneProperties props = renderer->sceneProperties();
    props.setExtrusionMode(ExtrusionMode::BaseHeight);
    props.setExtrusionExpression("height");
    graphicsOverlay->setRenderer(renderer);

    // setup graphic locations
    QList<Point> pointsList;
    for (auto i = 0; i <= 100; i++)
    {
        Point point(i / 10 * (m_size * 2) + lon, i % 10 * (m_size * 2) + lat, m_sceneView->spatialReference());
        pointsList.append(point);
    }

    foreach (auto point, pointsList)
    {
        // create a list of colors to ramdomly pick from
        QList<QColor>colors;
        colors << QColor("white")
               << QColor("green")
               << QColor("blue")
               << QColor("turquoise")
               << QColor("purple")
               << QColor("black")
               << QColor("red");

        // create a random z value
        int randNum = rand() % 6 + 1;
        double z = m_maxZ * randNum;

        // create a list of points
        QList<Point> points;
        points << Point(point.x(), point.y(), z)
               << Point(point.x() + m_size, point.y(), z)
               << Point(point.x() + m_size, point.y() + m_size, z)
               << Point(point.x(), point.y() + m_size, z);

        // create a new fill symbol to symbolize the graphic
        SimpleFillSymbol* sfs = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, colors[randNum], this);
        // create a new graphic
        Graphic* graphic = new Graphic(createPolygonFromPoints(points), sfs, this);
        // add a height attribute to the graphic using the attribute list model
        // the extrusion will be applied to this attribute. See the expression above
        graphic->attributes()->insertAttribute("height", z);
        // add the graphic to the graphic overlay
        graphicsOverlay->graphics()->append(graphic);
    }

    m_sceneView->graphicsOverlays()->append(graphicsOverlay);
}

// helper to create polygon from points
Esri::ArcGISRuntime::Polygon ExtrudeGraphics::createPolygonFromPoints(QList<Point> points)
{
    Esri::ArcGISRuntime::Polygon polygon;
    if (points.length() == 0)
      return polygon;

    // create a polygon builder
    PolygonBuilder* pb = new PolygonBuilder(m_sceneView->spatialReference(), this);
    foreach (auto point, points)
    {
      // add each point to the builder object
      pb->addPoint(point);
    }
    return polygon = pb->toGeometry();
}

