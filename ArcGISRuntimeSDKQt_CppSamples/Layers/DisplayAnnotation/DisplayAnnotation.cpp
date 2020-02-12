// [WriteFile Name=DisplayAnnotation, Category=Layers]
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

#include "DisplayAnnotation.h"

#include "AnnotationLayer.h"
#include "FeatureLayer.h"
#include "Map.h"
#include "MapQuickView.h"
#include "ServiceFeatureTable.h"

using namespace Esri::ArcGISRuntime;

namespace
{
const QUrl url("https://services1.arcgis.com/6677msI40mnLuuLr/arcgis/rest/services/East_Lothian_Rivers/FeatureServer/0");
const QUrl annotationUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/RiversAnnotation/FeatureServer/0");
}

DisplayAnnotation::DisplayAnnotation(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::lightGrayCanvas(this), this))
{
  const Point center(-2.725610, 55.882436, SpatialReference::wgs84());
  const Viewpoint viewpoint(center, 50000);
  m_map->setInitialViewpoint(viewpoint);

  ServiceFeatureTable* riverFeaturetable = new ServiceFeatureTable(url, this);
  FeatureLayer* riverFeatureLayer = new FeatureLayer(riverFeaturetable, this);
  m_map->operationalLayers()->append(riverFeatureLayer);

  ServiceFeatureTable* riverAnnotationtable = new ServiceFeatureTable(annotationUrl, this);
  AnnotationLayer* riverAnnotationLayer = new AnnotationLayer(riverAnnotationtable, this);
  m_map->operationalLayers()->append(riverAnnotationLayer);
}

DisplayAnnotation::~DisplayAnnotation() = default;

void DisplayAnnotation::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DisplayAnnotation>("Esri.Samples", 1, 0, "DisplayAnnotationSample");
}

MapQuickView* DisplayAnnotation::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void DisplayAnnotation::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}
