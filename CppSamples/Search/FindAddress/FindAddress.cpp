// [WriteFile Name=FindAddress, Category=Search]
// [Legal]
// Copyright 2016 Esri.
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
#include "FindAddress.h"

// ArcGIS Maps SDK headers
#include "AttributeListModel.h"
#include "Basemap.h"
#include "CalloutData.h"
#include "Envelope.h"
#include "GeocodeParameters.h"
#include "GeocodeResult.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "IdentifyGraphicsOverlayResult.h"
#include "LocatorTask.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "MapTypes.h"
#include "PictureMarkerSymbol.h"
#include "Point.h"
#include "SimpleRenderer.h"
#include "Viewpoint.h"
#include "Ogc3dTilesLayer.h"
#include "LayerListModel.h"
#include "MapViewTypes.h"
#include "Camera.h"
#include "Point.h"
#include "LayerSceneProperties.h"
#include "SpatialReference.h"
#include "SceneViewTypes.h"
#include "OrbitGeoElementCameraController.h"
#include "Portal.h"
#include "ArcGISRuntimeEnvironment.h"
#include "Authentication/ArcGISCredentialStore.h"
#include "Authentication/AuthenticationManager.h"
#include "Authentication/TokenCredential.h"
#include "ErrorException.h"

// Qt headers
#include <QFuture>
#include <QUrl>
#include <QUuid>
#include <QTimer>

// STL headers
#include <memory>

using namespace Esri::ArcGISRuntime;

FindAddress::FindAddress(QQuickItem* parent) :
  QQuickItem(parent)
{
}

FindAddress::~FindAddress() = default;

void FindAddress::init()
{
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<FindAddress>("Esri.Samples", 1, 0, "FindAddressSample");
  qmlRegisterUncreatableType<CalloutData>("Esri.Samples", 1, 0, "CalloutData", "CalloutData is an uncreatable type");
}

void FindAddress::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_sceneView = findChild<SceneQuickView*>("sceneView");

  QString sceneJson = R"(
  {
    "operationalLayers": [],
    "baseMap": {
        "baseMapLayers": [
        ],
        "groundLayers": [
            {
                "id": "Boston Mesh",
                "title": "Boston Mesh",
                "url": "https://tiles.arcgis.com/tiles/N82JbI5EYtAkuUKU/arcgis/rest/services/Boston/3DTilesServer/tileset.json",
                "layerType": "IntegratedMesh3DTilesLayer"
            }
        ],
        "id": "199113ecebd-basemap-0",
        "title": "Photorealistic Hybrid",
        "elevationLayers": []
    },
    "ground": {
        "layers": [],
        "transparency": 0,
        "navigationConstraint": {
            "type": "stayAbove"
        }
    },
    "heightModelInfo": {
        "heightModel": "gravity_related_height",
        "heightUnit": "meter"
    },
    "version": "1.39",
    "authoringApp": "WebSceneViewer",
    "authoringAppVersion": "2026.1.0",
    "initialState": {
        "environment": {
            "lighting": {
                "type": "sun",
                "datetime": 1773593054000,
                "displayUTCOffset": -5
            },
            "atmosphereEnabled": true,
            "starsEnabled": true,
            "weather": {
                "type": "sunny",
                "cloudCover": 0
            }
        },
        "viewpoint": {
            "camera": {
                "position": {
                    "spatialReference": {
                        "latestWkid": 3857,
                        "wkid": 102100,
                        "falseM": -100000,
                        "falseX": -20037700,
                        "falseY": -30241100,
                        "falseZ": -100000,
                        "mTolerance": 0.001,
                        "mUnits": 10000,
                        "xyTolerance": 0.001,
                        "xyUnits": 10000,
                        "zTolerance": 0.001,
                        "zUnits": 10000
                    },
                    "x": -7910153.22359822,
                    "y": 5211899.4686107645,
                    "z": 341.22248407267034
                },
                "heading": 3.6534769981657105,
                "tilt": 80.28799294964895
            }
        }
    },
    "spatialReference": {
        "latestWkid": 3857,
        "wkid": 102100,
        "falseM": -100000,
        "falseX": -20037700,
        "falseY": -30241100,
        "falseZ": -100000,
        "mTolerance": 0.001,
        "mUnits": 10000,
        "xyTolerance": 0.001,
        "xyUnits": 10000,
        "zTolerance": 0.001,
        "zUnits": 10000
    },
    "viewingMode": "global"
  })";
  //m_scene = Scene::fromJson(sceneJson, this);

  QUrl portalUrl("https://runtimecoretest.maps.arcgis.com");
  QFuture<Authentication::TokenCredential*> authFuture =
    Esri::ArcGISRuntime::Authentication::TokenCredential::createAsync(portalUrl, "c_api_publisher", "cDMyA3AfeK", {}, this);

  authFuture
    .then(
      [portalUrl, this](Authentication::TokenCredential* tokenCredential)
  {
    ArcGISRuntimeEnvironment::authenticationManager()->arcGISCredentialStore()->add(tokenCredential, portalUrl);

    // now do part 2 - the other stuff you were waiting on until you had the token.
    Portal* portal = new Portal(portalUrl, true, this);
    m_scene = new Scene(QUrl("https://runtimecoretest.maps.arcgis.com/home/"
                             "item.html?id=314afa9e9f5d422394999286197dd6d6"),
                        this);
    m_sceneView->setArcGISScene(m_scene);
  })
    .onFailed([](const ErrorException& e)
  {
    qDebug() << "createAndAddTokenCredential failed: " << e.error().message();
  });
  // set map on the map view
  m_sceneView->setArcGISScene(m_scene);
  // create graphics overlay and add to map view
  m_graphicsOverlay = new GraphicsOverlay(this);
  m_sceneView->graphicsOverlays()->append(m_graphicsOverlay);

  // Ogc3dTilesLayer* layer = new Ogc3dTilesLayer(QUrl("https://tile.googleapis.com/v1/3dtiles/root.json"), this);
  // QMap<QString, QString> customParamsMapGoogle;
  // customParamsMapGoogle.insert("key", "");
  // layer->setCustomParameters(customParamsMapGoogle);
  // m_scene->operationalLayers()->append(layer);

  // set a renderer on the graphics overlay
  SimpleRenderer* simpleRenderer = new SimpleRenderer(this);
  PictureMarkerSymbol* pictureMarkerSymbol = new PictureMarkerSymbol(QUrl("qrc:/Samples/Search/FindAddress/pin_circle_red.png"), this);
  pictureMarkerSymbol->setWidth(35);
  pictureMarkerSymbol->setHeight(35);
  pictureMarkerSymbol->setOffsetY(pictureMarkerSymbol->height() / 2);
  simpleRenderer->setSymbol(pictureMarkerSymbol);
  m_graphicsOverlay->setRenderer(simpleRenderer);
  m_graphic = new Graphic(this);
  m_graphicsOverlay->graphics()->append(m_graphic);
  LayerSceneProperties props = m_graphicsOverlay->sceneProperties();
  props.setSurfacePlacement(SurfacePlacement::RelativeToScene);
  m_graphicsOverlay->setSceneProperties(props);

  // create locator task and parameters
  //! [FindAddress create LocatorTask]
  m_locatorTask = new LocatorTask(QUrl("https://geocode-api.arcgis.com/arcgis/rest/services/World/GeocodeServer"), this);
  //! [FindAddress create LocatorTask]
  m_geocodeParameters.setMinScore(75);
  m_geocodeParameters.setResultAttributeNames(QStringList{"Place_addr", "Match_addr"});

  connectSignals();

  m_cameras << Camera(Point(151.21667523730338, -33.858288904108932, 75.57748594507575, m_sceneView->spatialReference()), 319.4, 79.7423, 0.0)
            << Camera(Point(7.6456953756234176, 45.949094718829706, 4131.6949092438444, m_sceneView->spatialReference()), 18.3768, 87.3143, 0.0);
}

