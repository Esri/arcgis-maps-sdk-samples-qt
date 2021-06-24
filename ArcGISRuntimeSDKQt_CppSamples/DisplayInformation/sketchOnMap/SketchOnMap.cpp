// [WriteFile Name=SketchOnMap, Category=DisplayInformation]
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "SketchOnMap.h"

#include "Map.h"
#include "MapQuickView.h"

#include "PolygonBuilder.h"
#include "SimpleFillSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "SketchEditor.h"

using namespace Esri::ArcGISRuntime;

SketchOnMap::SketchOnMap(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISLightGray, this))
{
  m_sketchOverlay = new GraphicsOverlay(this);
}

SketchOnMap::~SketchOnMap() = default;

void SketchOnMap::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<SketchOnMap>("Esri.Samples", 1, 0, "SketchOnMapSample");
}

MapQuickView* SketchOnMap::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void SketchOnMap::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  m_mapView->setViewpointCenter(Point(-118.819, 34.0138, SpatialReference::wgs84()), 50'000);

  m_mapView->graphicsOverlays()->append(m_sketchOverlay);

  m_sketchEditor = new SketchEditor();

  m_mapView->setSketchEditor(m_sketchEditor);

  PolygonBuilder* polygonBuilder = new PolygonBuilder(SpatialReference::wgs84());
  polygonBuilder->addPoint(-118.8190, 34.0138);
  polygonBuilder->addPoint(-118.8068, 34.0216);
  polygonBuilder->addPoint(-118.7914, 34.0164);
  polygonBuilder->addPoint(-118.7960, 34.0087);
  polygonBuilder->addPoint(-118.8086, 34.0035);

  const Polygon polygon = polygonBuilder->toPolygon();

  SimpleFillSymbol* polygonSymbol = new SimpleFillSymbol(
    SimpleFillSymbolStyle::Solid, // Fill style
    QColor(255, 131, 0), // Fill color, RGB orange
    new SimpleLineSymbol( // Outline
      SimpleLineSymbolStyle::Solid, // Outline style
      QColor(Qt::blue), // Outline color
      2.0, // Outline width (float)
      this
      ),
    this
    );

  Graphic* polygonGraphic = new Graphic(polygon, polygonSymbol, this);

  m_sketchOverlay->graphics()->append(polygonGraphic);

  m_sketchEditor->start(polygon);

  emit mapViewChanged();
}
