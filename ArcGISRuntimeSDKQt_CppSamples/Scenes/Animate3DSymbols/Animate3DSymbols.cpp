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
#include "MissionData.h"

#include "ArcGISTiledElevationSource.h"
#include "Camera.h"
#include "DistanceCompositeSceneSymbol.h"
#include "GraphicsOverlay.h"
#include "Map.h"
#include "MapQuickView.h"
#include "ModelSceneSymbol.h"
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
#include "TaskWatcher.h"

#include <QDir>
#include <QFileInfo>
#include <QStringListModel>
#include <QQmlProperty>

using namespace Esri::ArcGISRuntime;

const QString Animate3DSymbols::HEADING = "HEADING";
const QString Animate3DSymbols::ROLL = "ROLL";
const QString Animate3DSymbols::PITCH = "PITCH";
const QString Animate3DSymbols::ANGLE = "ANGLE";

struct Animate3DSymbols::CameraHandler
{

  void setZoomDist(double zoomDist)
  {
    m_camWatcher.cancel();
    m_zoomDist = zoomDist;
  }

  void setAngle(double angle)
  {
    m_camWatcher.cancel();
    m_angle = angle;
  }

  void setIntervalMs(int intervalMs)
  {
    m_camWatcher.cancel();
    m_intervalMs = intervalMs;
  }

  float animationDurationSeconds() const
  {
    // the faster the animation, the shorter duration we want for our camera animations (range 10-200)
    float res = (200 - m_intervalMs) / 200.0f;
    res *= 10.0f;

    // slow down camera animation when the camera is zoomed right in (range 10-500)
    float zoomFactor = (510 - m_zoomDist) / 250.0f;
    res += zoomFactor;

    return res;
  }

  TaskWatcher m_camWatcher;
  float m_intervalMs = 0.0;
  double m_zoomDist = 0.0;
  double m_angle = 90.0;
};

