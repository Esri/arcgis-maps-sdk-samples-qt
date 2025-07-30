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
#include "SnapSourcesListModel.h"

// Qt headers
#include <QImage>
#include <QObject>

template <typename T> class QFuture;
class QMouseEvent;

namespace Esri::ArcGISRuntime {
  class ArcGISFeature;
  class Geodatabase;
  class GeometryEditor;
  class IdentifyLayerResult;
  class Map;
  class MapQuickView;
  class Renderer;
  class SimpleLineSymbol;
  class SnapSourceSettings;
  class SubtypeFeatureLayer;
  class SubtypeSublayer;
  class UtilityAssetType;
}

Q_MOC_INCLUDE("MapQuickView.h");

struct LoadOperationalLayersReturnStruct
{
  Esri::ArcGISRuntime::SubtypeFeatureLayer* m_pipeSubtypeLayer = nullptr;
  Esri::ArcGISRuntime::SubtypeFeatureLayer* m_deviceSubtypeLayer = nullptr;
};

class SnapGeometryEditsWithRules : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
    Q_PROPERTY(bool geometryEditorStarted READ geometryEditorStarted NOTIFY geometryEditorStartedChanged)
    Q_PROPERTY(bool isElementSelected READ isElementSelected NOTIFY isElementSelectedChanged)
    Q_PROPERTY(SnapSourcesListModel* snapSourcesListModel READ snapSourcesListModel NOTIFY snapSourceModelChanged)

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
    SnapSourcesListModel* snapSourcesListModel() const;

signals:
    void mapViewChanged();
    void isElementSelectedChanged();
    void geometryEditorStartedChanged();
    void snapSourceModelChanged();
    void assetGroupChanged(const QString& assetGroup);
    void assetTypeChanged(const QString& assetType);

private slots:
    void onMapViewClicked(const QMouseEvent& mouseEvent);

private:
    void initializeMap();

    QFuture<void>  loadGeodatabase();
    QFuture<LoadOperationalLayersReturnStruct> loadOperationalLayers();
    QFuture<void> loadUtilityNetwork();

    void modifyOperationalLayersVisibility(Esri::ArcGISRuntime::SubtypeFeatureLayer* pipeLayer, Esri::ArcGISRuntime::SubtypeFeatureLayer* deviceLayer);
    void updateSnapSourceRenderers(const QList<Esri::ArcGISRuntime::SnapSourceSettings*>& snapSourceSettings);
    Esri::ArcGISRuntime::ArcGISFeature* getFeatureFromResult(const QList<Esri::ArcGISRuntime::IdentifyLayerResult*>& identifyResult);
    void setSnapSettings(Esri::ArcGISRuntime::UtilityAssetType* assetType);
    void resetSelections();

    Esri::ArcGISRuntime::Map* m_map = nullptr;
    Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

    Esri::ArcGISRuntime::Geodatabase* m_geodatabase = nullptr;

    SnapSourcesListModel* m_snapSourcesListModel = nullptr;

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
