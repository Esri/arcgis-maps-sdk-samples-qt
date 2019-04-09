// [WriteFile Name=Animate3DSymbols, Category=Scenes]
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

#include "Animate3DSymbols.h"

#include "ArcGISTiledElevationSource.h"
#include "Camera.h"
#include "DistanceCompositeSceneSymbol.h"
#include "GlobeCameraController.h"
#include "GraphicsOverlay.h"
#include "Map.h"
#include "MapQuickView.h"
#include "ModelSceneSymbol.h"
#include "OrbitGeoElementCameraController.h"
#include "PointCollection.h"
#include "Polyline.h"
#include "PolylineBuilder.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "SceneViewTypes.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleMarkerSceneSymbol.h"
#include "SimpleRenderer.h"
#include "SpatialReference.h"

#include "MissionData.h"

#include <QDir>
#include <QFileInfo>
#include <QStringListModel>
#include <QQmlProperty>

using namespace Esri::ArcGISRuntime;

const QString Animate3DSymbols::HEADING = QStringLiteral("heading");
const QString Animate3DSymbols::ROLL = QStringLiteral("roll");
const QString Animate3DSymbols::PITCH = QStringLiteral("pitch");
const QString Animate3DSymbols::ANGLE = QStringLiteral("angle");

Animate3DSymbols::Animate3DSymbols(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_missionsModel(new QStringListModel({QStringLiteral("Grand Canyon"),
                                        QStringLiteral("Hawaii"),
                                        QStringLiteral("Pyrenees"),
                                        QStringLiteral("Snowdon")},
                                       this)),
  m_missionData(new MissionData())
{
}

Animate3DSymbols::~Animate3DSymbols() = default;

void Animate3DSymbols::init()
{
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<Animate3DSymbols>("Esri.Samples", 1, 0, "Animate3DSymbolsSample");
}

void Animate3DSymbols::componentComplete()
{
  QQuickItem::componentComplete();

  // get the data path
  m_dataPath = QQmlProperty::read(this, "dataPath").toString();

  // find QML SceneView component
  m_sceneView = findChild<SceneQuickView*>("sceneView");
  // create a new scene instance
  Scene* scene = new Scene(Basemap::imagery(this), this);

  // set scene on the scene view
  m_sceneView->setArcGISScene(scene);

  // for use when not in following mode
  m_globeController = new GlobeCameraController(this);

  // create a new elevation source
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
    QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  scene->baseSurface()->elevationSources()->append(elevationSource);

  // create a new graphics overlay and add it to the sceneview
  GraphicsOverlay* sceneOverlay = new GraphicsOverlay(this);
  sceneOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::Absolute));
  m_sceneView->graphicsOverlays()->append(sceneOverlay);

  SimpleRenderer* renderer3D = new SimpleRenderer(this);
  RendererSceneProperties renderProperties = renderer3D->sceneProperties();
  renderProperties.setHeadingExpression(QString("[%1]").arg(HEADING));
  renderProperties.setPitchExpression(QString("[%1]").arg(PITCH));
  renderProperties.setRollExpression(QString("[%1]").arg(ROLL));
  renderer3D->setSceneProperties(renderProperties);
  sceneOverlay->setRenderer(renderer3D);

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setAttributionTextVisible(false);

  // set up mini map
  Map* map = new Map(Basemap::imagery(this), this);
  m_mapView->setMap(map);

  // create a graphics overlay for the mini map
  GraphicsOverlay* mapOverlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(mapOverlay);

  // set up route graphic
  createRoute2d(mapOverlay);

  // set up overlay 2D graphic
  createModel2d(mapOverlay);
}

void Animate3DSymbols::setMissionFrame(int newFrame)
{
  if (!m_missionData||
     newFrame < 0 ||
     m_frame == newFrame)
    return;

  m_frame = newFrame;
  emit missionFrameChanged();
}

void Animate3DSymbols::animate()
{
  if (!m_missionData)
    return;

  if (missionFrame() < missionSize())
  {
    // get the data for this stage in the mission
    const MissionData::DataPoint& dp = m_missionData->dataAt(missionFrame());

    // move 3D graphic to the new position
    m_graphic3d->setGeometry(dp.m_pos);
    // update attribute expressions to immediately update rotation
    m_graphic3d->attributes()->replaceAttribute(HEADING, dp.m_heading);
    m_graphic3d->attributes()->replaceAttribute(PITCH, dp.m_pitch);
    m_graphic3d->attributes()->replaceAttribute(ROLL, dp.m_roll);

    // move 2D graphic to the new position
    m_graphic2d->setGeometry(dp.m_pos);
    m_symbol2d->setAngle(dp.m_heading);
  }

  // increment the frame count
  emit nextFrameRequested();
}

void Animate3DSymbols::changeMission(const QString &missionNameStr)
{
  setMissionFrame(0);

  // read the mission data from the samples .csv files
  QString formattedname = missionNameStr;
  m_missionData->parse(QUrl(m_dataPath).toLocalFile() + "/Missions/" + formattedname.remove(" ") + ".csv");

  // if the mission was loaded successfully, move to the start position
  if (missionReady())
  {
    // create a polyline representing the route for the mission
    PolylineBuilder* routeBldr = new PolylineBuilder(SpatialReference::wgs84(), this);
    for (int i = 0; i < missionSize(); ++i)
    {
      const MissionData::DataPoint& dp = m_missionData->dataAt(i);
      routeBldr->addPoint(dp.m_pos);
    }

    // set the polyline as a graphic on the mapView
    m_routeGraphic->setGeometry(routeBldr->toGeometry());

    m_mapView->setViewpointAndWait(Viewpoint(m_routeGraphic->geometry()));
    createGraphic3D();
  }

  emit missionReadyChanged();
  emit missionSizeChanged();
}

