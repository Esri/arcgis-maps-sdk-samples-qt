// [WriteFile Name=SnapGeometryEdits, Category=EditData]
// [Legal]
// Copyright 2024 Esri.

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

#ifndef SNAPGEOMETRYEDITS_H
#define SNAPGEOMETRYEDITS_H

#include "GeometryEditor.h"
namespace Esri::ArcGISRuntime {
  class Map;
  class MapQuickView;
  class Portal;
  class PortalItem;
  class GeometryEditor;
  class Graphic;
  class GraphicsOverlay;
  class SimpleMarkerSymbol;
  class SimpleLineSymbol;
  class SimpleFillSymbol;
  class SnapSourceSettings;
}

#include <QObject>

Q_MOC_INCLUDE("MapQuickView.h");

class SnapGeometryEdits : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool geometryEditorStarted READ geometryEditorStarted NOTIFY geometryEditorStartedChanged)
  Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
  Q_PROPERTY(bool elementIsSelected READ elementIsSelected NOTIFY elementIsSelectedChanged)
  Q_PROPERTY(QList<QString> pointLayers MEMBER m_pointLayers NOTIFY pointLayersChanged)
  Q_PROPERTY(QList<QString> polylineLayers MEMBER m_polylineLayers NOTIFY polylineLayersChanged)
  Q_PROPERTY(QList<bool> pointSourceCheckedState MEMBER m_pointSourceCheckedState NOTIFY pointSourceCheckedStateChanged)
  Q_PROPERTY(QList<bool> polylineSourceCheckedState MEMBER m_polylineSourceCheckedState NOTIFY polylineSourceCheckedStateChanged)

public:
  explicit SnapGeometryEdits(QObject* parent = nullptr);
  ~SnapGeometryEdits() override;

  enum class GeometryEditorMode {
    PointMode,
    MultipointMode,
    PolylineMode,
    PolygonMode
  };

  Q_ENUM(GeometryEditorMode)

  static void init();
  Q_INVOKABLE void startEditor(GeometryEditorMode geometryEditorMode);
  Q_INVOKABLE void stopEditor();
  Q_INVOKABLE void deleteSelection();
  Q_INVOKABLE void editorUndo();
  Q_INVOKABLE void snappingEnabledStatus(bool checkedValue);
  Q_INVOKABLE void onPointLayersEnabled();
  Q_INVOKABLE void onPolylineLayersEnabled();
  Q_INVOKABLE void configureSnapping();
  Q_INVOKABLE void pointSourceEnabledStatus(bool snappingCheckedState, int index);
  Q_INVOKABLE void polylineSourceEnabledStatus(bool snappingCheckedState, int index);

signals:
  void mapViewChanged();
  void geometryEditorStartedChanged();
  void canUndoChanged();
  void elementIsSelectedChanged();
  void polylineLayersChanged();
  void pointLayersChanged();
  void pointSourceCheckedStateChanged();
  void polylineSourceCheckedStateChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  bool geometryEditorStarted() const;
  void createInitialSymbols();
  void createConnections();
  bool canUndo();
  bool elementIsSelected();



  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Portal* m_portal = nullptr;
  Esri::ArcGISRuntime::PortalItem* m_portalItem = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::Graphic* m_editingGraphic = nullptr;
  Esri::ArcGISRuntime::GeometryEditor* m_geometryEditor = nullptr;
  Esri::ArcGISRuntime::SimpleMarkerSymbol* m_pointSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleMarkerSymbol* m_multiPointSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleLineSymbol* m_lineSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleFillSymbol* m_polygonSymbol = nullptr;
  QList<bool> m_pointSourceCheckedState;
  QList<bool> m_polylineSourceCheckedState;
  QList<Esri::ArcGISRuntime::SnapSourceSettings*> m_snapSourceList;
  QList<QString> m_pointLayers;
  QList<QString> m_polylineLayers;
  QObject* m_tempGraphicsParent = nullptr;
};

#endif // SNAPGEOMETRYEDITS_H