Animate3DSymbols::Animate3DSymbols(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_sceneView(nullptr),
  m_mapView(nullptr),
  m_model3d(nullptr),
  m_graphic3d(nullptr),
  m_graphic2d(nullptr),
  m_routeGraphic(nullptr),
  m_missionsModel(new QStringListModel({"Grand Canyon", "Hawaii", "Pyrenees", "Snowdon"}, this)),
  m_missionData(new MissionData()),
  m_camHandler(new CameraHandler),
  m_frame(0),
  m_following(true),
  m_mapZoomFactor(5.0)
{
  Q_INIT_RESOURCE(Animate3DSymbols);
}

Animate3DSymbols::~Animate3DSymbols()
{
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
  m_sceneView->setScene(scene);

  // create a new elevation source
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
    QUrl("http://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  scene->baseSurface()->elevationSources()->append(elevationSource);

  // create a new graphics overlay and add it to the sceneview
  GraphicsOverlay* sceneOverlay = new GraphicsOverlay(this);
  sceneOverlay->sceneProperties().setSurfacePlacement(SurfacePlacement::Absolute);
  m_sceneView->graphicsOverlays()->append(sceneOverlay);

  SimpleRenderer* renderer3D = new SimpleRenderer(this);
  RendererSceneProperties renderProperties = renderer3D->sceneProperties();
  renderProperties.setHeadingExpression(HEADING);
  renderProperties.setPitchExpression(PITCH);
  renderProperties.setRollExpression(ROLL);
  sceneOverlay->setRenderer(renderer3D);

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // set up mini map
  Map* map = new Map(Basemap::imagery(this), this);
  m_mapView->setMap(map);

  // create a graphics overlay for the mini map
  GraphicsOverlay* mapOverlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(mapOverlay);

  // create renderer to handle updating plane heading using the graphics card
  SimpleRenderer* renderer2D = new SimpleRenderer(this);
  renderer2D->setRotationExpression(QString("[%1]").arg(ANGLE));
  mapOverlay->setRenderer(renderer2D);

  // set up route graphic
  createRoute2d(mapOverlay);
  // set up 3d graphic for model position in map view
  createModel2d(mapOverlay);
}

void Animate3DSymbols::setFrame(int newFrame)
{
  if (m_missionData == nullptr ||
     newFrame > (int)m_missionData->size() ||
     newFrame < 0 ||
     m_frame == newFrame)
    return;

  m_frame = newFrame;
}

void Animate3DSymbols::changeSpeed(int intervalMs)
{
  m_camHandler->setIntervalMs(intervalMs);
}

void Animate3DSymbols::nextFrame()
{
  if (m_missionData == nullptr)
    return;

  if (m_frame < (int)m_missionData->size())
  {
    // get the data for this stage in the mission
    const MissionData::DataPoint& dp = m_missionData->dataAt(m_frame);

    if (m_following)
    {
      if(!m_camHandler->m_camWatcher.isDone())
        m_sceneView->update();
      else
      {
        // move the camera to follow the 3d model
        Camera camera(dp.m_pos, m_camHandler->m_zoomDist, dp.m_heading, m_camHandler->m_angle, dp.m_roll);
        m_camHandler->m_camWatcher = m_sceneView->setViewpointCamera(camera, m_camHandler->animationDurationSeconds());
      }

#ifdef ANIMATE_MAP
      // rotate the map view about the direction of motion
      m_mapView->setViewpoint(Viewpoint(dp.m_pos, m_mapView->mapScale(), 360. + dp.m_heading));
#endif
    }
    else
    {
#ifdef ANIMATE_MAP
      m_graphic2d->attributes()->replaceAttribute(ANGLE, 360 + dp.m_heading - m_mapView->mapRotation());
#endif
      m_sceneView->update();
    }

#ifdef ANIMATE_MAP
    // move 2d graphic to the new position
    m_graphic2d->setGeometry(dp.m_pos);
#endif

    // move 3D graphic to the new position
    m_graphic3d->setGeometry(dp.m_pos);
    // update attribute expressions to immediately update rotation
    m_graphic3d->attributes()->replaceAttribute(HEADING, dp.m_heading);
    m_graphic3d->attributes()->replaceAttribute(PITCH, dp.m_pitch);
    m_graphic3d->attributes()->replaceAttribute(ROLL, dp.m_roll);
  }

  // increment the frame count
  m_frame++;
  if (m_frame >= (int)m_missionData->size())
    m_frame = 0;
}

void Animate3DSymbols::changeMission(const QString &missionNameStr)
{
  m_frame = 0;
  m_camHandler->m_camWatcher.cancel();

  // read the mission data from .csv files stored in qrc
  QString formattedname = missionNameStr;
  m_missionData->parse(":/Missions/" + formattedname.remove(" ") + ".csv");

  // if the mission was loaded successfully, move to the start position
  if (missionReady())
  {
    // create a polyline representing the route for the mission
    PolylineBuilder* routeBldr = new PolylineBuilder(SpatialReference::wgs84(), this);
    for(size_t i = 0; i < m_missionData->size(); ++i)
    {
      const MissionData::DataPoint& dp = m_missionData->dataAt(i);
      routeBldr->addPoint(dp.m_pos);
    }
    // set the polyline as a graphic on the mapView
    m_routeGraphic->setGeometry(routeBldr->toGeometry());

    const MissionData::DataPoint& dp = m_missionData->dataAt(m_frame);
    Camera camera(dp.m_pos, m_camHandler->m_zoomDist, dp.m_heading, m_camHandler->m_angle, dp.m_roll);
    m_sceneView->setViewpointCameraAndWait(camera);
    m_mapView->setViewpointAndWait(Viewpoint(m_routeGraphic->geometry()));
    createModel3d();
  }

  emit missionReadyChanged();
  emit missionSizeChanged();
}

void Animate3DSymbols::setZoom(double zoomDist)
{
  m_camHandler->setZoomDist(zoomDist);
}

void Animate3DSymbols::setAngle(double angle)
{
  m_camHandler->setAngle(angle);
}

void Animate3DSymbols::clearGraphic3D()
{
  if (m_graphic3d == nullptr)
    return;

  if (m_sceneView->graphicsOverlays()->size() > 0 && m_sceneView->graphicsOverlays()->at(0))
    m_sceneView->graphicsOverlays()->at(0)->graphics()->clear();

  delete m_graphic3d;
  m_graphic3d = nullptr;
}

void Animate3DSymbols::createModel3d()
{
  // load the ModelSceneSymbol to be animated in the 3d view
  if (m_model3d == nullptr)
  {
    m_model3d = new ModelSceneSymbol(QUrl(m_dataPath + "/SkyCrane/SkyCrane.lwo"), 0.01f, this);
    // correct the symbol's orientation to match the graphic's orientation
    m_model3d->setHeading(180.);
  }

  // when the model is successfully loaded, setup the 3g draphic to show it it the scene
  connect(m_model3d, &ModelSceneSymbol::loadStatusChanged, [this]()
    {
      if (m_model3d->loadStatus() == LoadStatus::Loaded)
        createGraphic3D();
    }
  );

  if (m_model3d->loadStatus() == LoadStatus::Loaded)
    createGraphic3D(); // if the model is already loaded just recreate the graphic
  else
    m_model3d->load(); // if the model has not been loaded before, call load
}

void Animate3DSymbols::createModel2d(GraphicsOverlay *mapOverlay)
{
  // create a blue triangle symbol to represent the plane on the mini map
  SimpleMarkerSymbol* plane2DSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Triangle, Qt::blue, 10, this);

  // create a graphic with the symbol and attributes
  QVariantMap attributes;
  attributes.insert(ANGLE, 0.f);
  m_graphic2d = new Graphic(Point(0, 0, SpatialReference::wgs84()), attributes, plane2DSymbol, this);
  mapOverlay->graphics()->append(m_graphic2d);
}

void Animate3DSymbols::createRoute2d(GraphicsOverlay* mapOverlay)
{
  // Create a 2d graphic of a solid red line to represen the route of the mission
  SimpleLineSymbol* routeSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::red, 2, this);
  m_routeGraphic = new Graphic(this);
  m_routeGraphic->setSymbol(routeSymbol);
  mapOverlay->graphics()->append(m_routeGraphic);
}