QAbstractListModel *Animate3DSymbols::missionsModel()
{
  return m_missionsModel;
}

void Animate3DSymbols::setZoom(double zoomDist)
{
  if (m_followingController)
  {
    m_followingController->setCameraDistance(zoomDist);
    emit zoomChanged();
  }
}

void Animate3DSymbols::setAngle(double angle)
{
  if (m_followingController)
  {
    m_followingController->setCameraPitchOffset(angle);
    emit angleChanged();
  }
}

void Animate3DSymbols::createModel2d(GraphicsOverlay* mapOverlay)
{
  if (m_symbol2d || m_graphic2d)
    return;

  // get the mission data for the frame
  const MissionData::DataPoint& dp = m_missionData->dataAt(missionFrame());

  // create a blue triangle symbol to represent the plane on the mini map
  m_symbol2d = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Triangle, Qt::blue, 10, this);
  m_symbol2d->setAngle(dp.m_heading);

  // create a graphic with the symbol
  m_graphic2d = new Graphic(dp.m_pos, m_symbol2d, this);

  mapOverlay->graphics()->append(m_graphic2d);
}

void Animate3DSymbols::createRoute2d(GraphicsOverlay* mapOverlay)
{
  // Create a 2d graphic of a solid red line to represen the route of the mission
  SimpleLineSymbol* routeSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::red, 1, this);
  m_routeGraphic = new Graphic(this);
  m_routeGraphic->setSymbol(routeSymbol);
  mapOverlay->graphics()->append(m_routeGraphic);
}

void Animate3DSymbols::createGraphic3D()
{
  if (!missionReady())
    return;

  // create the ModelSceneSymbol to be animated in the 3d view
  if (!m_model3d)
    m_model3d = new ModelSceneSymbol(QUrl(m_dataPath + "/Bristol/Collada/Bristol.dae"), 10.0f, this);

  // get the mission data for the frame
  const MissionData::DataPoint& dp = m_missionData->dataAt(missionFrame());

  if (!m_graphic3d)
  {
    // create a graphic using the model symbol
    m_graphic3d = new Graphic(dp.m_pos, m_model3d, this);
    m_graphic3d->attributes()->insertAttribute(HEADING, dp.m_heading);
    m_graphic3d->attributes()->insertAttribute(PITCH, dp.m_pitch);
    m_graphic3d->attributes()->insertAttribute(ROLL, dp.m_roll);

    // add the graphic to the graphics overlay
    m_sceneView->graphicsOverlays()->at(0)->graphics()->append(m_graphic3d);

    // create the camera controller to follow the graphic
    m_followingController = new OrbitGeoElementCameraController(m_graphic3d, 500, this);
    m_sceneView->setCameraController(m_followingController);
  }
  else
  {
    // update existing graphic's geometry and attributes
    m_graphic3d->setGeometry(dp.m_pos);
    m_graphic3d->attributes()->replaceAttribute(HEADING, dp.m_heading);
    m_graphic3d->attributes()->replaceAttribute(PITCH, dp.m_pitch);
    m_graphic3d->attributes()->replaceAttribute(ROLL, dp.m_roll);
  }
}

void Animate3DSymbols::setFollowing(bool following)
{
  if (following)
    m_sceneView->setCameraController(m_followingController);
  else
    m_sceneView->setCameraController(m_globeController);
}

void Animate3DSymbols::zoomMapIn()
{
  if (!m_mapView ||
      !m_routeGraphic)
    return;

  // zoom the map view in, focusing on the position of the 2d graphic for the helicopter
  m_mapView->setViewpoint(Viewpoint((Point)m_routeGraphic->geometry(), m_mapView->mapScale() / m_mapZoomFactor));
}

void Animate3DSymbols::zoomMapOut()
{
  if (!m_mapView ||
      !m_routeGraphic)
    return;

  // zoom the map view out, focusing on the position of the 2d graphic for the helicopter
  m_mapView->setViewpoint(Viewpoint((Point)m_routeGraphic->geometry(), m_mapView->mapScale() * m_mapZoomFactor));
}

void Animate3DSymbols::viewWidthChanged(bool sceneViewIsWider)
{
  if (!m_sceneView || !m_mapView)
  {
    return;
  }

  // only show the attribution text on the view with the widest visible extent
  m_sceneView->setAttributionTextVisible(sceneViewIsWider);
  m_mapView->setAttributionTextVisible(!sceneViewIsWider);
}

bool Animate3DSymbols::missionReady() const
{
  if (!m_missionData)
    return false;

  return m_missionData->ready();
}

int Animate3DSymbols::missionSize() const
{
  if (!m_missionData)
    return 0;

  return (int)m_missionData->size();
}

int Animate3DSymbols::missionFrame() const
{
  return m_frame;
}

double Animate3DSymbols::zoom() const
{
  return m_followingController ? m_followingController->cameraDistance() : 200.0;
}

double Animate3DSymbols::angle() const
{
  return m_followingController ? m_followingController->cameraDistance() : 45.0;
}

double Animate3DSymbols::minZoom() const
{
  return m_followingController ? m_followingController->minCameraDistance() : 0;
}

