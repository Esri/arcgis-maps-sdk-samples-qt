// [WriteFile Name=DisplayGeometryEditorInformationDuringInteraction, Category=EditData]
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

#ifndef DISPLAYGEOMETRYEDITORINFORMATIONDURINGINTERACTION_H
#define DISPLAYGEOMETRYEDITORINFORMATIONDURINGINTERACTION_H

// Qt headers
#include <QObject>
#include <QString>

class QMouseEvent;

namespace Esri::ArcGISRuntime
{
  class Geometry;
  class GeometryEditor;
  class GeometryEditorInteractionPreview;
  class Graphic;
  class GraphicsOverlay;
  class Map;
  class MapQuickView;
  class Point;
} // namespace Esri::ArcGISRuntime

Q_MOC_INCLUDE("MapQuickView.h");

class DisplayGeometryEditorInformationDuringInteraction : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool geometryEditorStarted READ geometryEditorStarted NOTIFY geometryEditorStartedChanged)
  Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
  Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged)
  Q_PROPERTY(bool interactionInfoVisible MEMBER m_interactionInfoVisible NOTIFY interactionInfoChanged)
  Q_PROPERTY(QString interactionDescription MEMBER m_interactionDescription NOTIFY interactionInfoChanged)
  Q_PROPERTY(QString interactionValue MEMBER m_interactionValue NOTIFY interactionInfoChanged)

public:
  explicit DisplayGeometryEditorInformationDuringInteraction(QObject* parent = nullptr);
  ~DisplayGeometryEditorInformationDuringInteraction() override;

  static void init();

  Q_INVOKABLE void undo();
  Q_INVOKABLE void redo();
  Q_INVOKABLE void cancelEdits();
  Q_INVOKABLE void saveEdits();

signals:
  void mapViewChanged();
  void geometryEditorStartedChanged();
  void canUndoChanged();
  void canRedoChanged();
  void interactionInfoChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  bool geometryEditorStarted() const;
  bool canUndo() const;
  bool canRedo() const;

  void createInitialGraphics();
  void onMapClicked(const QMouseEvent& mouseEvent);
  void onInteractionPreviewChanged(Esri::ArcGISRuntime::GeometryEditorInteractionPreview* interactionPreview);
  void setScaleInformation(const Esri::ArcGISRuntime::Geometry& previewGeometry);
  void setRotationInformation(const Esri::ArcGISRuntime::Geometry& previewGeometry);
  void setMoveInformation(const Esri::ArcGISRuntime::Geometry& previewGeometry);
  void finishEditing(bool save);
  void clearInteractionInformation();

  static Esri::ArcGISRuntime::Point firstPoint(const Esri::ArcGISRuntime::Geometry& geometry);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::GeometryEditor* m_geometryEditor = nullptr;
  Esri::ArcGISRuntime::Graphic* m_editingGraphic = nullptr;
  QString m_interactionDescription;
  QString m_interactionValue;
  bool m_interactionInfoVisible = false;
};

#endif // DISPLAYGEOMETRYEDITORINFORMATIONDURINGINTERACTION_H
