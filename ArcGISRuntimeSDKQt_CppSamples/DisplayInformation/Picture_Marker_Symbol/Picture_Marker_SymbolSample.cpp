// [WriteFile Name=Picture_Marker_SymbolSample, Category=DisplayInformation]
// [Legal]
// Copyright 2016 Esri.

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

#include "Picture_Marker_SymbolSample.h"

#include "GraphicsOverlay.h"
#include "Map.h"
#include "MapQuickView.h"

#include <QUrl>

using namespace Esri::ArcGISRuntime;

Picture_Marker_SymbolSample::Picture_Marker_SymbolSample(QQuickItem* parent /* = nullptr */)
    : QQuickItem(parent)
    , m_map(nullptr)
    , m_mapView(nullptr)
{
}

Picture_Marker_SymbolSample::~Picture_Marker_SymbolSample()
{
}

void Picture_Marker_SymbolSample::componentComplete()
{
    QQuickItem::componentComplete();

    // find QML MapView component
    m_mapView = findChild<MapQuickView*>("mapView");
    m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

    // Create a map using the topographic basemap
    m_map = new Map(Basemap::topographic(this), this);
    m_map->setInitialViewpoint(Viewpoint(Envelope(-13075816.4047166, 4014771.46954516, -13073005.6797177, 4016869.78617381, SpatialReference(102100))));

    GraphicsOverlay* graphicsOverlay = new GraphicsOverlay(this);
    m_mapView->graphicsOverlays()->append(graphicsOverlay);

    // Set map to map view
    m_mapView->setMap(m_map);
}
