// [WriteFile Name=FeatureLayerExtrusion, Category=Scenes]
// [Legal]
// Copyright 2017 Esri.

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

#ifndef FEATURELAYEREXTRUSION_H
#define FEATURELAYEREXTRUSION_H

namespace Esri
{
namespace ArcGISRuntime
{
class FeatureLayer;
class ServiceFeatureTable;
class SceneQuickView;
class OrbitLocationCameraController;
class SimpleLineSymbol;
class SimpleFillSymbol;
class SimpleRenderer;
enum class FeatureRenderingMode;
}
}

#include <QQuickItem>
#include "Point.h"
#include "Camera.h"

class FeatureLayerExtrusion : public QQuickItem
{
    Q_OBJECT

public:
    Q_INVOKABLE void popDensity();
    explicit FeatureLayerExtrusion(QQuickItem* parent = nullptr);
    ~FeatureLayerExtrusion() = default;

    void componentComplete() Q_DECL_OVERRIDE;
    static void init();

private:
    Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
    Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;
    Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable = nullptr;
    Esri::ArcGISRuntime::Camera m_camera;
    Esri::ArcGISRuntime::Point m_lookAtPoint;
    Esri::ArcGISRuntime::OrbitLocationCameraController* m_orbitController = nullptr;
    Esri::ArcGISRuntime::SimpleLineSymbol* m_lineSymbol = nullptr;
    Esri::ArcGISRuntime::SimpleFillSymbol* m_fillSymbol = nullptr;
    Esri::ArcGISRuntime::SimpleRenderer* renderer = nullptr;


};

#endif // FEATURELAYEREXTRUSION_H
