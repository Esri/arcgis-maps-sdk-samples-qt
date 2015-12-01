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

#include "DisplayDeviceLocation.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include <QGeoPositionInfoSource>

using namespace Esri::ArcGISRuntime;

DisplayDeviceLocation::DisplayDeviceLocation(QQuickItem* parent) :
    QQuickItem(parent),
    m_map(nullptr),
    m_mapView(nullptr),
    m_compassMode("Compass"),
    m_navigationMode("Navigation"),
    m_recenterMode("Re-Center"),
    m_onMode("On"),
    m_stopMode("Stop"),
    m_closeMode("Close")
{
    emit compassModeChanged();
    emit navigationModeChanged();
    emit recenterModeChanged();
    emit onModeChanged();
    emit stopModeChanged();
    emit closeModeChanged();
}

DisplayDeviceLocation::~DisplayDeviceLocation()
{
}

void DisplayDeviceLocation::componentComplete()
{
    QQuickItem::componentComplete();

    // find QML MapView component
    m_mapView = findChild<MapQuickView*>("mapView");

    // create a new basemap instance
    Basemap* basemap = Basemap::imagery(this);

    // create a new map instance
    m_map = new Map(basemap, this);

    // set map on the map view
    m_mapView->setMap(m_map);

    // turn on the location display
    m_mapView->locationDisplay()->setPositionSource(QGeoPositionInfoSource::createDefaultSource(this));
    m_mapView->locationDisplay()->start();
}

void DisplayDeviceLocation::startLocationDisplay()
{
    // start location display
    m_mapView->locationDisplay()->start();
}

void DisplayDeviceLocation::stopLocationDisplay()
{
    // stop location display
    m_mapView->locationDisplay()->stop();
}

void DisplayDeviceLocation::setAutoPanMode(QString autoPanMode)
{
    // set the correct auto pan mode on the location display
    if (autoPanMode == m_compassMode)
    {
        m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::CompassNavigation);
    }
    else if (autoPanMode == m_navigationMode)
    {
        m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Navigation);
    }
    else if (autoPanMode == m_recenterMode)
    {
        m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Default);
    }
    else if (autoPanMode == m_stopMode)
    {
        m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Off);
    }
}

QString DisplayDeviceLocation::compassMode() const
{
    return m_compassMode;
}

QString DisplayDeviceLocation::navigationMode() const
{
    return m_navigationMode;
}

QString DisplayDeviceLocation::recenterMode() const
{
    return m_recenterMode;
}

QString DisplayDeviceLocation::onMode() const
{
    return m_onMode;
}

QString DisplayDeviceLocation::stopMode() const
{
    return m_stopMode;
}

QString DisplayDeviceLocation::closeMode() const
{
    return m_closeMode;
}
