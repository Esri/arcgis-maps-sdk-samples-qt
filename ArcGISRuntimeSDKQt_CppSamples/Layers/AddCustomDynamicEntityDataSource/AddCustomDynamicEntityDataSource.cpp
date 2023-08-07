// [WriteFile Name=AddCustomDynamicEntityDataSource, Category=Layers]
// [Legal]
// Copyright 2023 Esri.

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

#include "AddCustomDynamicEntityDataSource.h"

#include "ArcGISStreamService.h"
#include "DynamicEntityDataSource.h"
#include "DynamicEntityLayer.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "SimulatedDataSource.h"
#include "TaskWatcher.h"
#include "Viewpoint.h"

using namespace Esri::ArcGISRuntime;

AddCustomDynamicEntityDataSource::AddCustomDynamicEntityDataSource(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISOceans, this))
{

}

AddCustomDynamicEntityDataSource::~AddCustomDynamicEntityDataSource() = default;

void AddCustomDynamicEntityDataSource::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<AddCustomDynamicEntityDataSource>("Esri.Samples", 1, 0, "AddCustomDynamicEntityDataSourceSample");
}

MapQuickView* AddCustomDynamicEntityDataSource::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void AddCustomDynamicEntityDataSource::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  m_mapView->setViewpoint(Viewpoint(47.984, -123.657, 3e6));

  auto dataSource = new SimulatedDataSource("AddCustomDynamicEntityDataSource.qrc/AIS_MarineCadastre_SelectedVessels_CustomDataSource.json", "MMSI", 100, this);
  auto dynamicEntityLayer = new DynamicEntityLayer(dataSource, this);
  dynamicEntityLayer->dataSource()->load();
  QFuture q = dynamicEntityLayer->dataSource()->connectDataSourceAsync();

  emit mapViewChanged();
}
