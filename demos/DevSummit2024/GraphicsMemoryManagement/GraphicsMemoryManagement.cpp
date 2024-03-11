// Copyright 2024 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#include "GraphicsMemoryManagement.h"

#include "Map.h"
#include "MapTypes.h"
#include "MapQuickView.h"

#include "GraphicsOverlay.h"
#include "SimpleRenderer.h"
#include "SimpleMarkerSymbol.h"
#include "SymbolTypes.h"
#include "GraphicsOverlayListModel.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "Point.h"
#include "SpatialReference.h"

// display current RAM
#include <random>

namespace
{
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> longitudeDist(-180.0, 180.0);
std::uniform_real_distribution<float> latitudeDist(-90.0, 90.0);
}

using namespace Esri::ArcGISRuntime;

GraphicsMemoryManagement::GraphicsMemoryManagement(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISTerrain, this))
{
  m_graphicsOverlay = new GraphicsOverlay(this);

  SimpleRenderer* pointRenderer = new SimpleRenderer(new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, Qt::blue, 5, this), this);
  m_graphicsOverlay->setRenderer(pointRenderer);

  m_graphicsParent.reset(new QObject);
}

GraphicsMemoryManagement::~GraphicsMemoryManagement()
{
}

MapQuickView* GraphicsMemoryManagement::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void GraphicsMemoryManagement::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  emit mapViewChanged();
}

int GraphicsMemoryManagement::graphicsOverlayCount() const
{
  return m_graphicsOverlay->graphics()->size();
}

Point createRandomWgs84Point()
{
  float longitude = longitudeDist(gen);
  float latitude = latitudeDist(gen);
  return Point(longitude, latitude, SpatialReference::wgs84());
}

void GraphicsMemoryManagement::badMemoryManagement()
{
  for (int i = 0; i < 10'000; ++i)
  {
    m_graphicsOverlay->graphics()->append(new Graphic(createRandomWgs84Point(), this));
  }
  emit appUpdated();
}

void GraphicsMemoryManagement::properMemoryManagement()
{
  for (int i = 0; i < 10'000; ++i)
  {
    m_graphicsOverlay->graphics()->append(new Graphic(createRandomWgs84Point(), m_graphicsParent.get()));
  }
  emit appUpdated();
}

void GraphicsMemoryManagement::clearGraphics()
{
  m_graphicsOverlay->graphics()->clear();
  m_graphicsParent.reset(new QObject);
  emit appUpdated();
}
