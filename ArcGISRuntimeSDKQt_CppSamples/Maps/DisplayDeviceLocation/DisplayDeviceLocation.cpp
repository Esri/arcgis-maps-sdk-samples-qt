// [WriteFile Name=DisplayDeviceLocation, Category=Maps]
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

#include "DisplayDeviceLocation.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include <QGeoPositionInfoSource>
#include <QCompass>

using namespace Esri::ArcGISRuntime;

const QString DisplayDeviceLocation::s_compassMode = "Compass";
const QString DisplayDeviceLocation::s_navigationMode = "Navigation";
const QString DisplayDeviceLocation::s_recenterMode = "Re-Center";
const QString DisplayDeviceLocation::s_onMode = "On";
const QString DisplayDeviceLocation::s_stopMode = "Stop";
const QString DisplayDeviceLocation::s_closeMode = "Close";

DisplayDeviceLocation::DisplayDeviceLocation(QQuickItem* parent) :
    QQuickItem(parent),
    m_map(nullptr),
    m_mapView(nullptr)
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

    //! [start location display api snippet]
    // turn on the location display
    m_mapView->locationDisplay()->setPositionSource(QGeoPositionInfoSource::createDefaultSource(this));
    QCompass* compass = new QCompass(this);
    m_mapView->locationDisplay()->setCompass(compass);
    m_mapView->locationDisplay()->start();
    //! [start location display api snippet]
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
    if (autoPanMode == compassMode())
    {
        m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::CompassNavigation);
    }
    else if (autoPanMode == navigationMode())
    {
        m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Navigation);
    }
    else if (autoPanMode == recenterMode())
    {
        m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Default);
    }
    else if (autoPanMode == stopMode())
    {
        m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Off);
    }
}

const QString DisplayDeviceLocation::compassMode()
{
    return s_compassMode;
}

const QString DisplayDeviceLocation::navigationMode()
{
    return s_navigationMode;
}

const QString DisplayDeviceLocation::recenterMode()
{
    return s_recenterMode;
}

const QString DisplayDeviceLocation::onMode()
{
    return s_onMode;
}

const QString DisplayDeviceLocation::stopMode()
{
    return s_stopMode;
}

const QString DisplayDeviceLocation::closeMode()
{
    return s_closeMode;
}
