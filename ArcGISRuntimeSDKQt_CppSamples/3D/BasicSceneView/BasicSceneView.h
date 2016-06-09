// [WriteFile Name=BasicSceneView, Category=3D]
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

#ifndef BASIC_SCENEVIEW_H
#define BASICS_SCENEVIEW_H

namespace Esri
{
    namespace ArcGISRuntime
    {
        class Scene;
        class SceneQuickView;
    }
}

#include <QQuickItem>

class BasicSceneView : public QQuickItem
{
    Q_OBJECT

public:
    BasicSceneView(QQuickItem* parent = 0);
    ~BasicSceneView();

    void componentComplete() Q_DECL_OVERRIDE;

private:
    Esri::ArcGISRuntime::Scene* m_scene;
    Esri::ArcGISRuntime::SceneQuickView* m_sceneView;
};

#endif // BASIC_SCENEVIEW_H

