// [WriteFile Name=Add3DTilesLayer, Category=Scenes]
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

#include "Add3DTilesLayer.h"

#include "ArcGISTiledElevationSource.h"
#include "ElevationSourceListModel.h"
#include "MapTypes.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "Surface.h"
#include "Basemap.h"
#include "Ogc3dTilesLayer.h"
#include "LayerListModel.h"
#include "Layer.h"
#include "SpatialReference.h"
#include "Point.h"
#include "LocationLineOfSight.h"
#include "AnalysisOverlay.h"
#include "AnalysisOverlayListModel.h"
#include "AnalysisListModel.h"
#include "Camera.h"
#include "TransformationCatalog.h"

#include <MapView.h>
#include <QString>
#include <QFuture>
#include <QDebug>
#include <QStandardPaths>
#include <QVariantMap>
#include <QUrl>

using namespace Esri::ArcGISRuntime;
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
}

const QString DATAPATH = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+ "/ArcGIS/Runtime/Data/";


Add3DTilesLayer::Add3DTilesLayer(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(BasemapStyle::ArcGISImagery, this))
{
  // create a new elevation source from Terrain3D REST service
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
        QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);
  // add projection
  TransformationCatalog::setProjectionEngineDirectory(DATAPATH + "pedata");
  if (TransformationCatalog::projectionEngineDirectory().isEmpty())
    logErrorText("Projection Engine Directory not found at: " + DATAPATH + "pedata");

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);

  // add 3d layers
  add3DTilesLayer();
}

Add3DTilesLayer::~Add3DTilesLayer() = default;

void Add3DTilesLayer::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<Add3DTilesLayer>("Esri.Samples", 1, 0, "Add3DTilesLayerSample");
}

SceneQuickView* Add3DTilesLayer::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void Add3DTilesLayer::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
    return;

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  emit sceneViewChanged();

  if (!defaultDataPath().isEmpty() && m_sceneView)
    initialize();
}

void Add3DTilesLayer::initialize() {

  // add an Analysis Overlay
  m_analysisOverlay = new AnalysisOverlay(this);
  m_sceneView->analysisOverlays()->append(m_analysisOverlay);
  // set initial viewpoint
  setInitialViewpoint();
  // create LocationLineOfSight
  createLineOfSight();
  // connect signals
  connectSignals();
}

void Add3DTilesLayer::logErrorText(const QString &errorMessage)
{
  const QString errorNumber = QVariant(m_errorTextList.size() + 1).toString();
  m_errorTextList.push_front(errorNumber + ": " + errorMessage);
  emit errorTextListChanged();
}

void Add3DTilesLayer::add3DTilesLayer() {

  // edit the url to the path of your data, default path should be the home folder
  const QUrl modelPath = defaultDataPath() + "/ArcGIS/Runtime/Data/Stuttgart.3tz";

  m_ogc3dTilesLayer = new Ogc3dTilesLayer(modelPath, this);
  m_scene->operationalLayers()->append(m_ogc3dTilesLayer);
}

void Add3DTilesLayer::createLineOfSight() {

  // add a observe point (static) and a target point (moving with mouse clicks)
  const Point observerPt( 9.16257, 48.84661, 313, SpatialReference::wgs84());
  const Point targetPt(9.16239, 48.84632, 300, SpatialReference::wgs84());

  // create the line of sight
  m_lineOfSight = new LocationLineOfSight(observerPt, targetPt, this);
  m_analysisOverlay->analyses()->append(m_lineOfSight);

  // configure the LoS color and width
  LineOfSight::setVisibleColor(QColor("cyan"));
  LineOfSight::setObstructedColor(QColor("red"));
  LineOfSight::setLineWidth(2.0f);
}

void Add3DTilesLayer::setInitialViewpoint()
{

  // add a camera
  constexpr double latitude = 48.84553;
  constexpr double longitude = 9.16275;
  constexpr double altitude = 350.0;
  constexpr double heading = 0;
  constexpr double pitch = 60;
  constexpr double roll = 0.0;
  const Camera sceneCamera(latitude, longitude, altitude, heading, pitch, roll);
  m_sceneView->setViewpointCameraAndWait(sceneCamera);
}

void Add3DTilesLayer::connectSignals()
{
  // on mouse click perform the location viewshed
  connect(m_sceneView, &SceneQuickView::mouseClicked, this, [this](QMouseEvent& event)
  {
    m_sceneView->screenToLocationAsync(event.pos().x(), event.pos().y()).then(this, [this](Point p)
    {
      qDebug() << p.toJson();
      m_lineOfSight->setTargetLocation(p);
    });
  });


  connect(m_sceneView, &SceneQuickView::mousePressedAndHeld, this, [this]
  {
    m_calculating = true;
  });

  connect(m_sceneView, &SceneQuickView::mouseMoved, this, [this](QMouseEvent& event)
  {
    if (m_calculating)
    {
      const Point pt = m_sceneView->screenToBaseSurface(event.position().x(), event.position().y());
      m_lineOfSight->setTargetLocation(pt);
    }
  });

  connect(m_sceneView, &SceneQuickView::mouseReleased, this, [this]
  {
    m_calculating = false;
  });
}