void FindAddress::connectSignals()
{
  //! [FindAddress geocodeCompleted handler]

  connect(m_sceneView, &SceneQuickView::drawStatusChanged, this, [this](DrawStatus status)
  {
    qDebug() << "Draw status changed: " << static_cast<int>(status);

    if (status != DrawStatus::Completed)
    {
      return;
    }

    qDebug() << m_sceneView->currentViewpointCamera().location().toJson() << m_sceneView->currentViewpointCamera().heading()
             << m_sceneView->currentViewpointCamera().pitch();
  });

  // connect to the mouse click signal on the SceneQuickView
  connect(m_sceneView, &SceneQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    // set the properties for qml
    // m_sceneView->calloutData()->setLocation(m_sceneView->screenToLocationAsync(mouseEvent.position().x(), mouseEvent.position().y()));
    // emit hideCallout();

    // call identify on the map view
    m_sceneView->identifyGraphicsOverlayAsync(m_graphicsOverlay, mouseEvent.position(), 5, false, 1)
      .then(this, [this](IdentifyGraphicsOverlayResult* rawIdentifyResult)
    {
      // Delete rawIdentifyResult on leaving scope.
      auto identifyResult = std::unique_ptr<IdentifyGraphicsOverlayResult>(rawIdentifyResult);

      if (!identifyResult)
      {
        return;
      }

      const QList<Graphic*> graphics = identifyResult->graphics();
      if (graphics.isEmpty())
      {
        return;
      }

      const AttributeListModel* attributes = graphics.at(0)->attributes();
      const QString calloutText = attributes->attributeValue("Match_addr").toString();
      m_sceneView->calloutData()->setTitle(calloutText);
      emit showCallout();
    });
  });
}

void FindAddress::geocodeAddress(const QString& address)
{
  //! [FindAddress geocodeWithParameters]
  m_locatorTask->geocodeWithParametersAsync(address, m_geocodeParameters)
    .then(this, [this, address](const QList<GeocodeResult>& geocodeResults)
  {
    if (geocodeResults.isEmpty())
    {
      return;
    }

    // m_graphic->setGeometry(geocodeResults.at(0).displayLocation());
    m_graphic->attributes()->setAttributesMap(geocodeResults.at(0).attributes());
    constexpr double scale = 8000.0;

    if (address.toLower() == "opera house, sydney")
    {
      m_sceneView->setViewpointCameraAsync(m_cameras.at(0), 5.0);
    }
    else if (address.toLower() == "matterhorn, switzerland")
    {
      m_sceneView->setViewpointCameraAsync(m_cameras.at(1), 5.0);
    }
    else
    {
      m_sceneView->setViewpointAsync(Viewpoint(geocodeResults.at(0).displayLocation(), scale));
    }
  });
  //! [FindAddress geocodeWithParameters]
}

void FindAddress::clearGraphics()
{
  m_graphic->setGeometry(Point());
}

#include "OrbitLocationCameraController.h"

void FindAddress::startCameraController()
{
  m_sceneView->screenToLocationAsync(m_sceneView->width() * 0.5, m_sceneView->height() * 0.5)
    .then(this, [this](const Point& location)
  {
    OrbitLocationCameraController* cameraController =
      new OrbitLocationCameraController(location, m_sceneView->currentViewpointCamera().location(), this);
    m_sceneView->setCameraController(cameraController);

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [cameraController]()
    {
      cameraController->setCameraHeadingOffset(cameraController->cameraHeadingOffset() + 1.0);
    });
    timer->setInterval(50);
    timer->start();
  });
}
