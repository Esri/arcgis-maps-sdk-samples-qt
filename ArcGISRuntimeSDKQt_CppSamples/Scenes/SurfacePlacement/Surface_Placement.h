// [WriteFile Name=Surface_Placement, Category=Scenes]
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

#ifndef SURFACE_PLACEMENT_H
#define SURFACE_PLACEMENT_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class SceneQuickView;
    class GraphicsOverlay;
  }
}

#include <QQuickItem>

class Surface_Placement : public QQuickItem
{
  Q_OBJECT

public:
  explicit Surface_Placement(QQuickItem* parent = nullptr);
  ~Surface_Placement() override;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void changeDrapedVisibility();
  Q_INVOKABLE void changeZValue(double zValue);

private:
  void addGraphics();
  void addGraphicsOverlays();

private:
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_drapedFlatOverlay = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_drapedBillboardedOverlay = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_relativeOverlay = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_relativeToSceneOverlay = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_absoluteOverlay = nullptr;
};

#endif // SURFACE_PLACEMENT_H
