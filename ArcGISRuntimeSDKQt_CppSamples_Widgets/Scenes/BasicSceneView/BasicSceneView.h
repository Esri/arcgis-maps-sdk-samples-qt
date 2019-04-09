// [WriteFile Name=BasicSceneView, Category=Scenes]
// [Legal]
// Copyright 2015 Esri.

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

#ifndef BASIC_SCENEVIEW_H
#define BASIC_SCENEVIEW_H

namespace Esri {
namespace ArcGISRuntime {
  class Scene;
  class SceneGraphicsView;
  }
}

#include <QWidget>

class BasicSceneView : public QWidget
{
  Q_OBJECT

public:
  explicit BasicSceneView(QWidget* parent = nullptr);
   ~BasicSceneView() override;

private:
  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneGraphicsView* m_sceneView = nullptr;
};

#endif // BASIC_SCENEVIEW_H
