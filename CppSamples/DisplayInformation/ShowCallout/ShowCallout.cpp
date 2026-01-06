// [WriteFile Name=ShowCallout, Category=DisplayInformation]
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
#include "ShowCallout.h"

// ArcGIS Maps SDK headers
#include "ArcGISTiledElevationSource.h"
#include "CalloutData.h"
#include "Camera.h"
#include "ElevationSourceListModel.h"
#include "LocalSceneQuickView.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "SceneViewTypes.h"
#include "SpatialReference.h"
#include "Surface.h"
#include "Viewpoint.h"

// Qt headers
#include <QDebug>
#include <QFuture>
#include <QImage>
#include <QMouseEvent>
#include <QRandomGenerator>

using namespace Esri::ArcGISRuntime;

namespace
{
  QString makeDetailsText(const Point& mapPoint)
  {
    return QString("Latitude: %1\nLongitude: %2\nElevation: %3m\nRandom ID: %4")
      .arg(mapPoint.y(), 0, 'f', 4)
      .arg(mapPoint.x(), 0, 'f', 4)
      .arg(mapPoint.z(), 0, 'f', 1)
      .arg(QRandomGenerator::global()->bounded(1000, 9999));
  }
} // namespace

ShowCallout::ShowCallout(QObject* parent /* = nullptr */) :
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISImagery, this)),
  m_scene(new Scene(BasemapStyle::ArcGISImagery, this)),
  m_localScene(new Scene(BasemapStyle::ArcGISImagery, SceneViewingMode::Local, this))
{
  // create new elevation sources from Terrain3D REST service
  auto* elevationSourceGlobal =
    new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);
  auto* elevationSourceLocal =
    new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add elevation sources to scenes to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSourceGlobal);
  m_localScene->baseSurface()->elevationSources()->append(elevationSourceLocal);
}

ShowCallout::~ShowCallout() = default;

void ShowCallout::init()
{
  // Register classes for QML
  qmlRegisterType<LocalSceneQuickView>("Esri.Samples", 1, 0, "LocalSceneView");
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<ShowCallout>("Esri.Samples", 1, 0, "ShowCalloutSample");
  qmlRegisterUncreatableType<CalloutData>("Esri.Samples", 1, 0, "CalloutData", "CalloutData is an uncreatable type");
}

MapQuickView* ShowCallout::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ShowCallout::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  if (m_mapClickConnection)
  {
    disconnect(m_mapClickConnection);
  }

  if (m_mapSceneLocationVisibilityConnection)
  {
    disconnect(m_mapSceneLocationVisibilityConnection);
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  // Set an initial viewpoint.
  constexpr double latitude = 34.0270;
  constexpr double longitude = -118.8050;
  constexpr double scale = 1.0e5;
  m_mapView->setViewpointAsync(Viewpoint(latitude, longitude, scale));

  // Initialize callout with dummy data
  m_mapView->calloutData()->setVisible(false);
  m_mapView->calloutData()->setTitle("Test Location");
  QImage image(":/Samples/DisplayInformation/ShowCallout/RedShinyPin.png");
  m_mapView->calloutData()->setImage(image);

  m_mapSceneLocationVisibilityConnection = connect(m_mapView->calloutData(), &CalloutData::sceneLocationVisibilityChanged, this, [this]
  {
    const auto opt = m_mapView->calloutData()->sceneLocationVisibility();
    if (opt.has_value())
    {
      qDebug() << "CalloutData.sceneLocationVisibility (MapView):" << *opt;
    }
    else
    {
      qDebug() << "CalloutData.sceneLocationVisibility (MapView):" << "<none>";
    }
  });
  {
    const auto opt = m_mapView->calloutData()->sceneLocationVisibility();
    if (opt.has_value())
    {
      qDebug() << "[Init] CalloutData.sceneLocationVisibility (MapView):" << *opt;
    }
    else
    {
      qDebug() << "[Init] CalloutData.sceneLocationVisibility (MapView):" << "<none>";
    }
  }

  m_mapClickConnection = connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    auto* calloutData = m_mapView->calloutData();
    if (calloutData->isVisible())
    {
      calloutData->setVisible(false);
      return;
    }

    const Point mapPoint(m_mapView->screenToLocation(mouseEvent.position().x(), mouseEvent.position().y()));
    if (mapPoint.isEmpty())
    {
      return;
    }

    calloutData->setLocation(mapPoint);
    calloutData->setDetail(makeDetailsText(mapPoint));
    calloutData->setVisible(true);

    const auto vis = calloutData->sceneLocationVisibility();

    qDebug() << "CalloutData.sceneLocationVisibility (MapView click):" << vis.has_value();
  });

  emit mapViewChanged();
}

