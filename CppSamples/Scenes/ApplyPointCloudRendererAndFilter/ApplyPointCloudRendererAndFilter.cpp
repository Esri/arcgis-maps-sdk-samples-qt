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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "ApplyPointCloudRendererAndFilter.h"

// ArcGIS Maps SDK headers
#include "ArcGISTiledElevationSource.h"
#include "ElevationSourceListModel.h"
#include "Envelope.h"
#include "LayerListModel.h"
#include "LocalSceneQuickView.h"
#include "MapTypes.h"
#include "PointCloudBitfieldFilter.h"
#include "PointCloudClassBreaksRenderer.h"
#include "PointCloudColorClassBreak.h"
#include "PointCloudColorStop.h"
#include "PointCloudColorUniqueValue.h"
#include "PointCloudFilterListModel.h"
#include "PointCloudLayer.h"
#include "PointCloudReturnFilter.h"
#include "PointCloudRgbRenderer.h"
#include "PointCloudSplatAlgorithm.h"
#include "PointCloudStretchRenderer.h"
#include "PointCloudTypes.h"
#include "PointCloudUniqueValueRenderer.h"
#include "PointCloudValueFilter.h"
#include "PortalItem.h"
#include "Scene.h"
#include "SceneViewTypes.h"
#include "Surface.h"
#include "Viewpoint.h"

// Qt headers
#include <QColor>
#include <QQmlEngine>
#include <QUrl>

// STL headers
#include <limits>

using namespace Esri::ArcGISRuntime;

ApplyPointCloudRendererAndFilter::ApplyPointCloudRendererAndFilter(QObject *parent /* = nullptr */)
    : QObject(parent)
    , m_scene(new Scene(SceneViewingMode::Local, BasemapStyle::ArcGISStreets, this))
{
    // create a new elevation source from Terrain3D rest service
    ArcGISTiledElevationSource *elevationSource
        = new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/"
                                              "WorldElevation3D/Terrain3D/ImageServer"),
                                         this);

    // add the elevation source to the scene to display elevation
    m_scene->baseSurface()->elevationSources()->append(elevationSource);

    initializePointCloudLayer();
    initializeRenderers();
    initializeFilters();

    applyRGBRenderer();
}

ApplyPointCloudRendererAndFilter::~ApplyPointCloudRendererAndFilter() = default;

void ApplyPointCloudRendererAndFilter::init()
{
    qmlRegisterType<LocalSceneQuickView>("Esri.Samples", 1, 0, "LocalSceneView");
    qmlRegisterType<ApplyPointCloudRendererAndFilter>("Esri.Samples", 1, 0,
                                                       "ApplyPointCloudRendererAndFilterSample");
}

void ApplyPointCloudRendererAndFilter::initializePointCloudLayer()
{
    auto *pointCloudItem = new PortalItem(QStringLiteral("bc963a0adfd7450d8cc11b58510fda8d"), this);
    m_pointCloudLayer = new PointCloudLayer(pointCloudItem, this);

    connect(m_pointCloudLayer, &PointCloudLayer::doneLoading, this, [this]() {
        if (!m_sceneView || m_pointCloudLayer->loadStatus() != LoadStatus::Loaded) {
            return;
        }

        m_sceneView->setViewpointAndWait(Viewpoint(m_pointCloudLayer->fullExtent()));
    });

    m_scene->operationalLayers()->append(m_pointCloudLayer);
}

