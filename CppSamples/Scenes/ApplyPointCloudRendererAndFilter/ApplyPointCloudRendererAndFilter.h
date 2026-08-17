// [WriteFile Name=ApplyPointCloudRendererAndFilter, Category=Scenes]
// [Legal]
// Copyright 2026 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifndef APPLYPOINTCLOUDRENDERERANDFILTER_H
#define APPLYPOINTCLOUDRENDERERANDFILTER_H

// Qt headers
#include <QObject>

namespace Esri::ArcGISRuntime {
class LocalSceneQuickView;
class PointCloudBitfieldFilter;
class PointCloudLayer;
class PointCloudRenderer;
class PointCloudReturnFilter;
class PointCloudValueFilter;
class Scene;
} // namespace Esri::ArcGISRuntime

Q_MOC_INCLUDE("LocalSceneQuickView.h")

class ApplyPointCloudRendererAndFilter : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Esri::ArcGISRuntime::LocalSceneQuickView *sceneView READ sceneView WRITE setSceneView
                   NOTIFY sceneViewChanged)

public:
    explicit ApplyPointCloudRendererAndFilter(QObject *parent = nullptr);
    ~ApplyPointCloudRendererAndFilter() override;

    static void init();

    Q_INVOKABLE void applyRGBRenderer();
    Q_INVOKABLE void applyStretchRenderer();
    Q_INVOKABLE void applyClassBreaksRenderer();
    Q_INVOKABLE void applyUniqueValueRenderer();
    Q_INVOKABLE void setPointSize(double pointSize);

    Q_INVOKABLE void applyValueFilter();
    Q_INVOKABLE void toggleClassCode(int classCode);
    Q_INVOKABLE void toggleValueFilterMode(bool excluded);
    Q_INVOKABLE void clearValueFilter();

    Q_INVOKABLE void applyReturnFilter();
    Q_INVOKABLE void toggleReturnType(int returnType);
    Q_INVOKABLE void clearReturnFilter();

    Q_INVOKABLE void applyBitfieldFilter();
    Q_INVOKABLE void toggleBitfieldBit(int bitPosition, bool setBit);
    Q_INVOKABLE void clearBitfieldFilter();

signals:
    void sceneViewChanged();

private:
    Esri::ArcGISRuntime::LocalSceneQuickView *sceneView() const;
    void setSceneView(Esri::ArcGISRuntime::LocalSceneQuickView *sceneView);

    void initializePointCloudLayer();
    void initializeRenderers();
    void initializeFilters();

    Esri::ArcGISRuntime::Scene *m_scene = nullptr;
    Esri::ArcGISRuntime::LocalSceneQuickView *m_sceneView = nullptr;
    Esri::ArcGISRuntime::PointCloudLayer *m_pointCloudLayer = nullptr;
    Esri::ArcGISRuntime::PointCloudRenderer *m_rgbRenderer = nullptr;
    Esri::ArcGISRuntime::PointCloudRenderer *m_stretchRenderer = nullptr;
    Esri::ArcGISRuntime::PointCloudRenderer *m_classBreaksRenderer = nullptr;
    Esri::ArcGISRuntime::PointCloudRenderer *m_uniqueValueRenderer = nullptr;
    Esri::ArcGISRuntime::PointCloudValueFilter *m_valueFilter = nullptr;
    Esri::ArcGISRuntime::PointCloudReturnFilter *m_returnFilter = nullptr;
    Esri::ArcGISRuntime::PointCloudBitfieldFilter *m_bitfieldFilter = nullptr;
};

#endif // APPLYPOINTCLOUDRENDERERANDFILTER_H
