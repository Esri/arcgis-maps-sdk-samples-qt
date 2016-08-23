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

#ifndef MOBILEMAP_SEARCHANDROUTE_H
#define MOBILEMAP_SEARCHANDROUTE_H

namespace Esri
{
    namespace ArcGISRuntime
    {
        class Map;
        class MapQuickView;
        class MobileMapPackage;
    }
}

#include <QQuickItem>
#include <QFileInfoList>
#include <QDir>

class MobileMap_SearchAndRoute : public QQuickItem
{
    Q_OBJECT

public:
    MobileMap_SearchAndRoute(QQuickItem* parent = nullptr);
    ~MobileMap_SearchAndRoute();

    void componentComplete() Q_DECL_OVERRIDE;

    void identifyMobileMapPackages();
    void createMobileMapPackages(int index);

private:
    QDir m_mmpkDirectory;
    QFileInfoList m_fileInfoList;
    Esri::ArcGISRuntime::Map* m_map;
    Esri::ArcGISRuntime::MapQuickView* m_mapView;
    QList<Esri::ArcGISRuntime::MobileMapPackage*> m_mobileMapPackages;
};

#endif // MOBILEMAP_SEARCHANDROUTE_H
