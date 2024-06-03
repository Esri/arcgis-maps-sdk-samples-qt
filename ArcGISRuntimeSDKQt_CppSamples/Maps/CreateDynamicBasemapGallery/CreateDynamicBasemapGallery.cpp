// [WriteFile Name=CreateDynamicBasemapGallery, Category=Maps]
// [Legal]
// Copyright 2024 Esri.

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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "CreateDynamicBasemapGallery.h"

#include "Basemap.h"
#include "BasemapStyleInfo.h"
#include "BasemapStyleLanguageInfo.h"
#include "BasemapStyleParameters.h"
#include "BasemapStylesService.h"
#include "BasemapStylesServiceInfo.h"
#include "Error.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Worldview.h"

#include <QFuture>

using namespace Esri::ArcGISRuntime;

namespace
{
QMap<QString, BasemapStyleLanguageStrategy> languageStrategyNameToEnumMap{
    {"Default", BasemapStyleLanguageStrategy::Default},
    {"Global", BasemapStyleLanguageStrategy::Global},
    {"Local", BasemapStyleLanguageStrategy::Local},
    {"Application Locale", BasemapStyleLanguageStrategy::ApplicationLocale}
};
}

CreateDynamicBasemapGallery::CreateDynamicBasemapGallery(
    QObject* parent /* = nullptr */)
    : QObject(parent),
    m_map(new Map(BasemapStyle::ArcGISNavigation, this))
{
    BasemapStylesService* service = new BasemapStylesService(this);

    connect(service, &BasemapStylesService::doneLoading, this, [this, service](const Error& /*error*/){
        if (service->loadStatus() != LoadStatus::Loaded)
        {
            return;
        }

        m_styleInfos = service->info()->stylesInfo();
        createGallery();
    });

    service->load();
}

CreateDynamicBasemapGallery::~CreateDynamicBasemapGallery()
{}

void CreateDynamicBasemapGallery::init()
{
    // Register the map view for QML
    qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
    qmlRegisterType<CreateDynamicBasemapGallery>("Esri.Samples",
                                                 1,
                                                 0,
                                                 "CreateDynamicBasemapGallerySample");
}

// -------------------------------------------------- //
//                         Methods                    //
// -------------------------------------------------- //

void CreateDynamicBasemapGallery::createGallery()
{
    for (const BasemapStyleInfo* info : m_styleInfos)
    {
        m_gallery->insertRowIntoGallery(info);
    }

    emit galleryChanged();
}

void CreateDynamicBasemapGallery::updateSelectedStyle(const QString& nameOfSelectedStyle)
{
    const auto iteratorToInfoForSelectedStyle = std::find_if(m_styleInfos.begin(),
                                                             m_styleInfos.end(),
                                                             [nameOfSelectedStyle](const BasemapStyleInfo* info){
                                                                 return info->styleName().compare(nameOfSelectedStyle, Qt::CaseInsensitive) == 0;
                                                             });
    m_selectedStyle = *iteratorToInfoForSelectedStyle;
    emit selectedStyleChanged();

    updateLanguageStrategiesList();
    updateLanguagesList();
    updateWorldviewsList();
}

void CreateDynamicBasemapGallery::updateLanguageStrategiesList()
{
    m_languageStrategies.clear();
    if (m_selectedStyle->languageStrategies().isEmpty())
    {
        emit languageStrategiesChanged();
        return;
    }

    m_languageStrategies.append("None"); // Add "None" to allow the user to unset this parameter in the UI.
    for(const BasemapStyleLanguageStrategy& strategy : m_selectedStyle->languageStrategies())
    {
        QString displayName = languageStrategyNameToEnumMap.key(strategy);
        if (displayName.isEmpty())
        {
            continue;
        }
        m_languageStrategies.append(displayName);
    }

    emit languageStrategiesChanged();
}

void CreateDynamicBasemapGallery::updateLanguagesList()
{
    m_languages.clear();
    if (m_selectedStyle->specificLanguages().isEmpty())
    {
        emit languagesChanged();
        return;
    }

    m_languages.append("None"); // Add "None" to allow the user to unset this parameter in the UI.
    for (const BasemapStyleLanguageInfo* info : m_selectedStyle->specificLanguages())
    {
        m_languages.append(info->displayName());
    }

    emit languagesChanged();
}

