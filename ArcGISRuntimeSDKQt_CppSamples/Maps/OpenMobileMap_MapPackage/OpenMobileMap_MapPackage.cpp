// [WriteFile Name=OpenMobileMap_MapPackage, Category=Maps]
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

#include "OpenMobileMap_MapPackage.h"

#include "MobileMapPackage.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include <QQmlProperty>

using namespace Esri::ArcGISRuntime;

OpenMobileMap_MapPackage::OpenMobileMap_MapPackage(QQuickItem* parent) :
    QQuickItem(parent),
    m_mapView(nullptr)
{
}

OpenMobileMap_MapPackage::~OpenMobileMap_MapPackage()
{
}

void OpenMobileMap_MapPackage::componentComplete()
{
    QQuickItem::componentComplete();

    // find QML MapView component
    m_mapView = findChild<MapQuickView*>("mapView");

    // get the data path
    QString dataPath = QQmlProperty::read(this, "dataPath").toUrl().toLocalFile();

    //! [open mobile map package cpp snippet]
    // instatiate a mobile map package
    m_mobileMapPackage = new MobileMapPackage(dataPath + "Yellowstone.mmpk", this);

    // wait for the mobile map package to load
    connect(m_mobileMapPackage, &MobileMapPackage::doneLoading, this, [this](Error error)
    {
        if (error.isEmpty())
        {
            // set the map view's map to the first map in the mobile map package
            m_mapView->setMap(m_mobileMapPackage->maps().at(0));
        }
    });

    // load the mobile map package
    m_mobileMapPackage->load();
    //! [open mobile map package cpp snippet]
}

