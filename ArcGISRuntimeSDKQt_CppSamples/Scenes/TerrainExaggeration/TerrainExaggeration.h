// [WriteFile Name=TerrainExaggeration, Category=Scenes]
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

#ifndef TERRAINEXAGGERATION_H
#define TERRAINEXAGGERATION_H

namespace Esri
{
namespace ArcGISRuntime
{
class SceneQuickView;
class Surface;
}
}

#include <QQuickItem>

class TerrainExaggeration : public QQuickItem
{
  Q_OBJECT

public:
  explicit TerrainExaggeration(QQuickItem* parent = nullptr);
  ~TerrainExaggeration() override = default;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void setElevationExaggeration(double factor);

private:
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::Surface* m_surface = nullptr;
};

#endif // TERRAINEXAGGERATION_H