void ApplyPointCloudRendererAndFilter::initializeRenderers()
{
    m_rgbRenderer = new PointCloudRgbRenderer("RGB", this);

    const QList<PointCloudColorStop *>
        colorStops{new PointCloudColorStop(QColor(31, 79, 255), 0.0, this),
                   new PointCloudColorStop(QColor(33, 163, 102), 30.0, this),
                   new PointCloudColorStop(QColor(229, 57, 53), 90.0, this)};
    m_stretchRenderer = new PointCloudStretchRenderer("ELEVATION", colorStops, this);

    const QList<PointCloudColorClassBreak *>
        classBreaks{new PointCloudColorClassBreak(QColor(96, 67, 151), 0.0, 20.0, this),
                    new PointCloudColorClassBreak(QColor(65, 145, 136), 20.0, 40.0, this),
                    new PointCloudColorClassBreak(QColor(216, 155, 77),
                                                  40.0,
                                                  std::numeric_limits<float>::max(),
                                                  this)};
    m_classBreaksRenderer = new PointCloudClassBreaksRenderer("ELEVATION", classBreaks, this);

    const QList<PointCloudColorUniqueValue *> uniqueValues{
        new PointCloudColorUniqueValue(QColor(139, 178, 194), {QStringLiteral("1")}, this),
        new PointCloudColorUniqueValue(QColor(212, 223, 160), {QStringLiteral("2")}, this),
        new PointCloudColorUniqueValue(QColor(168, 208, 141), {QStringLiteral("3")}, this),
        new PointCloudColorUniqueValue(QColor(112, 173, 71), {QStringLiteral("4")}, this),
        new PointCloudColorUniqueValue(QColor(47, 107, 47), {QStringLiteral("5")}, this),
        new PointCloudColorUniqueValue(QColor(200, 62, 62), {QStringLiteral("6")}, this),
        new PointCloudColorUniqueValue(QColor(187, 185, 220), {QStringLiteral("7")}, this),
        new PointCloudColorUniqueValue(QColor(187, 225, 228), {QStringLiteral("8")}, this),
        new PointCloudColorUniqueValue(QColor(155, 191, 177), {QStringLiteral("9")}, this),
        new PointCloudColorUniqueValue(QColor(75, 85, 99), {QStringLiteral("10")}, this),
        new PointCloudColorUniqueValue(QColor(107, 114, 128), {QStringLiteral("11")}, this),
        new PointCloudColorUniqueValue(QColor(209, 213, 219), {QStringLiteral("12")}, this),
        new PointCloudColorUniqueValue(QColor(245, 158, 11), {QStringLiteral("13")}, this),
        new PointCloudColorUniqueValue(QColor(234, 179, 8), {QStringLiteral("14")}, this),
        new PointCloudColorUniqueValue(QColor(124, 58, 237), {QStringLiteral("15")}, this),
        new PointCloudColorUniqueValue(QColor(236, 72, 153), {QStringLiteral("16")}, this),
        new PointCloudColorUniqueValue(QColor(139, 90, 43), {QStringLiteral("17")}, this),
        new PointCloudColorUniqueValue(QColor(17, 24, 39), {QStringLiteral("18")}, this)};
    m_uniqueValueRenderer = new PointCloudUniqueValueRenderer("CLASS_CODE", uniqueValues, this);

    const QList<PointCloudRenderer *> renderers{m_rgbRenderer,
                                                m_stretchRenderer,
                                                m_classBreaksRenderer,
                                                m_uniqueValueRenderer};
    for (PointCloudRenderer *renderer : renderers) {
        renderer->setSizeAlgorithm(new PointCloudSplatAlgorithm(1.0, renderer));
        renderer->setPointsPerInch(25.0);
    }
}

void ApplyPointCloudRendererAndFilter::initializeFilters()
{
    m_valueFilter = new PointCloudValueFilter("CLASS_CODE",
                                              {},
                                              PointCloudValueFilterMode::Include,
                                              m_pointCloudLayer);
    m_returnFilter = new PointCloudReturnFilter("RETURNS", {}, m_pointCloudLayer);
    m_bitfieldFilter = new PointCloudBitfieldFilter("FLAGS", {}, {}, m_pointCloudLayer);
}

LocalSceneQuickView *ApplyPointCloudRendererAndFilter::sceneView() const
{
    return m_sceneView;
}

// Set the view (created in QML)
void ApplyPointCloudRendererAndFilter::setSceneView(LocalSceneQuickView *sceneView)
{
    if (!sceneView || sceneView == m_sceneView) {
        return;
    }

    m_sceneView = sceneView;
    m_sceneView->setArcGISScene(m_scene);

    emit sceneViewChanged();
}

void ApplyPointCloudRendererAndFilter::applyRGBRenderer()
{
    m_pointCloudLayer->setRenderer(m_rgbRenderer);
}

void ApplyPointCloudRendererAndFilter::applyStretchRenderer()
{
    m_pointCloudLayer->setRenderer(m_stretchRenderer);
}

void ApplyPointCloudRendererAndFilter::applyClassBreaksRenderer()
{
    m_pointCloudLayer->setRenderer(m_classBreaksRenderer);
}

void ApplyPointCloudRendererAndFilter::applyUniqueValueRenderer()
{
    m_pointCloudLayer->setRenderer(m_uniqueValueRenderer);
}