SceneQuickView* ShowCallout::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void ShowCallout::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
  {
    return;
  }

  if (m_sceneClickConnection)
  {
    disconnect(m_sceneClickConnection);
  }

  if (m_sceneSceneLocationVisibilityConnection)
  {
    disconnect(m_sceneSceneLocationVisibilityConnection);
  }

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  // create a camera
  const double latitude = 34.0270;
  const double longitude = -118.8050;
  const double altitude = 10000.0;
  const double heading = 10.0;
  const double pitch = 55.0;
  const double roll = 0.0;
  Camera camera(latitude, longitude, altitude, heading, pitch, roll);

  // set the viewpoint
  m_sceneView->setViewpointCameraAndWait(camera);

  // Initialize callout with dummy data
  m_sceneView->calloutData()->setVisible(false);
  m_sceneView->calloutData()->setTitle("Test Location");
  QImage image(":/Samples/DisplayInformation/ShowCallout/RedShinyPin.png");
  m_sceneView->calloutData()->setImage(image);

  m_sceneSceneLocationVisibilityConnection = connect(m_sceneView->calloutData(), &CalloutData::sceneLocationVisibilityChanged, this, [this]
  {
    const auto opt = m_sceneView->calloutData()->sceneLocationVisibility();
    if (opt.has_value())
    {
      qDebug() << "CalloutData.sceneLocationVisibility (SceneView):" << *opt;
    }
    else
    {
      qDebug() << "CalloutData.sceneLocationVisibility (SceneView):" << "<none>";
    }
  });
  {
    const auto opt = m_sceneView->calloutData()->sceneLocationVisibility();
    if (opt.has_value())
    {
      qDebug() << "[Init] CalloutData.sceneLocationVisibility (SceneView):" << *opt;
    }
    else
    {
      qDebug() << "[Init] CalloutData.sceneLocationVisibility (SceneView):" << "<none>";
    }
  }

  m_sceneClickConnection = connect(m_sceneView, &SceneQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    auto* calloutData = m_sceneView->calloutData();
    if (calloutData->isVisible())
    {
      calloutData->setVisible(false);
      return;
    }

    const Point mapPoint(m_sceneView->screenToBaseSurface(mouseEvent.position().x(), mouseEvent.position().y()));
    if (mapPoint.isEmpty())
    {
      return;
    }

    calloutData->setLocation(mapPoint);
    calloutData->setDetail(makeDetailsText(mapPoint));
    calloutData->setVisible(true);
  });

  emit sceneViewChanged();
}

LocalSceneQuickView* ShowCallout::localSceneView() const
{
  return m_localSceneView;
}

// Set the view (created in QML)
void ShowCallout::setLocalSceneView(LocalSceneQuickView* localSceneView)
{
  if (!localSceneView || localSceneView == m_localSceneView)
  {
    return;
  }

  if (m_localSceneClickConnection)
  {
    disconnect(m_localSceneClickConnection);
  }

  if (m_localSceneSceneLocationVisibilityConnection)
  {
    disconnect(m_localSceneSceneLocationVisibilityConnection);
  }

  m_localSceneView = localSceneView;
  m_localSceneView->setArcGISScene(m_localScene);

  // create a camera
  const double latitude = 34.0270;
  const double longitude = -118.8050;
  const double altitude = 10000.0;
  const double heading = 10.0;
  const double pitch = 55.0;
  const double roll = 0.0;
  Camera camera(latitude, longitude, altitude, heading, pitch, roll);

  // set the viewpoint
  m_localSceneView->setViewpointCameraAndWait(camera);

  // Initialize callout with dummy data
  m_localSceneView->calloutData()->setVisible(false);
  m_localSceneView->calloutData()->setTitle("Test Location");
  QImage image(":/Samples/DisplayInformation/ShowCallout/RedShinyPin.png");
  m_localSceneView->calloutData()->setImage(image);

  m_localSceneSceneLocationVisibilityConnection = connect(m_localSceneView->calloutData(), &CalloutData::sceneLocationVisibilityChanged, this, [this]
  {
    const auto opt = m_localSceneView->calloutData()->sceneLocationVisibility();
    if (opt.has_value())
    {
      qDebug() << "CalloutData.sceneLocationVisibility (LocalSceneView):" << *opt;
    }
    else
    {
      qDebug() << "CalloutData.sceneLocationVisibility (LocalSceneView):" << "<none>";
    }
  });
  {
    const auto opt = m_localSceneView->calloutData()->sceneLocationVisibility();
    if (opt.has_value())
    {
      qDebug() << "[Init] CalloutData.sceneLocationVisibility (LocalSceneView):" << *opt;
    }
    else
    {
      qDebug() << "[Init] CalloutData.sceneLocationVisibility (LocalSceneView):" << "<none>";
    }
  }

  // display callout on mouseClicked with dummy data
  m_localSceneClickConnection = connect(m_localSceneView, &LocalSceneQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    auto* calloutData = m_localSceneView->calloutData();
    if (calloutData->isVisible())
    {
      calloutData->setVisible(false);
    }
    else
    {
      // Convert screen coordinates to location
      Point mapPoint(m_localSceneView->screenToBaseSurface(mouseEvent.position().x(), mouseEvent.position().y()));

      if (!mapPoint.isEmpty())
      {
        // Set callout position
        calloutData->setLocation(mapPoint);
        calloutData->setDetail(makeDetailsText(mapPoint));
        calloutData->setVisible(true);
      }
    }
  });

  emit localSceneViewChanged();
}
