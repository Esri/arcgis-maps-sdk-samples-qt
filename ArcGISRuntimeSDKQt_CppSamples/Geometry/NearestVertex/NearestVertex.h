// [WriteFile Name=NearestVertex, Category=Geometry]
// [Legal]
// Copyright 2020 Esri.

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

#ifndef NEARESTVERTEX_H
#define NEARESTVERTEX_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
}
}

#include <QObject>

class NearestVertex : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(int vertexDistance READ vertexDistance NOTIFY vertexDistanceCalculated)
  Q_PROPERTY(int coordinateDistance READ coordinateDistance NOTIFY coordinateDistanceCalculated)

public:
  explicit NearestVertex(QObject* parent = nullptr);
  ~NearestVertex();

  static void init();

signals:
  void mapViewChanged();
  void vertexDistanceCalculated();
  void coordinateDistanceCalculated();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  int vertexDistance() const;
  int coordinateDistance() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void setupGraphics();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  int m_vertexDistance = 0;
  int m_coordinateDistance = 0;
};

#endif // NEARESTVERTEX_H
