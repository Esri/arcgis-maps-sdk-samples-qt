// [WriteFile Name=ChangeBasemap, Category=Maps]
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

#include "ChangeBasemap.h"
#include "Map.h"
#include "MapGraphicsView.h"
#include "MapTypes.h"
#include "Basemap.h"

#include <QComboBox>
#include <QVBoxLayout>
#include <QGraphicsProxyWidget>

using namespace Esri::ArcGISRuntime;

ChangeBasemap::ChangeBasemap(QWidget* parent) :
    QWidget(parent)
{
    // Create a map initially using the topographic basemap
    m_map = new Map(BasemapStyle::ArcGISTopographic, this);

    // Create a map view, and pass in the map
    m_mapView = new MapGraphicsView(m_map, this);

    // Create and populate a combo box with several basemap styles
    m_basemapCombo = new QComboBox(this);
    m_basemapCombo->adjustSize();
    m_basemapCombo->setStyleSheet("QComboBox#combo {color: black; background-color:#000000;}");
    m_basemapCombo->addItems(QStringList { "Topographic", "Streets", "Imagery", "Oceans" });

    // Connect the combo box signal to lambda for setting new basemaps
    connect(m_basemapCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int i) {
        switch (i)
        {
        // Call setBasemap and pass in the appropriate basemap
        case 0:
            m_map->setBasemap(new Basemap(BasemapStyle::ArcGISTopographic, this));
            break;
        case 1:
            m_map->setBasemap(new Basemap(BasemapStyle::ArcGISStreets, this));
            break;
        case 2:
            m_map->setBasemap(new Basemap(BasemapStyle::ArcGISImageryStandard, this));
            break;
        case 3:
            m_map->setBasemap(new Basemap(BasemapStyle::ArcGISOceans, this));
            break;
        }
    });

    // Set up the UI
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_basemapCombo);
    widget->setLayout(layout);

    QGraphicsProxyWidget *proxy = m_mapView->scene()->addWidget(widget);
    proxy->setPos(10, 10);
    proxy->setOpacity(0.95);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}

ChangeBasemap::~ChangeBasemap() = default;
