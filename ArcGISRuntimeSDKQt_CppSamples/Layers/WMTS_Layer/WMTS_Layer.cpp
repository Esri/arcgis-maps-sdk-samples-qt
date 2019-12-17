// [WriteFile Name=WMTS_Layer, Category=Layers]
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

#include "WMTS_Layer.h"

#include "Map.h"
#include "MapQuickView.h"
#include "WmtsLayer.h"
#include "WmtsService.h"
#include "WmtsServiceInfo.h"
#include "WmtsLayerInfo.h"
#include "Basemap.h"

using namespace Esri::ArcGISRuntime;

WMTS_Layer::WMTS_Layer(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void WMTS_Layer::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<WMTS_Layer>("Esri.Samples", 1, 0, "WMTS_LayerSample");
}

void WMTS_Layer::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // create the WMTS layer
  createWmtsLayer();
}

void WMTS_Layer::createWmtsLayer()
{
  // create the service
  m_service = new WmtsService(m_wmtsServiceUrl, this);

  // connect to the doneLoading signal of the service
  connect(m_service, &WmtsService::doneLoading, this, [this](Error loadError)
  {
    if (!loadError.isEmpty())
    {
      qDebug() << "Error:" << loadError.message();
      return;
    }

    // get the layer info list
    WmtsServiceInfo serviceInfo = m_service->serviceInfo();
    QList<WmtsLayerInfo> layerInfos = serviceInfo.layerInfos();
    // get the first layer ID from the list
    QString layerId = layerInfos.at(0).wmtsLayerId();
    // create the WMTS layer
    WmtsLayer* wmtsLayer = new WmtsLayer(m_wmtsServiceUrl, layerId, this);
    // create a basemap from the layer
    Basemap* basemap = new Basemap(wmtsLayer, this);
    // create a map
    Map* map = new Map(basemap, this);
    // set the map on the mapview
    m_mapView->setMap(map);
  });

  // load the service
  m_service->load();
}
