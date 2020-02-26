// [WriteFile Name=ConvexHull, Category=Geometry]
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

#ifndef CONVEXHULL_H
#define CONVEXHULL_H

namespace Esri
{
namespace ArcGISRuntime
{
class Graphic;
class GraphicsOverlay;
class Map;
class MapQuickView;
class MultipointBuilder;
class SimpleFillSymbol;
class SimpleLineSymbol;
class SimpleMarkerSymbol;
}
}

#include <QObject>
#include <QList>

class ConvexHull : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  explicit ConvexHull(QObject* parent = nullptr);
  ~ConvexHull();

  static void init();

  Q_INVOKABLE void displayConvexHull();
  Q_INVOKABLE void clearGraphics();

signals:
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  void setupGraphics();
  void getInputs();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

  Esri::ArcGISRuntime::Graphic* m_inputsGraphic = nullptr;
  Esri::ArcGISRuntime::Graphic* m_convexHullGraphic = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::SimpleFillSymbol* m_fillSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleLineSymbol* m_lineSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleMarkerSymbol* m_markerSymbol = nullptr;
  Esri::ArcGISRuntime::MultipointBuilder* m_multipointBuilder = nullptr;
};

#endif // CONVEXHULL_H
