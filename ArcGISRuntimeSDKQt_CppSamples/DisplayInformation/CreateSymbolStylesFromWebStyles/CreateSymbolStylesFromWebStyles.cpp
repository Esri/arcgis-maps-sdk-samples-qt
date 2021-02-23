
// Copyright 2019 ESRI
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

#include "CreateSymbolStylesFromWebStyles.h"

#include "Basemap.h"
#include "Map.h"
#include "MapQuickView.h"

#include "FeatureLayer.h"
#include "ServiceFeatureTable.h"

#include <QUrl>

using namespace Esri::ArcGISRuntime;

CreateSymbolStylesFromWebStyles::CreateSymbolStylesFromWebStyles(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISNavigation, this))
{
  m_map->setReferenceScale(100'000);

  QUrl webStyleLayerUrl = QUrl("http://services.arcgis.com/V6ZHFr6zdgNZuVG0/arcgis/rest/services/LA_County_Points_of_Interest/FeatureServer/0");
  FeatureLayer* webStyleLayer = new FeatureLayer(new ServiceFeatureTable(webStyleLayerUrl, this), this);
  m_map->operationalLayers()->append(webStyleLayer);
}

CreateSymbolStylesFromWebStyles::~CreateSymbolStylesFromWebStyles()
{
}

MapQuickView* CreateSymbolStylesFromWebStyles::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void CreateSymbolStylesFromWebStyles::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  const double x_longitude = -118.44186;
  const double y_latitude = 34.38301;
  const Point centerPt(x_longitude, y_latitude, SpatialReference::wgs84());
  const double scale = 7000;

  m_mapView->setViewpointCenter(centerPt, scale);

  emit mapViewChanged();
}
