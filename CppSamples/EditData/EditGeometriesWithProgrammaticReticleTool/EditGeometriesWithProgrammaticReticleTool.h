// [WriteFile Name=EditGeometriesWithProgrammaticReticleTool, Category=EditData]
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

#ifndef EDITGEOMETRIESWITHPROGRAMMATICRETICLETOOL_H
#define EDITGEOMETRIESWITHPROGRAMMATICRETICLETOOL_H

// ArcGIS Maps SDK headers
#include "GeometryTypes.h"

// Qt headers
#include <QObject>

class QMouseEvent;

namespace Esri::ArcGISRuntime
{
  class GeometryEditor;
  class Graphic;
  class GraphicsOverlay;
  class Map;
  class MapQuickView;
  class ProgrammaticReticleTool;
  class SimpleMarkerSymbol;
  class SimpleLineSymbol;
  class SimpleFillSymbol;
  class Symbol;
}

Q_MOC_INCLUDE("MapQuickView.h");

class EditGeometriesWithProgrammaticReticleTool : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool geometryEditorStarted READ geometryEditorStarted NOTIFY geometryEditorStartedChanged)
  Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoOrRedoChanged)
  Q_PROPERTY(bool canRedo READ canRedo NOTIFY canUndoOrRedoChanged)
  Q_PROPERTY(bool vertexCreationAllowed READ vertexCreationAllowed WRITE setVertexCreationAllowed NOTIFY vertexCreationAllowedChanged)
  Q_PROPERTY(GeometryEditorMode selectedGeometryType READ selectedGeometryType WRITE setSelectedGeometryType NOTIFY selectedGeometryTypeChanged)
  Q_PROPERTY(QString multifunctionButtonText READ multifunctionButtonText NOTIFY multifunctionButtonTextChanged)
  Q_PROPERTY(bool multifunctionButtonEnabled READ multifunctionButtonEnabled NOTIFY multifunctionButtonEnabledChanged)
  Q_PROPERTY(bool selectedElementCanDelete READ selectedElementCanDelete NOTIFY selectedElementCanDeleteChanged)

public:
  explicit EditGeometriesWithProgrammaticReticleTool(QObject* parent = nullptr);
  ~EditGeometriesWithProgrammaticReticleTool() override;

  enum class GeometryEditorMode {
    PointMode,
    MultipointMode,
    PolylineMode,
    PolygonMode
  };

  Q_ENUM(GeometryEditorMode)

  enum class ReticleState {
    Default,
    PickedUp,
    HoveringVertex,
    HoveringMidVertex
  };

  static void init();
  Q_INVOKABLE void handleMultifunctionButton();
  Q_INVOKABLE void undoOrCancel();
  Q_INVOKABLE void redo();
  Q_INVOKABLE void saveEdits();
  Q_INVOKABLE void discardEdits();
  Q_INVOKABLE void deleteSelectedElement();

signals:
  void mapViewChanged();
  void geometryEditorStartedChanged();
  void canUndoOrRedoChanged();
  void vertexCreationAllowedChanged();
  void selectedGeometryTypeChanged();
  void multifunctionButtonTextChanged();
  void multifunctionButtonEnabledChanged();
  void selectedElementCanDeleteChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  bool geometryEditorStarted() const;

  bool canUndo() const;
  bool canRedo() const;

  bool vertexCreationAllowed() const;
  void setVertexCreationAllowed(bool allowed);

  GeometryEditorMode selectedGeometryType() const;
  void setSelectedGeometryType(GeometryEditorMode mode);

  QString multifunctionButtonText() const;
  bool multifunctionButtonEnabled() const;

  bool selectedElementCanDelete() const;
  void createInitialGraphics();
  void createConnections();
  void setMultifunctionButtonState();
  void resetFromEditingSession();
  void handleMapTap(const QMouseEvent& mouseEvent);
  Esri::ArcGISRuntime::Symbol* getSymbol(Esri::ArcGISRuntime::GeometryType type) const;

  bool m_vertexCreationAllowed = true;
  bool m_multifunctionButtonEnabled = true;
  bool m_selectedElementCanDelete = false;
  QString m_multifunctionButtonText = "Start geometry editor";
  ReticleState m_reticleState = ReticleState::Default;
  GeometryEditorMode m_selectedGeometryEditorType = GeometryEditorMode::PointMode;

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::Graphic* m_editingGraphic = nullptr;

  Esri::ArcGISRuntime::SimpleMarkerSymbol* m_pointSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleMarkerSymbol* m_multiPointSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleLineSymbol* m_lineSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleFillSymbol* m_polygonSymbol = nullptr;

  Esri::ArcGISRuntime::GeometryEditor* m_geometryEditor = nullptr;
  Esri::ArcGISRuntime::ProgrammaticReticleTool* m_reticleTool = nullptr;
};

#endif // EDITGEOMETRIESWITHPROGRAMMATICRETICLETOOL_H
