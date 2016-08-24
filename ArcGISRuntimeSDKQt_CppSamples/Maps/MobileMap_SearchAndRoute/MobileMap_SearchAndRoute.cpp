// [WriteFile Name=MobileMap_SearchAndRoute, Category=Maps]
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

#include "MobileMap_SearchAndRoute.h"

#include "Map.h"
#include "MapQuickView.h"
#include "MobileMapPackage.h"
#include "LocatorTask.h"
#include "RouteTask.h"
#include "RouteParameters.h"
#include "ReverseGeocodeParameters.h"

#include <QDir>
#include <QQmlProperty>
#include <QFileInfoList>
#include <QFile>

using namespace Esri::ArcGISRuntime;

MobileMap_SearchAndRoute::MobileMap_SearchAndRoute(QQuickItem* parent):
    QQuickItem(parent),
    m_mmpkDirectory(QDir::homePath() + "/ArcGIS/Runtime/Data/mmpk/"),
    m_map(nullptr),
    m_mapView(nullptr),
    m_currentLocatorTask(nullptr),
    m_currentRouteParameters(nullptr),
    m_currentRouteTask(nullptr)
{
    m_fileInfoList = m_mmpkDirectory.entryInfoList();
}

MobileMap_SearchAndRoute::~MobileMap_SearchAndRoute()
{
}

void MobileMap_SearchAndRoute::componentComplete()
{
    QQuickItem::componentComplete();

    // find QML MapView component
    m_mapView = findChild<MapQuickView*>("mapView");
    m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

    // identify and create MobileMapPackages using mmpk files in datapath
    createMobileMapPackages(0);
}

void MobileMap_SearchAndRoute::createMobileMapPackages(int index)
{
    if (index >= m_fileInfoList.length())
        return;
    else
    {
        if (m_fileInfoList[index].completeSuffix() == "mmpk")
        {
            MobileMapPackage* mobileMapPackage = new MobileMapPackage(m_mmpkDirectory.absoluteFilePath(m_fileInfoList[index].fileName()));
            mobileMapPackage->load();

            connect(mobileMapPackage, &MobileMapPackage::doneLoading, [mobileMapPackage, this](Error error)
            {
                if (error.isEmpty())
                {
                    qDebug() << mobileMapPackage->path();
                    m_mobileMapPackages.append(mobileMapPackage);

                    m_mobileMapPackageList << mobileMapPackage->item().title();

                    emit mmpkListChanged();
                }
            });
        }

        index++;
    }

    createMobileMapPackages(index);
}

void MobileMap_SearchAndRoute::connectSignals()
{
    connect(m_mapView, &MapQuickView::mouseClick, [this](QMouseEvent& mouseEvent)
    {
      qDebug() << "hey!";
    });
}

void MobileMap_SearchAndRoute::createMapList(int index)
{
    m_mapList.clear();
    m_selectedMmpkIndex = index;

    int counter = 1;
    foreach (const auto& map, m_mobileMapPackages[index]->maps())
    {
        QVariantMap mapList;
        mapList["name"] = map->item().title() + counter;
        mapList["geocoding"] = m_mobileMapPackages[index]->locatorTask() != nullptr;
        mapList["routing"] = map->transportationNetworks().count() > 0;

        m_mapList << mapList;

        ++counter;
    }

    emit mapListChanged();
}

void MobileMap_SearchAndRoute::selectMap(int index)
{
    // set the MapView
    m_mapView->setMap(m_mobileMapPackages[m_selectedMmpkIndex]->maps().at(index));

    // set the locatorTask
    m_currentLocatorTask = m_mobileMapPackages[m_selectedMmpkIndex]->locatorTask();

    // connect geocoding signals

    // create reverse geocoding parameters
    m_reverseGeocodeParameters = new ReverseGeocodeParameters();
    m_reverseGeocodeParameters->setMaxResults(1);

    // create a RouteTask with selected map's transportation network if available
    if (m_mobileMapPackages[m_selectedMmpkIndex]->maps().at(index)->transportationNetworks().count() > 0) {
        m_currentRouteTask = new RouteTask(m_mobileMapPackages[m_selectedMmpkIndex]->maps().at(index)->transportationNetworks().at(0), this);
        m_currentRouteTask->load();
    }

    // connect routing signals -- > loading


}

QStringList MobileMap_SearchAndRoute::mmpkList() const
{
    return m_mobileMapPackageList;
}

QVariantList MobileMap_SearchAndRoute::mapList() const
{
    return m_mapList;
}
