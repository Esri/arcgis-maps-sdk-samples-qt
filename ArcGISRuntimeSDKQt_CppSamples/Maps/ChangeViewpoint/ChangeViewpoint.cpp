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
    QQuickItem(parent),
    m_map(nullptr),
    m_mapView(nullptr),
    m_rotationValue(0)
{
}

ChangeViewpoint::~ChangeViewpoint()
{
}

void ChangeViewpoint::componentComplete()
{
    QQuickItem::componentComplete();

    // find QML MapView component
    m_mapView = findChild<MapQuickView*>("mapView");

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
        m_mapView->setViewpointGeometry(envBeijing, 200);
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
        Viewpoint vpSpring(Envelope(-12338668.348591767, 5546908.424239618, -12338247.594362013, 5547223.989911933, SpatialReference(102100)));
        m_mapView->setViewpointAnimated(vpSpring, 4.0, AnimationCurve::EaseInOutCubic);
    }
}
