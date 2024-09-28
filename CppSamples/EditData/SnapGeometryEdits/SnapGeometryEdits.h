// [WriteFile Name=SnapGeometryEdits, Category=EditData]
// [Legal]
// Copyright 2024 Esri.
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

#ifndef SNAPGEOMETRYEDITS_H
#define SNAPGEOMETRYEDITS_H

// Qt headers
#include <QObject>

namespace Esri::ArcGISRuntime {
  class Geometry;
  class GeometryEditor;
  class GeometryEditor;
  class Graphic;
  class GraphicsOverlay;
  class Map;
  class MapQuickView;
  class Portal;
  class PortalItem;
  class SimpleFillSymbol;
  class SimpleLineSymbol;
  class SimpleMarkerSymbol;
  class SnapSourceSettings;
  class Symbol;
}

class SnapSourceListModel;
class QAbstractListModel;

Q_MOC_INCLUDE("MapQuickView.h");
Q_MOC_INCLUDE("QAbstractListModel");

class SnapGeometryEdits : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool geometryEditorStarted READ geometryEditorStarted NOTIFY geometryEditorStartedChanged)
  Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
  Q_PROPERTY(bool isElementSelected READ isElementSelected NOTIFY isElementSelectedChanged)
  Q_PROPERTY(bool layersLoaded MEMBER m_layersLoaded NOTIFY layersLoadedChanged)
  Q_PROPERTY(QAbstractListModel* snapSourceModel READ snapSourceListModel NOTIFY snapSourceModelChanged)

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
  Q_INVOKABLE void stopEditing();
  Q_INVOKABLE void deleteSelection();
  Q_INVOKABLE void editorUndo();
  Q_INVOKABLE void snappingEnabledStatus(bool checkedValue);
  Q_INVOKABLE void geometryGuidesEnabledStatus(bool checkedValue);
  Q_INVOKABLE void featureSnappingEnabledStatus(bool checkedValue);
  Q_INVOKABLE void displaySnapSources();
  Q_INVOKABLE void enableAllLayersInSection(const QString& section);

signals:
  void canUndoChanged();
  void isElementSelectedChanged();
  void geometryEditorStartedChanged();
  void layersLoadedChanged();
  void mapViewChanged();
  void pointLayersChanged();
  void pointSourceCheckedStateChanged();
  void polylineLayersChanged();
  void polylineSourceCheckedStateChanged();
  void snapSourceModelChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  bool geometryEditorStarted() const;
  bool canUndo();
  void createInitialSymbols();
  void createConnections();
  bool isElementSelected();
  QAbstractListModel* snapSourceListModel() const;
  Esri::ArcGISRuntime::Symbol* determineGeometrySymbol(const Esri::ArcGISRuntime::Geometry& geometry);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::Graphic* m_editingGraphic = nullptr;
  Esri::ArcGISRuntime::GeometryEditor* m_geometryEditor = nullptr;
  Esri::ArcGISRuntime::SimpleMarkerSymbol* m_pointSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleMarkerSymbol* m_multiPointSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleLineSymbol* m_lineSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleFillSymbol* m_polygonSymbol = nullptr;
  bool m_layersLoaded = false;
  SnapSourceListModel* m_snapSourceListModel = nullptr;
};

#endif // SNAPGEOMETRYEDITS_H
