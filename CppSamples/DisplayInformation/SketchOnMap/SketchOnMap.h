// [WriteFile Name=SketchOnMap, Category=DisplayInformation]
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

#ifndef SKETCHONMAP_H
#define SKETCHONMAP_H

namespace Esri::ArcGISRuntime
{
class Map;
class MapQuickView;
class Graphic;
class GraphicsOverlay;
class SketchEditor;
class SimpleMarkerSymbol;
class SimpleLineSymbol;
class SimpleFillSymbol;
}

#include <QObject>

Q_MOC_INCLUDE("MapQuickView.h")

class SketchOnMap : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool sketchEditorStarted READ sketchEditorStarted NOTIFY sketchEditorStartedChanged)

public:
  explicit SketchOnMap(QObject* parent = nullptr);
  ~SketchOnMap();

  enum class SampleSketchMode {
    PointSketchMode,
    MultipointSketchMode,
    PolylineSketchMode,
    PolygonSketchMode
  };

  Q_ENUM(SampleSketchMode)

  static void init();
  Q_INVOKABLE void setSketchCreationMode(SketchOnMap::SampleSketchMode sketchCreationMode);
  Q_INVOKABLE void stopSketching(bool saveGeometry);
  Q_INVOKABLE void clearGraphics();
  Q_INVOKABLE void undo();
  Q_INVOKABLE void redo();
  Q_INVOKABLE void deleteVertex();

signals:
  void mapViewChanged();
  void sketchEditorStartedChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void createConnections();
  void createSymbols();
  bool sketchEditorStarted() const;

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_sketchOverlay = nullptr;
  Esri::ArcGISRuntime::SketchEditor* m_sketchEditor = nullptr;
  Esri::ArcGISRuntime::Graphic* editingGraphic = nullptr;

  Esri::ArcGISRuntime::SimpleMarkerSymbol* m_pointSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleMarkerSymbol* m_multiPointSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleLineSymbol* m_lineSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleFillSymbol* m_polygonSymbol = nullptr;
};

#endif // SKETCHONMAP_H
