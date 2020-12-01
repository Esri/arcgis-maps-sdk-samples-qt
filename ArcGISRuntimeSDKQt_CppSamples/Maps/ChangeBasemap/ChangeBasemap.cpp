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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

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
  Basemap* basemap = new Basemap(BasemapStyle::ArcGISTopographic, this);
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
      m_map->setBasemap(new Basemap(BasemapStyle::ArcGISTopographic, this));
    else if (basemap == "Streets")
      m_map->setBasemap(new Basemap(BasemapStyle::ArcGISStreets, this));
    else if (basemap == "Streets - Relief")
      m_map->setBasemap(new Basemap(BasemapStyle::ArcGISStreetsRelief, this));
    else if (basemap == "Streets - Night")
      m_map->setBasemap(new Basemap(BasemapStyle::ArcGISStreetsNight, this));
    else if (basemap == "Imagery")
      m_map->setBasemap(new Basemap(BasemapStyle::ArcGISImageryStandard, this));
    else if (basemap == "Imagery with Labels")
      m_map->setBasemap(new Basemap(BasemapStyle::ArcGISImagery, this));
    else if (basemap == "Labels without Imagery")
      m_map->setBasemap(new Basemap(BasemapStyle::ArcGISImageryLabels, this));
    else if (basemap == "Dark Gray Canvas")
      m_map->setBasemap(new Basemap(BasemapStyle::ArcGISDarkGray, this));
    else if (basemap == "Light Gray Canvas")
      m_map->setBasemap(new Basemap(BasemapStyle::ArcGISLightGray, this));
    else if (basemap == "Navigation")
      m_map->setBasemap(new Basemap(BasemapStyle::ArcGISNavigation, this));
    else if (basemap == "OpenStreetMap")
      m_map->setBasemap(new Basemap(BasemapStyle::OsmStandard, this));
    else if (basemap == "Oceans")
      m_map->setBasemap(new Basemap(BasemapStyle::ArcGISOceans, this));
  }
}

