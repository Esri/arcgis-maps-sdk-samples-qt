// [WriteFile Name=RealisticLightingAndShadows, Category=Scenes]
// [Legal]
// Copyright 2020 Esri.

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

#ifndef REALISTICLIGHTINGANDSHADOWS_H
#define REALISTICLIGHTINGANDSHADOWS_H

namespace Esri
{
namespace ArcGISRuntime
{
class Scene;
class SceneQuickView;
}
}

#include <QObject>

class RealisticLightingAndShadows : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)

public:
    explicit RealisticLightingAndShadows(QObject* parent = nullptr);
    ~RealisticLightingAndShadows();

    static void init();

    Q_INVOKABLE void setSunTimeFromValue(const double sunTimeValue);
    Q_INVOKABLE void setLightingMode(const int lightingModeValue);



signals:
    void sceneViewChanged();
    void sunTimeChanged(const QString& sunTimeText);

private:
    Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
    void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);

    Esri::ArcGISRuntime::Scene* m_scene = nullptr;
    Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;

};

#endif // REALISTICLIGHTINGANDSHADOWS_H
