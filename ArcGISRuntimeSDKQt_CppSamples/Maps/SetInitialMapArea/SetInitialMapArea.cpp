// Copyright 2015 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "SetInitialMapArea.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "Viewpoint.h"
#include "Envelope.h"

using namespace Esri::ArcGISRuntime;

SetInitialMapArea::SetInitialMapArea(QQuickItem* parent) :
    QQuickItem(parent)
{
}

SetInitialMapArea::~SetInitialMapArea()
{
}

void SetInitialMapArea::componentComplete()
{
    QQuickItem::componentComplete();

    // find QML MapView component
    m_mapView = findChild<MapQuickView*>("mapView");

    // create a new basemap instance
    Basemap* basemap = Basemap::imageryWithLabels(this);
    // create a new map instance
    m_map = new Map(basemap, this);
    // set the initial viewpoint
    Envelope envelope(-12211308.778729, 4645116.003309, -12208257.879667, 4650542.535773, SpatialReference(102100));
    m_map->setInitialViewpoint(Viewpoint(envelope));
    // set map on the map view
    m_mapView->setMap(m_map);
}

