// [WriteFile Name=StyleWmsLayer, Category=Layers]
// [Legal]
// Copyright 2018 Esri.
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
#include "StyleWmsLayer.h"

// ArcGIS Maps SDK headers
#include "Envelope.h"
#include "Error.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "SpatialReference.h"
#include "WmsLayer.h"
#include "WmsLayerInfo.h"
#include "WmsSublayer.h"

// Qt headers
#include <QFuture>

using namespace Esri::ArcGISRuntime;

StyleWmsLayer::StyleWmsLayer(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void StyleWmsLayer::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<StyleWmsLayer>("Esri.Samples", 1, 0, "StyleWmsLayerSample");
}

void StyleWmsLayer::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map with spatial reference appropriate for the WMS service (North American Datum 83)
  m_map = new Map(SpatialReference(26915), this);
  m_map->setMinScale(7'000'000);

  // Add a WMS Layer
  WmsLayer* wmsLayer = new WmsLayer(QUrl("https://imageserver.gisdata.mn.gov/cgi-bin/mncomp?SERVICE=WMS&VERSION=1.3.0&REQUEST=GetCapabilities"), QStringList{"mncomp"}, this);
  m_map->operationalLayers()->append(wmsLayer);

  // connect to the doneLoading signal of the WMS Layer
  connect(wmsLayer, &WmsLayer::doneLoading, this, [this, wmsLayer](const Error& e)
  {
    if (!e.isEmpty())
      return;

    // zoom to the layer
    m_mapView->setViewpointGeometryAsync(wmsLayer->fullExtent());

    // get the subLayer
    if (!wmsLayer->sublayers().empty())
      m_wmsSublayer = wmsLayer->sublayers().at(0);
  });

  // Set map to map view
  m_mapView->setMap(m_map);
}

void StyleWmsLayer::setCurrentStyle(int index)
{
  if (!m_wmsSublayer)
    return;

  // get the styles
  const QStringList styles = m_wmsSublayer->sublayerInfo().styles();
  if (styles.length() < index + 1)
    return;

  // get the style
  const QString style = styles.at(index);

  // set the style on the WMS Sublayer
  m_wmsSublayer->setCurrentStyle(style);
}
