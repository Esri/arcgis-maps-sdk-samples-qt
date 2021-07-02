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
  m_map(new Map(BasemapStyle::ArcGISImagery, this))
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

  emit mapViewChanged();
}

void SketchOnMap::setSketchCreationMode(SampleSketchMode sketchCreationMode)
{
  if (m_map->loadStatus() != LoadStatus::Loaded)
    return;

  switch(sketchCreationMode)
  {
    case SampleSketchMode::PointSketchMode:
      m_sketchEditor->start(SketchCreationMode::Point);
      break;

    case SampleSketchMode::MultipointSketchMode:
      m_sketchEditor->start(SketchCreationMode::Multipoint);
      break;

    case SampleSketchMode::PolylineSketchMode:
      m_sketchEditor->start(SketchCreationMode::Polyline);
      break;

    case SampleSketchMode::PolygonSketchMode:
      m_sketchEditor->start(SketchCreationMode::Polygon);
      break;

    default:
      break;

  }
}

void SketchOnMap::stopSketching(bool saveGeometry)
{
  if (!saveGeometry)
  {
    m_sketchEditor->stop();
    return;
  }

  Geometry sketchGeometry = m_sketchEditor->geometry();
  Symbol* geometrySymbol = nullptr;

  if (sketchGeometry.geometryType() == GeometryType::Point || sketchGeometry.geometryType() == GeometryType::Multipoint)
  {
    SimpleMarkerSymbol* pointSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor(255, 131, 0), 10, this);
    pointSymbol->setOutline(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(Qt::blue), 2.0, this));
    geometrySymbol = pointSymbol;
  }
  else if (sketchGeometry.geometryType() == GeometryType::Polyline)
  {
    geometrySymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(255, 131, 0), 2, this);
  }
  else if (sketchGeometry.geometryType() == GeometryType::Polygon)
  {
    geometrySymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor(255, 131, 0),
                                          new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(Qt::blue), 2.0, this), this);
  }

  Graphic* sketchGraphic = new Graphic(sketchGeometry, geometrySymbol, this);

  m_sketchOverlay->graphics()->append(sketchGraphic);

  m_sketchEditor->stop();
}

void SketchOnMap::clearGraphics()
{
  m_sketchOverlay->graphics()->clear();
}

void SketchOnMap::undo()
{
  m_sketchEditor->undo();
}

void SketchOnMap::redo()
{
  m_sketchEditor->redo();
}