void Animate3DSymbols::createGraphic3D()
{
  if (m_model3d == nullptr || !missionReady())
    return;

  clearGraphic3D();

  // get the mission data for the frame
  const MissionData::DataPoint& dp = m_missionData->dataAt(m_frame);

  // create a graphic using the model symbol
  m_graphic3d = new Graphic(dp.m_pos, m_model3d, this);
  m_graphic3d->attributes()->insertAttribute(HEADING, dp.m_heading);
  m_graphic3d->attributes()->insertAttribute(PITCH, dp.m_pitch);
  m_graphic3d->attributes()->insertAttribute(ROLL, dp.m_roll);

  // add the graphic to the graphics overlay
  m_sceneView->graphicsOverlays()->at(0)->graphics()->append(m_graphic3d);
}

void Animate3DSymbols::setFollowing(bool following)
{
  m_following = following;
}

void Animate3DSymbols::zoomMapIn()
{
  if (m_mapView == nullptr ||
      m_graphic2d == nullptr)
    return;

  // zoom the map view in, focusing on the position of the 2d graphic for the helicopter
  m_mapView->setViewpoint(Viewpoint((Point)m_routeGraphic->geometry(), m_mapView->mapScale() / m_mapZoomFactor));
}

void Animate3DSymbols::zoomMapOut()
{
  if (m_mapView == nullptr ||
      m_graphic2d == nullptr)
    return;

  // zoom the map view out, focusing on the position of the 2d graphic for the helicopter
  m_mapView->setViewpoint(Viewpoint((Point)m_routeGraphic->geometry(), m_mapView->mapScale() * m_mapZoomFactor));
}

bool Animate3DSymbols::missionReady() const
{
  if (m_missionData == nullptr)
    return false;

  return m_missionData->ready();
}

int Animate3DSymbols::missionSize() const
{
  if (m_missionData == nullptr)
    return 0;

  return (int)m_missionData->size();
}
