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

#include "SetInitialMapLocation.h"
#include "Map.h"
#include "MapView.h"
#include <QDir>
#include <QUrl>
#include <QVBoxLayout>

using namespace Esri::ArcGISRuntime;

SetInitialMapLocation::SetInitialMapLocation(QWidget* parent) :
  QWidget(parent),
  m_map(nullptr),
  m_mapView(nullptr)
{
    // Create a new map with the basemap type enum and pass in initial lat, lon, and scale
    m_map = new Map(BasemapType::ImageryWithLabels, -33.867886, -63.985, 16, this);

    // Create a map view, and pass in the map
    m_mapView = new MapView(m_map, this);

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}

SetInitialMapLocation::~SetInitialMapLocation()
{
}
