// [Legal]
// Copyright 2025 Esri.
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

#ifndef SNAPGEOMETRYEDITSWITHRULES_H
#define SNAPGEOMETRYEDITSWITHRULES_H

// sample headers
#include "SnapSourceListModel.h"

// Qt headers
#include <QImage>
#include <QObject>

class QMouseEvent;

namespace Esri::ArcGISRuntime {
    class UtilityAssetType;
    class FeatureLayer;
    class IdentifyLayerResult;
    class Map;
    class MapQuickView;
    class GraphicsOverlay;
    class GeometryEditor;
    class Geodatabase;
    class SubtypeSublayer;
    class SubtypeFeatureLayer;
    class SnapSourceSettings;
    class ArcGISFeature;
    class UtilityNetwork;
    class Renderer;
    class SimpleLineSymbol;
}

Q_MOC_INCLUDE("MapQuickView.h");

template <typename T> class QFuture;

class SnapGeometryEditsWithRules : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
    Q_PROPERTY(bool geometryEditorStarted READ geometryEditorStarted NOTIFY geometryEditorStartedChanged)
    Q_PROPERTY(bool isElementSelected READ isElementSelected NOTIFY isElementSelectedChanged)
    Q_PROPERTY(SnapSourceListModel* snapSourceListModel READ snapSourceListModel NOTIFY snapSourceModelChanged)

public:
    explicit SnapGeometryEditsWithRules(QObject* parent = nullptr);
    ~SnapGeometryEditsWithRules() override;

    static void init();
    Q_INVOKABLE void startEditor();
    Q_INVOKABLE void stopEditing();
    Q_INVOKABLE void discardEdits();

    Esri::ArcGISRuntime::MapQuickView* mapView() const;
    void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

    bool isElementSelected() const;
    bool geometryEditorStarted() const;
    SnapSourceListModel* snapSourceListModel() const;

signals:
    void mapViewChanged();
    void isElementSelectedChanged();
    void geometryEditorStartedChanged();
    void snapSourceModelChanged();
    void assetGroupChanged(const QString& assetGroup);
    void assetTypeChanged(const QString& assetType);

private:
    void initializeMap();
    QFuture<void>  loadGeodatabase();
    QFuture<std::pair<Esri::ArcGISRuntime::SubtypeFeatureLayer*, Esri::ArcGISRuntime::SubtypeFeatureLayer*>> loadOperationalLayers();
    QFuture<void> loadUtilityNetwork();
    void connectSignals();
    void onMapViewClicked(const QMouseEvent& mouseEvent);
    void modifyOperationalLayersVisibility(Esri::ArcGISRuntime::SubtypeFeatureLayer* pipeLayer, Esri::ArcGISRuntime::SubtypeFeatureLayer* deviceLayer);
    void modifySnapSourceRenderers(const QList<Esri::ArcGISRuntime::SnapSourceSettings*> snapSourceSettings);
    Esri::ArcGISRuntime::ArcGISFeature* getFeatureFromResult(const QList<Esri::ArcGISRuntime::IdentifyLayerResult*>& identifyResult);
    void setSnapSettings(Esri::ArcGISRuntime::UtilityAssetType* assetType);
    void resetSelections();

    Esri::ArcGISRuntime::Map* m_map = nullptr;
    Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
    Esri::ArcGISRuntime::Geodatabase* m_geodatabase = nullptr;
    SnapSourceListModel* m_snapSourceListModel;

    Esri::ArcGISRuntime::Renderer* m_defaultDistributionRenderer = nullptr;
    Esri::ArcGISRuntime::Renderer* m_defaultServiceRenderer = nullptr;
    Esri::ArcGISRuntime::Renderer* m_defaultGraphicsOverlayRenderer = nullptr;
    
    Esri::ArcGISRuntime::SubtypeSublayer* m_distributionPipeLayer = nullptr;
    Esri::ArcGISRuntime::SubtypeSublayer* m_servicePipeLayer = nullptr;

    Esri::ArcGISRuntime::SimpleLineSymbol* m_rulesLimitSymbol = nullptr;
    Esri::ArcGISRuntime::SimpleLineSymbol* m_rulesPreventSymbol = nullptr;
    Esri::ArcGISRuntime::SimpleLineSymbol* m_noneSymbol = nullptr;

    Esri::ArcGISRuntime::ArcGISFeature* m_selectedFeature = nullptr;

};

#endif // SNAPGEOMETRYEDITSWITHRULES_H
