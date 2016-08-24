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

#include <QDir>
#include <QQmlProperty>
#include <QFileInfoList>
#include <QFile>

using namespace Esri::ArcGISRuntime;

MobileMap_SearchAndRoute::MobileMap_SearchAndRoute(QQuickItem* parent):
    QQuickItem(parent),
    m_mmpkDirectory(QDir::homePath() + "/ArcGIS/Runtime/Data/mmpk/"),
    m_map(nullptr),
    m_mapView(nullptr)
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

                    QVariantMap mmpkProperties;
                    mmpkProperties["name"] = mobileMapPackage->item().title();

                    // for parity with other QVariantList that will be used as a model
                    mmpkProperties["geocoding"] = false;
                    mmpkProperties["routing"] = false;

                    m_mobileMapProperties << mmpkProperties;

                    emit mmpkPropertiesChanged();
                }
            });
        }

        index++;
    }

    createMobileMapPackages(index);
}

// used to interact with ListView in QML
int MobileMap_SearchAndRoute::selectIndex(int index)
{
    return index;
}

void MobileMap_SearchAndRoute::createMapList(int index)
{
    m_mapList.clear();

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

QVariantList MobileMap_SearchAndRoute::mmpkProperties() const
{
    return m_mobileMapProperties;
}

QVariantList MobileMap_SearchAndRoute::mapList() const
{
    return m_mapList;
}
