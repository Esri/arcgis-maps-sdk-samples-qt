// [WriteFile Name=SetMapSpatialReference, Category=Maps]
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

#include "SetMapSpatialReference.h"
#include "Map.h"
#include "MapGraphicsView.h"
#include "Basemap.h"
#include "SpatialReference.h"
#include "ArcGISMapImageLayer.h"
#include <QUrl>
#include <QVBoxLayout>

using namespace Esri::ArcGISRuntime;

SetMapSpatialReference::SetMapSpatialReference(QWidget* parent) :
    QWidget(parent)
{
    // Create a new map with the spatial reference
    m_map = new Map(SpatialReference(54024), this);

    // create the URL pointing to the map image layer
    QUrl imageLayerUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/SampleWorldCities/MapServer");

    // construct the ArcGISMapImageLayer using the URL
    m_imageLayer = new ArcGISMapImageLayer(imageLayerUrl, this);

    // create a Basemap and pass in the ArcGISMapImageLayer
    m_basemap = new Basemap(m_imageLayer, this);

    //Set the ArcGISMapImageLayer as basemap
    m_map->setBasemap(m_basemap);

    // Create a map view, and pass in the map
    m_mapView = new MapGraphicsView(m_map, this);

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}

SetMapSpatialReference::~SetMapSpatialReference() = default;
