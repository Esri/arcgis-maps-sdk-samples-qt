// [WriteFile Name=MapRotation, Category=Maps]
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

#include "MapRotation.h"
#include "Map.h"
#include "MapGraphicsView.h"
#include "MapTypes.h"

#include <QFuture>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

using namespace Esri::ArcGISRuntime;

MapRotation::MapRotation(QWidget* parent) :
  QWidget(parent)
{
    // Create a map using the streets basemap
    m_map = new Map(BasemapStyle::ArcGISStreets, this);

    // Create a map view, and pass in the map
    m_mapView = new MapGraphicsView(m_map, this);

    // Users should use slider to rotate map view
    m_mapView->setRotationByPinchingEnabled(false);

    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setMinimum(0);
    m_slider->setMaximum(360);
    m_slider->setValue(0);

    m_degrees = new QLabel("  0", this);
    connect(m_slider, &QSlider::valueChanged, [this](int value){
              m_degrees->setText(QString::number(value));
              m_mapView->setViewpointRotationAsync(value);
            });

    // Set up the UI
    QHBoxLayout *hBoxLayout = new QHBoxLayout();
    hBoxLayout->addWidget(m_slider);
    hBoxLayout->addWidget(m_degrees);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(m_mapView);
    vBoxLayout->addLayout(hBoxLayout);

    setLayout(vBoxLayout);
}

MapRotation::~MapRotation() = default;
