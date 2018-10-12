// [WriteFile Name=ChangeViewpoint, Category=Maps]
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

#include "ChangeViewpoint.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "Point.h"
#include "SpatialReference.h"
#include "Envelope.h"
#include "Viewpoint.h"

using namespace Esri::ArcGISRuntime;

ChangeViewpoint::ChangeViewpoint(QQuickItem* parent) :
  QQuickItem(parent)
{
}

ChangeViewpoint::~ChangeViewpoint() = default;

void ChangeViewpoint::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ChangeViewpoint>("Esri.Samples", 1, 0, "ChangeViewpointSample");
}

void ChangeViewpoint::componentComplete()
{
  QQuickItem::componentComplete();

  // create a new basemap instance
  Basemap* basemap = Basemap::imageryWithLabels(this);
  // create a new map instance
  m_map = new Map(basemap, this);
  // set map on the map view
  m_mapView->setMap(m_map);
}

void ChangeViewpoint::changeViewpoint(QString viewpoint)
{
  if (viewpoint == "Center")
  {
    Point ptEsriHeadquarters(-117.195681,34.056218, SpatialReference(4326));
    m_mapView->setViewpointCenter(ptEsriHeadquarters);
  }
  else if (viewpoint == "Center and scale")
  {
    Point ptHawaii(-157.564, 20.677, SpatialReference(4236));
    m_mapView->setViewpointCenter(ptHawaii, 4000000.0);
  }
  else if (viewpoint == "Geometry")
  {
    Envelope envBeijing(116.380, 39.920, 116.400, 39.940, SpatialReference(4236));
    m_mapView->setViewpointGeometry(envBeijing);
  }
  else if (viewpoint == "Geometry and padding")
  {
    Envelope envBeijing(116.380, 39.920, 116.400, 39.940, SpatialReference(4236));
    m_mapView->setViewpointGeometry(envBeijing, 200 * screenRatio());
  }
  else if (viewpoint == "Rotation")
  {
    m_rotationValue = (m_rotationValue + 45) % 360;
    m_mapView->setViewpointRotation(m_rotationValue);
  }
  else if (viewpoint == "Scale 1:5,000,000")
  {
    m_mapView->setViewpointScale(5000000.0);
  }
  else if (viewpoint == "Scale 1:10,000,000")
  {
    m_mapView->setViewpointScale(10000000.0);
  }
  else if (viewpoint == "Animation")
  {
    //! [set viewpoint api snippet]
    constexpr double xMin = -12338668.348591767;
    constexpr double yMin = 5546908.424239618;
    constexpr double xMax = -12338247.594362013;
    constexpr double yMax = 5547223.989911933;
    constexpr int wkid = 102100;
    Viewpoint vpSpring(Envelope(xMin, yMin, xMax, yMax, SpatialReference(wkid)));
    constexpr float duration = 4.0f;
    m_mapView->setViewpointAnimated(vpSpring, duration, AnimationCurve::EaseInOutCubic);
    //! [set viewpoint api snippet]
  }
}

MapQuickView* ChangeViewpoint::mapQuickView() const
{
  return m_mapView;
}

void ChangeViewpoint::setMapQuickView(MapQuickView* mapView)
{
  m_mapView = mapView;
  emit mapQuickViewChanged();
}

double ChangeViewpoint::screenRatio() const
{
  const double width = static_cast<double>(m_mapView->mapWidth());
  const double height = static_cast<double>(m_mapView->mapHeight());
  return height > width ? width / height : height / width;
}
