// [WriteFile Name=ConfigureBasemapStyleLanguage, Category=Maps]
// [Legal]
// Copyright 2024 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "ConfigureBasemapStyleLanguage.h"

// ArcGIS Maps SDK headers
#include "Basemap.h"
#include "BasemapStyleParameters.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "SpatialReference.h"
#include "Viewpoint.h"

// Qt headers
#include <QFuture>

using namespace Esri::ArcGISRuntime;

ConfigureBasemapStyleLanguage::ConfigureBasemapStyleLanguage(QObject* parent /* = nullptr */) :
  QObject(parent),
  m_map(new Map(SpatialReference::webMercator(), this)),
  m_basemapStyleParameters(new BasemapStyleParameters(this))
{
}

ConfigureBasemapStyleLanguage::~ConfigureBasemapStyleLanguage() = default;

void ConfigureBasemapStyleLanguage::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ConfigureBasemapStyleLanguage>("Esri.Samples", 1, 0, "ConfigureBasemapStyleLanguageSample");
}

MapQuickView* ConfigureBasemapStyleLanguage::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ConfigureBasemapStyleLanguage::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  const Point point(3144804, 4904598);

  // Create a Viewpoint object with the created Point and a scale of 10000000
  const Viewpoint viewpoint(point, 10000000);

  // Set the viewpoint for the mapView
  mapView->setViewpointAsync(viewpoint);

  emit mapViewChanged();
}

// Set new basemap language based on the parameters selected
void ConfigureBasemapStyleLanguage::setNewBasemapLanguage(bool global, const QString& language)
{
  m_basemapStyleParameters->setLanguageStrategy(global ? BasemapStyleLanguageStrategy::Global : BasemapStyleLanguageStrategy::Local);

  // A SpecificLanguage setting overrides the LanguageStrategy settings
  if (language == "none")
  {
    m_basemapStyleParameters->setSpecificLanguage("");
  } 
  else if (language == "Bulgarian")
  {
    m_basemapStyleParameters->setSpecificLanguage("bg");
  } 
  else if (language == "Greek")
  {
    m_basemapStyleParameters->setSpecificLanguage("el");
  } 
  else if (language == "Turkish")
  {
    m_basemapStyleParameters->setSpecificLanguage("tr");
  }

  if (m_basemap)
  {
    m_basemap->deleteLater();
  }

  m_basemap = new Basemap(BasemapStyle::ArcGISLightGray, m_basemapStyleParameters, this);
  m_map->setBasemap(m_basemap);
}
