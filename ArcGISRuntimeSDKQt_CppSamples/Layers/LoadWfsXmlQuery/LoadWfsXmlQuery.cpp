// [WriteFile Name=LoadWfsXmlQuery, Category=Layers]
// [Legal]
// Copyright 2019 Esri.

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

#include "LoadWfsXmlQuery.h"

#include "Map.h"
#include "MapQuickView.h"
#include "FeatureLayer.h"
#include "WfsFeatureTable.h"

using namespace Esri::ArcGISRuntime;

namespace
{
  const QString xmlQueryString = R"(
    <wfs:GetFeature service="WFS" version="2.0.0"
      xmlns:Seattle_Downtown_Features="https://dservices2.arcgis.com/ZQgQTuoyBrtmoGdP/arcgis/services/Seattle_Downtown_Features/WFSServer"
      xmlns:wfs="http://www.opengis.net/wfs/2.0"
      xmlns:fes="http://www.opengis.net/fes/2.0"
      xmlns:gml="http://www.opengis.net/gml/3.2">
      <wfs:Query typeNames="Seattle_Downtown_Features:Trees">
        <fes:Filter>
        <fes:PropertyIsLike wildCard="*" escapeChar="\">
        <fes:ValueReference>Trees:SCIENTIFIC</fes:ValueReference>
        <fes:Literal>Tilia *</fes:Literal>
        </fes:PropertyIsLike>
        </fes:Filter>
      </wfs:Query>
    </wfs:GetFeature>)";
}

LoadWfsXmlQuery::LoadWfsXmlQuery(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::navigationVector(this), this))
{
  // create WFS Feature Table
  const QUrl featureTableUrl("https://dservices2.arcgis.com/ZQgQTuoyBrtmoGdP/arcgis/services/Seattle_Downtown_Features/WFSServer?service=wfs&request=getcapabilities");
  WfsFeatureTable* wfsFeatureTable = new WfsFeatureTable(featureTableUrl,
                                                         "Seattle_Downtown_Features:Trees",
                                                         this);
  wfsFeatureTable->setFeatureRequestMode(FeatureRequestMode::ManualCache);

  // connect to feature table doneLoading signal
  connect(wfsFeatureTable, &WfsFeatureTable::doneLoading, this, [wfsFeatureTable, this](Error e)
  {
    if (!e.isEmpty())
      return;

    // populate the table with XML Query
    wfsFeatureTable->populateFromService(xmlQueryString, true);

    // set the viewpoint to the center of the extent
    m_mapView->setViewpointGeometry(wfsFeatureTable->extent(), 50);
  });

  // create Feature Layer
  FeatureLayer* featureLayer = new FeatureLayer(wfsFeatureTable, this);

  // add layer to the map
  m_map->operationalLayers()->append(featureLayer);
}

LoadWfsXmlQuery::~LoadWfsXmlQuery() = default;

void LoadWfsXmlQuery::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<LoadWfsXmlQuery>("Esri.Samples", 1, 0, "LoadWfsXmlQuerySample");
}

MapQuickView* LoadWfsXmlQuery::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void LoadWfsXmlQuery::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}
