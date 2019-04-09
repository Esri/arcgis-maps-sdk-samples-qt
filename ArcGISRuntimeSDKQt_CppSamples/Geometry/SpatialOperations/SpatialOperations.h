// [WriteFile Name=SpatialOperations, Category=Geometry]
// [Legal]
// Copyright 2018 Esri.

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

#ifndef SpatialOperations_H
#define SpatialOperations_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class GraphicsOverlay;
  }
}

#include <QQuickItem>
#include <QStringList>
#include "Geometry.h"
class SpatialOperations : public QQuickItem
{
  Q_OBJECT
  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapQuickView WRITE setMapQuickView NOTIFY mapQuickViewChanged)
  Q_PROPERTY(QStringList geometryOperations READ geometryOperations CONSTANT)

public:
  explicit SpatialOperations(QQuickItem* parent = nullptr);
  ~SpatialOperations() override = default;

  static void init();
  void componentComplete() override;  
  Q_INVOKABLE void applyGeometryOperation(int index);

signals:
  void mapQuickViewChanged();

private:
  void addPolygons();
  void addOutput(const Esri::ArcGISRuntime::Geometry& geometry);
  QStringList geometryOperations() const { return m_geometryOperations; }

  Esri::ArcGISRuntime::MapQuickView* mapQuickView() const;
  void setMapQuickView(Esri::ArcGISRuntime::MapQuickView* mapQuickView);

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Geometry m_polygon1;
  Esri::ArcGISRuntime::Geometry m_polygon2;
  Esri::ArcGISRuntime::GraphicsOverlay* m_inputsOverlay = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_outputsOverlay = nullptr;
  QStringList m_geometryOperations;
};

#endif // SpatialOperations_H
