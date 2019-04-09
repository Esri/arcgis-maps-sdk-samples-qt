// [WriteFile Name=SpatialRelationships, Category=Geometry]
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

#ifndef SPATIALRELATIONSHIPS_H
#define SPATIALRELATIONSHIPS_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class Graphic;
class GraphicsOverlay;
}
}

#include "Geometry.h"
#include <QQuickItem>

class SpatialRelationships : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(QString pointRelationships MEMBER m_pointRelationships NOTIFY relationshipsChanged)
  Q_PROPERTY(QString polygonRelationships MEMBER m_polygonRelationships NOTIFY relationshipsChanged)
  Q_PROPERTY(QString polylineRelationships MEMBER m_polylineRelationships NOTIFY relationshipsChanged)

public:
  explicit SpatialRelationships(QQuickItem* parent = nullptr);
  ~SpatialRelationships() override = default;

  void componentComplete() override;
  static void init();

signals:
  void relationshipsChanged();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::Graphic* m_pointGraphic = nullptr;
  Esri::ArcGISRuntime::Graphic* m_polylineGraphic = nullptr;
  Esri::ArcGISRuntime::Graphic* m_polygonGraphic = nullptr;
  QString m_pointRelationships;
  QString m_polygonRelationships;
  QString m_polylineRelationships;

  void addGraphics();
  void addPointGraphic();
  void addPolygonGraphic();
  void addPolylineGraphic();
  void connectSignals();
  QStringList getSpatialRelationships(const Esri::ArcGISRuntime::Geometry& geom1, const Esri::ArcGISRuntime::Geometry& geom2);
};

#endif // SPATIALRELATIONSHIPS_H
