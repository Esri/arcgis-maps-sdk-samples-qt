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

#ifndef DISPLAYKML_H
#define DISPLAYKML_H

namespace Esri
{
namespace ArcGISRuntime
{
class SceneQuickView;
class Scene;
class KmlLayer;
class KmlDataset;
class PortalItem;
}
}

#include "Viewpoint.h"
#include <QQuickItem>

class DisplayKml : public QQuickItem
{
  Q_OBJECT

public:
  explicit DisplayKml(QQuickItem* parent = nullptr);
  ~DisplayKml() override;

  void componentComplete() override;
  static void init();

  Q_INVOKABLE void createFromUrl();
  Q_INVOKABLE void createFromFile();
  Q_INVOKABLE void createFromPortalItem();

private:
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::Viewpoint m_viewpoint;
  Esri::ArcGISRuntime::KmlDataset* m_kmlDataset = nullptr;
  Esri::ArcGISRuntime::KmlLayer* m_kmlLayer = nullptr;
  Esri::ArcGISRuntime::PortalItem* m_portalItem = nullptr;

  void addLayerToScene(Esri::ArcGISRuntime::KmlLayer* layer);
  void clearLayers();
};

#endif // DISPLAYKML_H
