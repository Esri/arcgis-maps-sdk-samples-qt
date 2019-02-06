// [WriteFile Name=OpenScene, Category=Scenes]
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

#ifndef OPENSCENE_H
#define OPENSCENE_H

namespace Esri
{
namespace ArcGISRuntime
{
class SceneQuickView;
}
}

#include <QQuickItem>

class OpenScene : public QQuickItem
{
  Q_OBJECT

public:
  explicit OpenScene(QQuickItem* parent = nullptr);
  ~OpenScene() override = default;

  void componentComplete() override;
  static void init();

private:
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
};

#endif // OPENSCENE_H
