// [WriteFile Name=DisplayKmlNetworkLinks, Category=Layers]
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

#include "DisplayKmlNetworkLinks.h"

#include "ArcGISTiledElevationSource.h"
#include "KmlDataset.h"
#include "KmlLayer.h"
#include "Scene.h"
#include "SceneQuickView.h"

using namespace Esri::ArcGISRuntime;

DisplayKmlNetworkLinks::DisplayKmlNetworkLinks(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

DisplayKmlNetworkLinks::~DisplayKmlNetworkLinks() = default;

void DisplayKmlNetworkLinks::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<DisplayKmlNetworkLinks>("Esri.Samples", 1, 0, "DisplayKmlNetworkLinksSample");
}

QString DisplayKmlNetworkLinks::currentKmlNetworkMessage() const
{
  return m_currentKmlNetworkMessage;
}

void DisplayKmlNetworkLinks::setCurrentKmlNetworkMessage(const QString& message)
{
  m_currentKmlNetworkMessage = message;
  emit kmlMessageRecieved(message);
}

void DisplayKmlNetworkLinks::componentComplete()
{
  QQuickItem::componentComplete();

  // Create a scene and give it to the SceneView
  m_sceneView = findChild<SceneQuickView*>("sceneView");
  Scene* scene = new Scene(Basemap::imagery(this), this);

  // Create a KML dataset from the given resource.
  // This is a KML resource that references other KML resources over a network.
  KmlDataset* dataset = new KmlDataset(QUrl("https://www.arcgis.com/sharing/rest/content/items/600748d4464442288f6db8a4ba27dc95/data"), this);
  connect(dataset, &KmlDataset::kmlNetworkLinkMessageReceived,
          this, [this](KmlNetworkLink* /*link*/, const QString& message)
  {
    setCurrentKmlNetworkMessage(message);
  });

  // Now that we have the data, we need a layer to interpret it.
  KmlLayer* fileLayer = new KmlLayer(dataset, this);
  scene->operationalLayers()->append(fileLayer);

  // Take a look at continental Europe, where we'll see most of the data.
  m_sceneView->setViewpoint(Viewpoint { Point { 8.150526, 50.472421, SpatialReference::wgs84() }, 20000000 } );
  m_sceneView->setArcGISScene(scene);
}