void ApplyPointCloudRendererAndFilter::setPointSize(double pointSize)
{
    auto *splatAlgorithm = qobject_cast<PointCloudSplatAlgorithm *>(
        m_pointCloudLayer->renderer()->sizeAlgorithm());
    splatAlgorithm->setScaleFactor(pointSize);
}

void ApplyPointCloudRendererAndFilter::applyValueFilter()
{
    auto *filters = m_pointCloudLayer->filters();
    if (!filters->contains(m_valueFilter)) {
        filters->append(m_valueFilter);
    }
}

void ApplyPointCloudRendererAndFilter::toggleClassCode(int classCode)
{
    QList<double> classCodes = m_valueFilter->values();
    if (classCodes.contains(classCode)) {
        classCodes.removeAll(classCode);
    } else {
        classCodes.append(classCode);
    }

    m_valueFilter->setValues(classCodes);
}

void ApplyPointCloudRendererAndFilter::toggleValueFilterMode(bool excluded)
{
    m_valueFilter->setMode(excluded ? PointCloudValueFilterMode::Exclude
                                    : PointCloudValueFilterMode::Include);
}

void ApplyPointCloudRendererAndFilter::clearValueFilter()
{
    m_valueFilter->setValues({});
    m_pointCloudLayer->filters()->removeOne(m_valueFilter);
}

void ApplyPointCloudRendererAndFilter::applyReturnFilter()
{
    auto *filters = m_pointCloudLayer->filters();
    if (!filters->contains(m_returnFilter)) {
        filters->append(m_returnFilter);
    }
}

void ApplyPointCloudRendererAndFilter::toggleReturnType(int returnType)
{
    QList<PointCloudReturnType> returnTypes = m_returnFilter->includedReturns();
    const auto pointCloudReturnType = static_cast<PointCloudReturnType>(returnType);
    if (returnTypes.contains(pointCloudReturnType)) {
        returnTypes.removeAll(pointCloudReturnType);
    } else {
        returnTypes.append(pointCloudReturnType);
    }

    m_returnFilter->setIncludedReturns(returnTypes);
}

void ApplyPointCloudRendererAndFilter::clearReturnFilter()
{
    m_returnFilter->setIncludedReturns({});
    m_pointCloudLayer->filters()->removeOne(m_returnFilter);
}

void ApplyPointCloudRendererAndFilter::applyBitfieldFilter()
{
    auto *filters = m_pointCloudLayer->filters();
    if (!filters->contains(m_bitfieldFilter)) {
        filters->append(m_bitfieldFilter);
    }
}

void ApplyPointCloudRendererAndFilter::toggleBitfieldBit(int bitPosition, bool setBit)
{
    QList<quint32> requiredClearBits = m_bitfieldFilter->requiredClearBits();
    QList<quint32> requiredSetBits = m_bitfieldFilter->requiredSetBits();

    if (setBit) { // If this is a set bit and not a clear bit
        requiredClearBits.removeAll(
            bitPosition); // Remove the clear bit if it exists to ensure mutually exclusive bit positions between set and clear
        if (requiredSetBits.contains(
                bitPosition)) { // If the set bit already exists, toggle it off by removing it from the list of required set bits
            requiredSetBits.removeAll(bitPosition);
        } else {
            requiredSetBits.append(
                bitPosition); // Else toggle it on by adding it to the list of required set bits
        }
    } else { // If this is a clear bit and not a set bit
        requiredSetBits.removeAll(
            bitPosition); // Remove the clear bit if it exists to ensure mutually exclusive bit positions between set and clear
        if (requiredClearBits.contains(
                bitPosition)) { // If the clear bit already exists, toggle it off by removing it from the list of required clear bits
            requiredClearBits.removeAll(bitPosition);
        } else {
            requiredClearBits.append(
                bitPosition); // Else toggle it on by adding it to the list of required clear bits
        }
    }

    m_bitfieldFilter->setRequiredClearBits(requiredClearBits);
    m_bitfieldFilter->setRequiredSetBits(requiredSetBits);
}

void ApplyPointCloudRendererAndFilter::clearBitfieldFilter()
{
    m_bitfieldFilter->setRequiredClearBits({});
    m_bitfieldFilter->setRequiredSetBits({});
    m_pointCloudLayer->filters()->removeOne(m_bitfieldFilter);
}