void CreateDynamicBasemapGallery::updateWorldviewsList()
{
    m_worldviews.clear();
    if (m_selectedStyle->worldviews().isEmpty())
    {
        emit worldviewsChanged();
        return;
    }

    m_worldviews.append("None"); // Add "None" to allow the user to unset this parameter in the UI.
    for(const Worldview* worldview : m_selectedStyle->worldviews())
    {
        m_worldviews.append(worldview->displayName());
    }

    emit worldviewsChanged();
}

void CreateDynamicBasemapGallery::loadBasemap(const QString& selectedStrategy,
                                              const QString& selectedLanguage,
                                              const QString& selectedWorldview)
{
    if (!m_selectedStyle)
    {
        return;
    }

    BasemapStyleParameters* customisations = new BasemapStyleParameters(this);

    if (!selectedStrategy.isEmpty() && selectedStrategy != "None")
    {
        customisations->setLanguageStrategy(languageStrategyNameToEnumMap[selectedStrategy]);
    }

    if (!selectedLanguage.isEmpty() && selectedLanguage != "None")
    {
        const auto specificLanguages = m_selectedStyle->specificLanguages();

        const auto iteratorToLanguageInfoForSelectedLanguage = std::find_if(specificLanguages.begin(),
                                                                            specificLanguages.end(),
                                                                            [selectedLanguage](const BasemapStyleLanguageInfo* info)
                                                                            {
                                                                                return info->displayName().compare(selectedLanguage, Qt::CaseInsensitive) == 0;
                                                                            });

        if (iteratorToLanguageInfoForSelectedLanguage != specificLanguages.end())
        {
            const BasemapStyleLanguageInfo* languageInfo = *iteratorToLanguageInfoForSelectedLanguage;
            const QString code = languageInfo->languageCode();
            customisations->setSpecificLanguage(code);
        }
    }

    if (!selectedWorldview.isEmpty() && selectedWorldview != "None")
    {
        const auto worldviews = m_selectedStyle->worldviews();

        const auto iteratorToSelectedWorldview = std::find_if(worldviews.begin(), worldviews.end(), [selectedWorldview](const Worldview* view)
                                                              {
                                                                  return view->displayName().compare(selectedWorldview, Qt::CaseInsensitive) == 0;
                                                              });

        if (iteratorToSelectedWorldview != worldviews.end())
        {
            customisations->setWorldview(*iteratorToSelectedWorldview);
        }
    }

    Basemap* newBasemap = new Basemap(m_selectedStyle->style(), customisations, this);
    connect(newBasemap, &Basemap::doneLoading, this, [newBasemap, this](const Esri::ArcGISRuntime::Error& loadError){
        if (!loadError.isEmpty())
        {
            return;
        }

        m_viewpoint = m_mapView->currentViewpoint(ViewpointType::CenterAndScale);
        m_mapView->setMap(new Map(newBasemap, this));
        m_mapView->setViewpointAsync(m_viewpoint);
    });

    newBasemap->load();
}

int CreateDynamicBasemapGallery::indexOfSelectedStyle()
{
    return static_cast<int>(m_styleInfos.indexOf(m_selectedStyle));
}

// -------------------------------------------------- //
//             Property getters and setters           //
// -------------------------------------------------- //

MapQuickView* CreateDynamicBasemapGallery::mapView() const
{
    return m_mapView;
}

// Set the view (created in QML)
void CreateDynamicBasemapGallery::setMapView(MapQuickView* mapView)
{
    if (!mapView || mapView == m_mapView)
    {
        return;
    }

    m_mapView = mapView;
    m_mapView->setMap(m_map);
    m_mapView->setViewpointAsync(m_viewpoint);

    emit mapViewChanged();
}

QAbstractListModel* CreateDynamicBasemapGallery::gallery() const
{
    return m_gallery;
}

const QStringList& CreateDynamicBasemapGallery::languageStrategies() const
{
    return m_languageStrategies;
}

const QStringList& CreateDynamicBasemapGallery::languages() const
{
    return m_languages;
}

const QStringList& CreateDynamicBasemapGallery::worldviews() const
{
    return m_worldviews;
}
