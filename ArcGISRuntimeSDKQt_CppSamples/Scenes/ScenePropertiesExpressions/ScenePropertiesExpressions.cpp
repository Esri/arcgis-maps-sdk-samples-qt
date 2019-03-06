// [WriteFile Name=ScenePropertiesExpressions, Category=Scenes]
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

#include "ScenePropertiesExpressions.h"

#include "ArcGISTiledElevationSource.h"
#include "Basemap.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "SimpleMarkerSceneSymbol.h"
#include "SimpleRenderer.h"

using namespace Esri::ArcGISRuntime;

namespace
{
  const QString HEADING("HEADING");
  const QString PITCH("PITCH");
}

ScenePropertiesExpressions::ScenePropertiesExpressions(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(Basemap::imagery(this), this))
{
  // create a new elevation source from Terrain3D REST service
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
        QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);

  m_graphicsOverlay = new GraphicsOverlay(this);
  m_graphicsOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::Relative));

  // Create a SimpleRenderer and set expressions on its scene properties.
  // Then, set the renderer to the graphics overlay with GraphicsOverlay.setRenderer(renderer).
  SimpleRenderer* renderer3D = new SimpleRenderer(this);
  RendererSceneProperties renderProperties = renderer3D->sceneProperties();
  renderProperties.setHeadingExpression(QString("[%1]").arg(HEADING));
  renderProperties.setPitchExpression(QString("[%1]").arg(PITCH));
  renderer3D->setSceneProperties(renderProperties);
  m_graphicsOverlay->setRenderer(renderer3D);
}

ScenePropertiesExpressions::~ScenePropertiesExpressions() = default;

void ScenePropertiesExpressions::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<ScenePropertiesExpressions>("Esri.Samples", 1, 0, "ScenePropertiesExpressionsSample");
}

SceneQuickView* ScenePropertiesExpressions::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void ScenePropertiesExpressions::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
  {
    return;
  }

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  // create a camera
  const double latitude = 32.09;
  const double longitude = -118.71;
  const double altitude = 100000.0;
  const double heading = 0.0;
  const double pitch = 45.0;
  const double roll = 0.0;
  const int coneDimension = 10000;

  Camera camera(latitude - 1.0, // place the camera arbitrarily south of the cone
                longitude,
                altitude * 2, // place the camera arbitrarily higher than the cone
                heading,
                pitch,
                roll);

  // set the viewpoint
  m_sceneView->setViewpointCameraAndWait(camera);

  // add the graphics overlay to the scene view
  m_sceneView->graphicsOverlays()->append(m_graphicsOverlay);

  // create a scene symbol based on the current type
  SimpleMarkerSceneSymbol* smss = new SimpleMarkerSceneSymbol(SimpleMarkerSceneSymbolStyle::Cone, QColor("red"), 200, 200, 200, SceneSymbolAnchorPosition::Center, this);
  smss->setWidth(coneDimension);
  smss->setDepth(coneDimension);
  smss->setHeight(coneDimension * 2);

  // create a graphic using the symbol above and a point location
  m_graphic = new Graphic(Point(longitude, latitude, altitude, m_sceneView->spatialReference()), smss, this);
  m_graphic->attributes()->insertAttribute(HEADING, 0);
  m_graphic->attributes()->insertAttribute(PITCH, 0);

  // add the graphic to the graphics overlay
  m_graphicsOverlay->graphics()->append(m_graphic);

  emit sceneViewChanged();
}

void ScenePropertiesExpressions::setPitch(double pitchInDegrees)
{
  m_graphic->attributes()->replaceAttribute(PITCH, pitchInDegrees);

  emit pitchChanged();
}

double ScenePropertiesExpressions::pitch() const
{
  return m_graphic->attributes()->attributeValue(PITCH).toDouble();
}

void ScenePropertiesExpressions::setHeading(double headingInDegrees)
{
  m_graphic->attributes()->replaceAttribute(HEADING, headingInDegrees);

  emit headingChanged();
}

double ScenePropertiesExpressions::heading() const
{
  return m_graphic->attributes()->attributeValue(HEADING).toDouble();
}
