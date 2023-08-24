// [WriteFile Name=CreateAndEditGeometries, Category=Geometry]
// [Legal]
// Copyright 2023 Esri.

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

#ifndef CREATEANDEDITGEOMETRIES_H
#define CREATEANDEDITGEOMETRIES_H

namespace Esri::ArcGISRuntime
{
  class GeometryEditor;
  class Graphic;
  class GraphicsOverlay;
  class FreehandTool;
  class Map;
  class MapQuickView;
  class ShapeTool;
  class SimpleMarkerSymbol;
  class SimpleLineSymbol;
  class SimpleFillSymbol;
  class VertexTool;
}

#include <QObject>

Q_MOC_INCLUDE("MapQuickView.h");

class CreateAndEditGeometries : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool geometryEditorStarted READ geometryEditorStarted NOTIFY geometryEditorStartedChanged)
  Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoOrRedoChanged)
  Q_PROPERTY(bool canRedo READ canRedo NOTIFY canUndoOrRedoChanged)
  Q_PROPERTY(bool elementIsSelected READ elementIsSelected NOTIFY elementIsSelectedChanged)

public:
  explicit CreateAndEditGeometries(QObject* parent = nullptr);
  ~CreateAndEditGeometries() override;

  enum class GeometryEditorMode {
    PointMode,
    MultipointMode,
    PolylineMode,
    PolygonMode
  };

  Q_ENUM(GeometryEditorMode)

  enum class GeometryEditorToolType {
    Freehand,
    Vertex,
    Arrow,
    Ellipse,
    Rectangle,
    Triangle
  };

  Q_ENUM(GeometryEditorToolType)

  static void init();

  Q_INVOKABLE void startGeometryEditorWithGeometryType(GeometryEditorMode geometryEditorMode);
  Q_INVOKABLE void stopEditing(bool saveGeometry);
  Q_INVOKABLE void setTool(GeometryEditorToolType toolType);
  Q_INVOKABLE void clearGraphics();
  Q_INVOKABLE void undo();
  Q_INVOKABLE void redo();
  Q_INVOKABLE void deleteSelectedElement();
  Q_INVOKABLE void setUniformScaleMode(bool isUniformScale);

signals:
  void mapViewChanged();
  void geometryEditorStartedChanged();
  void canUndoOrRedoChanged();
  void elementIsSelectedChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  bool geometryEditorStarted() const;
  bool canUndo();
  bool canRedo();
  bool elementIsSelected();

  void createInitialSymbols();
  void createInitialGraphics();
  void createConnections();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::Graphic* m_editingGraphic = nullptr;

  Esri::ArcGISRuntime::SimpleMarkerSymbol* m_pointSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleMarkerSymbol* m_multiPointSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleLineSymbol* m_lineSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleFillSymbol* m_polygonSymbol = nullptr;

  Esri::ArcGISRuntime::GeometryEditor* m_geometryEditor = nullptr;

  // Tools
  Esri::ArcGISRuntime::VertexTool* m_vertexTool = nullptr;
  Esri::ArcGISRuntime::FreehandTool* m_freehandTool = nullptr;
  Esri::ArcGISRuntime::ShapeTool* m_arrowTool = nullptr;
  Esri::ArcGISRuntime::ShapeTool* m_ellipseTool = nullptr;
  Esri::ArcGISRuntime::ShapeTool* m_rectangleTool = nullptr;
  Esri::ArcGISRuntime::ShapeTool* m_triangleTool = nullptr;

  QObject* m_tempGraphicsParent = nullptr;
};

#endif // CREATEANDEDITGEOMETRIES_H
