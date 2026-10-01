// [WriteFile Name=ChangeViewpoint, Category=Maps]
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

#include "ChangeViewpoint.h"
#include "Map.h"
#include "MapWidget.h"
#include "MapTypes.h"
#include "SpatialReference.h"
#include "Envelope.h"
#include "Point.h"
#include "Viewpoint.h"
#include "MapViewTypes.h"

#include <QComboBox>
#include <QFuture>
#include <QGridLayout>
#include <QVBoxLayout>

using namespace Esri::ArcGISRuntime;

ChangeViewpoint::ChangeViewpoint(QWidget* parent) :
  QWidget(parent)
{
  // Create a map using an imagery basemap
  m_map = new Map(BasemapStyle::ArcGISImagery, this);

  // Create a map view, and pass in the map
  m_mapWidget = new MapWidget(this);
  m_mapWidget->setMap(m_map);

  // Create and populate a combo box with several viewpoint types
  m_viewpointCombo = new QComboBox(this);
  m_viewpointCombo->adjustSize();
  m_viewpointCombo->setStyleSheet("QComboBox { color: black; background-color: white;}");
  m_viewpointCombo->addItems(QStringList{"Center", "Center and scale", "Geometry", "Geometry and padding", "Rotation", "Scale: 1:5,000,000",
                                         "Scale: 1:10,000,000", "Animation"});

  // Connect the combo box signal to slot for setting new viewpoint
  connect(m_viewpointCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ChangeViewpoint::changeToNewViewpoint);

  // Set up the UI
  QWidget* widget = new QWidget(this);
  widget->setAttribute(Qt::WA_NativeWindow);
  QVBoxLayout* layout = new QVBoxLayout();
  layout->setContentsMargins(5, 5, 0, 0);
  layout->addWidget(m_viewpointCombo);
  widget->setLayout(layout);

  QGridLayout* gridLayout = new QGridLayout();
  gridLayout->addWidget(m_mapWidget, 0, 0);
  gridLayout->addWidget(widget, 0, 0, Qt::AlignTop | Qt::AlignLeft);
  setLayout(gridLayout);
  widget->raise();
}

ChangeViewpoint::~ChangeViewpoint() = default;

void ChangeViewpoint::changeToNewViewpoint(int index)
{
  // Create the objects used to change viewpoint in various cases

  Point ptEsriHeadquarters(-117.195681, 34.056218, SpatialReference(4236));
  Point ptHawaii(-157.564, 20.677, SpatialReference(4236));
  Envelope envBeijing(116.380, 39.920, 116.400, 39.940, SpatialReference(4236));
  Viewpoint vpSpring(Envelope(-12338668.348591767, 5546908.424239618, -12338247.594362013, 5547223.989911933, SpatialReference(102100)));

  switch (index)
  {
    // Call setViewpoint and pass in the appropriate viewpoint
    case 0: // "Center"
      m_mapWidget->setViewpointCenterAsync(ptEsriHeadquarters);
      break;
    case 1: // "Center and scale"
      m_mapWidget->setViewpointCenterAsync(ptHawaii, 4000000.0);
      break;
    case 2: // "Geometry"
      m_mapWidget->setViewpointGeometryAsync(envBeijing);
      break;
    case 3: // "Geometry and padding"
      m_mapWidget->setViewpointGeometryAsync(envBeijing, 200 * screenRatio());
      break;
    case 4: // "Rotation"
      m_rotationValue = (m_rotationValue + 45) % 360;
      m_mapWidget->setViewpointRotationAsync(m_rotationValue);
      break;
    case 5: // "Scale: 1:5,000,000"
      m_mapWidget->setViewpointScaleAsync(5000000.0);
      break;
    case 6: // "Scale: 1:10,000,000"
      m_mapWidget->setViewpointScaleAsync(10000000.0);
      break;
    case 7: // "Scale: 1:5,000,000"
      m_mapWidget->setViewpointAsync(vpSpring, 4.0, AnimationCurve::EaseInOutCubic);
      break;
  }
}

double ChangeViewpoint::screenRatio() const
{
  const double width = static_cast<double>(m_mapWidget->width());
  const double height = static_cast<double>(m_mapWidget->height());
  return height > width ? width / height : height / width;
}
