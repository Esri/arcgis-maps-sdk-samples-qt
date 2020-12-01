// [WriteFile Name=SetInitialMapArea, Category=Maps]
// [Legal]
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
// [Legal]

#include "SetInitialMapArea.h"
#include "Map.h"
#include "MapGraphicsView.h"
#include "Basemap.h"
#include "Envelope.h"
#include "SpatialReference.h"
#include <QVBoxLayout>

using namespace Esri::ArcGISRuntime;

SetInitialMapArea::SetInitialMapArea(QWidget* parent) :
    QWidget(parent)
{
    // Create envelope for the area of interest
    Envelope envelope(-12211308.778729, 4645116.003309, -12208257.879667, 4650542.535773, SpatialReference(102100));

    // Create a new map with the imagery with labels basemap
    m_map = new Map(BasemapStyle::ArcGISImageryStandard, this);

    // Set the initial viewpoint to the envelope
    m_map->setInitialViewpoint(Viewpoint(envelope));

    // Create a map view, and pass in the map
    m_mapView = new MapGraphicsView(m_map, this);

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}

SetInitialMapArea::~SetInitialMapArea() = default;
