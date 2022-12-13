// [WriteFile Name=GeometryEditorOnMap, Category=DisplayInformation]
// [Legal]
// Copyright 2021 Esri.

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

#ifndef GEOMETRYEDITORONMAP_H
#define GEOMETRYEDITORONMAP_H

namespace Esri::ArcGISRuntime
{
class Map;
class MapQuickView;
class Graphic;
class GraphicsOverlay;
class GeometryEditor;
class GeometryEditorConfiguration;
class SimpleMarkerSymbol;
class SimpleLineSymbol;
class SimpleFillSymbol;
class FeatureLayer;
class Feature;
}

#include <QObject>

Q_MOC_INCLUDE("MapQuickView.h")

class GeometryEditorOnMap : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool geometryEditorStarted READ geometryEditorStarted NOTIFY geometryEditorStartedChanged)
  Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
  Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged)
  Q_PROPERTY(bool canDeleteSelection READ canDeleteSelection NOTIFY canDeleteSelectionChanged)
  Q_PROPERTY(bool isGeoElementSelected READ isGeoElementSelected NOTIFY isGeoElementSelectedChanged)

public:
  explicit GeometryEditorOnMap(QObject* parent = nullptr);
  ~GeometryEditorOnMap();

  enum class SampleCreationMode {
    PointMode,
    MultipointMode,
    PolylineMode,
    PolygonMode
  };

  Q_ENUM(SampleCreationMode)

  static void init();
  Q_INVOKABLE void setCreationMode(GeometryEditorOnMap::SampleCreationMode creationMode);
  Q_INVOKABLE void startGeometry();
  Q_INVOKABLE void stopGeometryEditor(bool saveGeometry);
  Q_INVOKABLE void clearGraphics();
  Q_INVOKABLE void undo();
  Q_INVOKABLE void redo();
  Q_INVOKABLE void deleteSelection();

  Q_INVOKABLE void toggleInteractionEdit(bool checked);
  Q_INVOKABLE void toggleAllowSelectionMove(bool checked);
  Q_INVOKABLE void toggleRequireSelection(bool checked);
  Q_INVOKABLE void toggleVertexSelection(bool checked);
  Q_INVOKABLE void togglePartSelection(bool checked);
  Q_INVOKABLE void toggleGeometrySelection(bool checked);
  Q_INVOKABLE void toggleMidVertexSelection(bool checked);

signals:
  void mapViewChanged();
  void geometryEditorStartedChanged();
  void canUndoChanged();
  void canRedoChanged();
  void canDeleteSelectionChanged();
  void isGeoElementSelectedChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void createSymbols();
  bool geometryEditorStarted() const;
  bool canUndo() const;
  bool canRedo() const;
  bool canDeleteSelection() const;
  bool isGeoElementSelected() const;

  void connectIdentify();
  void connectSignals();

  void deselectGraphics();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::GeometryEditor* m_geometryEditor = nullptr;
  Esri::ArcGISRuntime::Graphic* editingGraphic = nullptr;

  Esri::ArcGISRuntime::SimpleMarkerSymbol* m_pointSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleMarkerSymbol* m_multiPointSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleLineSymbol* m_lineSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleFillSymbol* m_polygonSymbol = nullptr;

  QList<Esri::ArcGISRuntime::FeatureLayer*> m_featureLayers;

  Esri::ArcGISRuntime::Feature* m_feature = nullptr;
  Esri::ArcGISRuntime::Graphic* m_graphic = nullptr;

  Esri::ArcGISRuntime::GeometryEditorConfiguration* m_configuration = nullptr;
};

#endif // GEOMETRYEDITORONMAP_H
