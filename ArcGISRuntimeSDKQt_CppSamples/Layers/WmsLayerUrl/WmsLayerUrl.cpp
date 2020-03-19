// [WriteFile Name=WmsLayerUrl, Category=Layers]
// [Legal]
// Copyright 2017 Esri.

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

#include "WmsLayerUrl.h"

#include "Envelope.h"
#include "Map.h"
#include "MapQuickView.h"
#include "WmsLayer.h"

#include <QUrl>
#include <QStringList>

using namespace Esri::ArcGISRuntime;

WmsLayerUrl::WmsLayerUrl(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void WmsLayerUrl::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<WmsLayerUrl>("Esri.Samples", 1, 0, "WmsLayerUrlSample");
}

void WmsLayerUrl::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the light gray canvas basemap
  m_map = new Map(Basemap::lightGrayCanvas(this), this);

  // Add a WMS Layer by specifying the URL and layer name
  const QUrl wmsServiceUrl("https://nowcoast.noaa.gov/arcgis/services/nowcoast/radar_meteo_imagery_nexrad_time/MapServer/WMSServer?request=GetCapabilities&service=WMS");
  const QStringList layerNames{"1"};
  WmsLayer* wmsLayer = new WmsLayer(wmsServiceUrl, layerNames, this);
  m_map->operationalLayers()->append(wmsLayer);

  // Set map to map view
  m_mapView->setMap(m_map);

  // start zoomed in over the US
  m_mapView->setViewpointGeometry(Envelope(-19195297.778679, 512343.939994, -3620418.579987, 8658913.035426, SpatialReference::webMercator()));
}
