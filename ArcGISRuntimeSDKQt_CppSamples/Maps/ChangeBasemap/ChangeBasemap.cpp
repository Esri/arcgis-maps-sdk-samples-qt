// [WriteFile Name=ChangeBasemap, Category=Maps]
// [Legal]
// Copyright 2016 Esri.

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

#include "ChangeBasemap.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"

using namespace Esri::ArcGISRuntime;

ChangeBasemap::ChangeBasemap(QQuickItem* parent) :
  QQuickItem(parent)
{
}

ChangeBasemap::~ChangeBasemap() = default;

void ChangeBasemap::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ChangeBasemap>("Esri.Samples", 1, 0, "ChangeBasemapSample");
}

void ChangeBasemap::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // create a new basemap instance
  Basemap* basemap = Basemap::topographic(this);
  // create a new map instance
  m_map = new Map(basemap, this);
  // set map on the map view
  m_mapView->setMap(m_map);
}

void ChangeBasemap::changeBasemap(const QString& basemap)
{
  if (!m_map)
  {
    return;
  }

  if (m_map->loadStatus() == LoadStatus::Loaded)
  {
    if (basemap == "Topographic")
      m_map->setBasemap(Basemap::topographic(this));
    else if (basemap == "Streets")
      m_map->setBasemap(Basemap::streets(this));
    else if (basemap == "Streets (Vector)")
      m_map->setBasemap(Basemap::streetsVector(this));
    else if (basemap == "Streets - Night (Vector)")
      m_map->setBasemap(Basemap::streetsNightVector(this));
    else if (basemap == "Imagery (Raster)")
      m_map->setBasemap(Basemap::imagery(this));
    else if (basemap == "Imagery with Labels (Raster)")
      m_map->setBasemap(Basemap::imageryWithLabels(this));
    else if (basemap == "Imagery with Labels (Vector)")
      m_map->setBasemap(Basemap::imageryWithLabelsVector(this));
    else if (basemap == "Dark Gray Canvas (Vector)")
      m_map->setBasemap(Basemap::darkGrayCanvasVector(this));
    else if (basemap == "Light Gray Canvas (Raster)")
      m_map->setBasemap(Basemap::lightGrayCanvas(this));
    else if (basemap == "Light Gray Canvas (Vector)")
      m_map->setBasemap(Basemap::lightGrayCanvasVector(this));
    else if (basemap == "Navigation (Vector)")
      m_map->setBasemap(Basemap::navigationVector(this));
    else if (basemap == "OpenStreetMap (Raster)")
      m_map->setBasemap(Basemap::openStreetMap(this));
    else if (basemap == "Oceans")
      m_map->setBasemap(Basemap::oceans(this));
  }
}

