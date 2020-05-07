// [WriteFile Name=DisplayKml, Category=Layers]
// [Legal]
// Copyright 2018 Esri.

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

#include "DisplayKml.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "Viewpoint.h"
#include "Camera.h"
#include "SpatialReference.h"
#include "KmlLayer.h"
#include "KmlDataset.h"
#include "PortalItem.h"

#include <QDir>
#include <QtCore/qglobal.h>

#ifdef Q_OS_IOS
#include <QStandardPaths>
#endif // Q_OS_IOS

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
  QString defaultDataPath()
  {
    QString dataPath;

  #ifdef Q_OS_ANDROID
    dataPath = "/sdcard";
  #elif defined Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  #else
    dataPath = QDir::homePath();
  #endif

    return dataPath;
  }
} // namespace

DisplayKml::DisplayKml(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

DisplayKml::~DisplayKml() = default;

void DisplayKml::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<DisplayKml>("Esri.Samples", 1, 0, "DisplayKmlSample");
}

void DisplayKml::componentComplete()
{
  QQuickItem::componentComplete();

  // Create a scene and give it to the SceneView
  m_sceneView = findChild<SceneQuickView*>("sceneView");
  m_scene = new Scene(Basemap::imageryWithLabels(this), this);
  Surface* surface = new Surface(this);
  surface->elevationSources()->append(
        new ArcGISTiledElevationSource(
          QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"),
          this));
  m_scene->setBaseSurface(surface);
  m_sceneView->setArcGISScene(m_scene);

  // Create a viewpoint
  const Point pt(-98, 39, SpatialReference::wgs84());
  constexpr int scale = 10000000;
  const Camera camera(pt, scale, 0, 0, 0);
  m_viewpoint = Viewpoint(pt, scale, camera);
}

void DisplayKml::createFromUrl()
{
  clearLayers();

  // Create the Dataset from an Online URL
  m_kmlDataset = new KmlDataset(QUrl("https://www.wpc.ncep.noaa.gov/kml/noaa_chart/WPC_Day1_SigWx.kml"), this);

  // Create the Layer
  m_kmlLayer = new KmlLayer(m_kmlDataset, this);

  // Add the layer to the scene
  addLayerToScene(m_kmlLayer);
}

void DisplayKml::createFromFile()
{
  clearLayers();

  // Create the Dataset from a local file
  const QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/kml/";
  m_kmlDataset = new KmlDataset(QUrl(dataPath + "US_State_Capitals.kml"), this);

  // Create the Layer
  m_kmlLayer = new KmlLayer(m_kmlDataset, this);

  // Add the layer to the scene
  addLayerToScene(m_kmlLayer);
}

void DisplayKml::createFromPortalItem()
{
  clearLayers();

  // Create the PortalItem
  m_portalItem = new PortalItem(QStringLiteral("9fe0b1bfdcd64c83bd77ea0452c76253"), this);

  // Create the Layer
  m_kmlLayer = new KmlLayer(m_portalItem, this);

  // Add the layer to the scene
  addLayerToScene(m_kmlLayer);
}

void DisplayKml::addLayerToScene(KmlLayer* layer)
{
  if (!m_scene)
    return;

  m_scene->operationalLayers()->append(layer);

  if (m_viewpoint.isEmpty())
    return;

  m_sceneView->setViewpoint(m_viewpoint);
}

void DisplayKml::clearLayers()
{
  if (!m_scene)
    return;

  // remove the layers
  m_scene->operationalLayers()->clear();

  // clean up the kml datasets
  if (m_kmlDataset)
  {
    delete m_kmlDataset;
    m_kmlDataset = nullptr;
  }

  // clean up the portal items
  if (m_portalItem)
  {
    delete m_portalItem;
    m_portalItem = nullptr;
  }

  // clean up the kml layers
  if (m_kmlLayer)
  {
    delete m_kmlLayer;
    m_kmlLayer = nullptr;
  }
}
