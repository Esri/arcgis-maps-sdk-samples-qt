// [WriteFile Name=ViewshedGeoElement, Category=Analysis]
// [Legal]
// Copyright 2017 Esri.
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
#include "ViewshedGeoElement.h"

// ArcGIS Maps SDK headers
#include "AnalysisListModel.h"
#include "AnalysisOverlay.h"
#include "AnalysisOverlayListModel.h"
#include "AngularUnit.h"
#include "ArcGISSceneLayer.h"
#include "ArcGISTiledElevationSource.h"
#include "AttributeListModel.h"
#include "ElevationSourceListModel.h"
#include "GeoElementViewshed.h"
#include "GeodeticDistanceResult.h"
#include "GeometryEngine.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "LayerListModel.h"
#include "LayerSceneProperties.h"
#include "LinearUnit.h"
#include "MapTypes.h"
#include "ModelSceneSymbol.h"
#include "OrbitGeoElementCameraController.h"
#include "Point.h"
#include "RendererSceneProperties.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "SceneViewTypes.h"
#include "SimpleRenderer.h"
#include "SpatialReference.h"
#include "Surface.h"
#include "SymbolTypes.h"

// Qt headers
#include <QList>
#include <QStandardPaths>
#include <QString>
#include <QTimer>
#include <QUrl>
#include <QVariant>
#include <QtCore/qglobal.h>

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
  QString defaultDataPath()
  {
    QString dataPath;

  #ifdef Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  #else
    dataPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
  #endif

    return dataPath;
  }
} // namespace

ViewshedGeoElement::ViewshedGeoElement(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void ViewshedGeoElement::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<ViewshedGeoElement>("Esri.Samples", 1, 0, "ViewshedGeoElementSample");
}

void ViewshedGeoElement::componentComplete()
{
  QQuickItem::componentComplete();

  // Create a scene and give it to the SceneView
  m_sceneView = findChild<SceneQuickView*>("sceneView");
  Scene* scene = new Scene(BasemapStyle::ArcGISImageryStandard, this);

  // Create a surface
  Surface* surface = new Surface(this);
  surface->elevationSources()->append(
        new ArcGISTiledElevationSource(
          QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"),
          this));
  scene->setBaseSurface(surface);

  // Add a SceneLayer
  ArcGISSceneLayer* sceneLayer = new ArcGISSceneLayer(QUrl("https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/Buildings_Brest/SceneServer/layers/0"), this);
  scene->operationalLayers()->append(sceneLayer);

  // Add an AnalysisOverlay
  m_analysisOverlay = new AnalysisOverlay(this);
  m_sceneView->analysisOverlays()->append(m_analysisOverlay);

  // Add a GraphicsOverlay
  createGraphicsOverlay();

  // Create a Graphic
  createGraphic();

  // Create the GeoElementViewshed
  const double horizontalAngle = 90.0;
  const double verticalAngle = 25.0;
  const double minDistance = 5.0;
  const double maxDistance = 250.0;
  const double headingOffset = 0.0;
  const double pitchOffset = 0.0;
  m_viewshed = new GeoElementViewshed(m_tank, horizontalAngle,
                                      verticalAngle, minDistance,
                                      maxDistance, headingOffset,
                                      pitchOffset, this);
  m_viewshed->setOffsetY(0.5);
  m_viewshed->setOffsetZ(0.5);
  m_analysisOverlay->analyses()->append(m_viewshed);

  // Add the Scene to the SceneView
  m_sceneView->setArcGISScene(scene);

  // create the camera controller to follow the graphic
  OrbitGeoElementCameraController* followingController = new OrbitGeoElementCameraController(m_tank, 200.0, this);
  followingController->setCameraPitchOffset(45.0);
  m_sceneView->setCameraController(followingController);

  // Create a Timer
  m_timer = new QTimer(this);
  m_timer->setInterval(100);
  connect(m_timer, &QTimer::timeout, this, &ViewshedGeoElement::animate);

  // connect to the mouse clicked signal
  connect(m_sceneView, &SceneQuickView::mouseClicked, this, [this](QMouseEvent& event)
  {
    m_waypoint = m_sceneView->screenToBaseSurface(event.position().x(), event.position().y());
    m_timer->start();
  });
}

void ViewshedGeoElement::createGraphicsOverlay()
{
  // Add a GraphicsOverlay
  m_graphicsOverlay = new GraphicsOverlay(this);
  m_sceneView->graphicsOverlays()->append(m_graphicsOverlay);

  // Set the SurfacePlacement
  LayerSceneProperties sceneProperties = m_graphicsOverlay->sceneProperties();
  sceneProperties.setSurfacePlacement(SurfacePlacement::Relative);
  m_graphicsOverlay->setSceneProperties(sceneProperties);

  // Set a Renderer
  SimpleRenderer* simpleRenderer = new SimpleRenderer(this);
  const QString headingExpression = QString("[%1]").arg(m_headingAttr);
  simpleRenderer->setSceneProperties(RendererSceneProperties(headingExpression, "", ""));
  m_graphicsOverlay->setRenderer(simpleRenderer);
}

void ViewshedGeoElement::createGraphic()
{
  // Create the Graphic Point
  const double x = -4.508708007847015;
  const double y = 48.38823243446344;
  const double z = 0;
  const Point tankPoint(x, y, z, SpatialReference(4326));
  const float scale = 10;

  // Create the Graphic Symbol
  ModelSceneSymbol* sceneSymbol = new ModelSceneSymbol(QUrl(defaultDataPath() + "/ArcGIS/Runtime/Data/3D/bradley_low_3ds/bradle.3ds"), scale, this);
  sceneSymbol->setAnchorPosition(SceneSymbolAnchorPosition::Bottom);
  sceneSymbol->setHeading(90.0f);

  // Create the Graphic
  QVariantMap attr;
  attr[m_headingAttr] = 150.0;
  m_tank = new Graphic(tankPoint, attr, sceneSymbol, this);
  m_graphicsOverlay->graphics()->append(m_tank);
}

void ViewshedGeoElement::animate()
{
  if (m_waypoint.isEmpty())
    return;

  // get current location and distance from waypoint
  Point location = geometry_cast<Point>(m_tank->geometry());
  const GeodeticDistanceResult distance = GeometryEngine::distanceGeodetic(location, m_waypoint,
                                                                           LinearUnit(m_linearUnit),
                                                                           AngularUnit(m_angularUnit),
                                                                           m_curveType);

  // move toward waypoint based on speed and update orientation
  location = GeometryEngine::moveGeodetic(QList<Point>{location}, 1.0,
                                          LinearUnit(m_linearUnit),
                                          distance.azimuth1(),
                                          AngularUnit(m_angularUnit),
                                          m_curveType).at(0);
  m_tank->setGeometry(location);

  // update the heading
  const double heading = m_tank->attributes()->attributeValue(m_headingAttr).toDouble();
  const QString newHeading = QString::number(heading + (distance.azimuth1() - heading) / 10);
  m_tank->attributes()->replaceAttribute(m_headingAttr, newHeading);

  // reached waypoint
  if (distance.distance() <= 5)
  {
    m_waypoint = Point();
    m_timer->stop();
  }
}
