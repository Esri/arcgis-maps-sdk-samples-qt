// [WriteFile Name=MapLoaded, Category=Maps]
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

#include "MapLoaded.h"
#include "Map.h"
#include "MapGraphicsView.h"
#include <QLabel>
#include <QVBoxLayout>

using namespace Esri::ArcGISRuntime;

MapLoaded::MapLoaded(QWidget* parent) :
    QWidget(parent)
{
    // Create a map using the streets basemap
    m_map = new Map(BasemapStyle::ArcGISStreets, this);

    // Create a map view, and pass in the map
    m_mapView = new MapGraphicsView(m_map, this);

    // Create the load status label
    m_loadStatus= new QLabel("", this);

    // Map Load Status changed lambda
    connect(m_map, &Esri::ArcGISRuntime::Map::loadStatusChanged, this, [this](Esri::ArcGISRuntime::LoadStatus loadStatus)
    {
        switch (loadStatus)
        {
          case LoadStatus::Loaded:
            m_loadStatus->setText(QString("Map Load Status: Loaded"));
            break;
          case LoadStatus::Loading:
            m_loadStatus->setText(QString("Map Load Status: Loading..."));
            break;
          case LoadStatus::FailedToLoad:
            m_loadStatus->setText(QString("Map Load Status: Failed to Load"));
            break;
          case LoadStatus::NotLoaded:
            m_loadStatus->setText(QString("Map Load Status: Not Loaded"));
            break;
          case LoadStatus::Unknown:
            m_loadStatus->setText(QString("Map Load Status: Unknown"));
            break;
        }
    });

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(m_mapView);
    vBoxLayout->addWidget(m_loadStatus);
    setLayout(vBoxLayout);
}

MapLoaded::~MapLoaded() = default;
