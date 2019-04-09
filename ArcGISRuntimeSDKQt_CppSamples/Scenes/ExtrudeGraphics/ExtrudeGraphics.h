// [WriteFile Name=ExtrudeGraphics, Category=Scenes]
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

#ifndef EXTRUDE_GRAPHICS_H
#define EXTRUDE_GRAPHICS_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Scene;
    class SceneQuickView;
  }
}

#include "Polygon.h"
#include <QQuickItem>

class ExtrudeGraphics : public QQuickItem
{
  Q_OBJECT

public:
  explicit ExtrudeGraphics(QQuickItem* parent = nullptr);
  ~ExtrudeGraphics() override;

  void componentComplete() override;
  static void init();

private:
  Esri::ArcGISRuntime::Polygon createPolygonFromPoints(QList<Esri::ArcGISRuntime::Point>);

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  QUrl m_elevationSourceUrl = QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer");
  int m_maxZ = 1000;
  double m_size = 0.01;
};

#endif // EXTRUDE_GRAPHICS_H

