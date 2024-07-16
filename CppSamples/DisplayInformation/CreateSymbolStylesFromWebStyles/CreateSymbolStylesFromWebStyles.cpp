// [WriteFile Name=CreateSymbolStylesFromWebStyles, Category=DisplayInformation]
// [Legal]
// Copyright 2021 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "CreateSymbolStylesFromWebStyles.h"

#include "FeatureLayer.h"
#include "Map.h"
#include "MapQuickView.h"
#include "ServiceFeatureTable.h"
#include "SymbolStyle.h"
#include "UniqueValueRenderer.h"
#include "MapTypes.h"
#include "LayerListModel.h"
#include "SymbolStyleSearchParameters.h"
#include "SymbolStyleSearchResultListModel.h"
#include "SymbolStyleSearchResult.h"
#include "UniqueValue.h"
#include "UniqueValueListModel.h"
#include "SpatialReference.h"
#include "Point.h"
#include "Symbol.h"

#include <QFuture>
#include <QUuid>

using namespace Esri::ArcGISRuntime;

CreateSymbolStylesFromWebStyles::CreateSymbolStylesFromWebStyles(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISNavigation, this))
{
  const QUrl webStyleLayerUrl = QUrl("https://services.arcgis.com/V6ZHFr6zdgNZuVG0/arcgis/rest/services/LA_County_Points_of_Interest/FeatureServer/0");
  m_webStyleLayer = new FeatureLayer(new ServiceFeatureTable(webStyleLayerUrl, this), this);

  m_uniqueValueRenderer = new UniqueValueRenderer(this);

  // The UniqueValueRenderer will affect specific features based on the values of the specified FieldName(s)
  m_uniqueValueRenderer->setFieldNames({"cat2"});

  // The UniqueValueRenderer defines how features of a FeatureLayer are stylized
  // Without an overriding UniqueValueRenderer, features from m_webStyleLayer will use the web layer's default gray circle style
  m_webStyleLayer->setRenderer(m_uniqueValueRenderer);

  m_map->operationalLayers()->append(m_webStyleLayer);

  // Set the scale at which feature symbols and text will appear at their default size
  m_map->setReferenceScale(100'000);

  performSymbolSearch();
}

void CreateSymbolStylesFromWebStyles::performSymbolSearch()
{
  QMap<QString,QStringList> categoriesMap = createCategoriesMap();
  SymbolStyleSearchParameters searchParams;
  searchParams.setKeys(categoriesMap.keys());
  searchParams.setKeysStrictlyMatch(true);

  SymbolStyle* symbolStyle = new SymbolStyle("Esri2DPointSymbolsStyle", nullptr, this);
  symbolStyle->searchSymbolsAsync(searchParams).then(this,
  [this, symbolStyle, categoriesMap](SymbolStyleSearchResultListModel* searchResults)
  {
    m_legendInfoListModel = searchResults;
    emit legendInfoListModelChanged();

    auto results = searchResults->searchResults();
    for (SymbolStyleSearchResult& symbolStyleSearchResult : results)
    {
      // We pass symbolStyle as the QObject parent for fetchSymbol() because we don't need access to the resulting class outside the lifetime of this SymbolStyle
      symbolStyleSearchResult.fetchSymbolAsync(symbolStyle).then(this,
      [this, symbolStyleSearchResult, categoriesMap](Symbol* symbol)
      {
        const QString symbolLabel = symbolStyleSearchResult.key();
        // If multiple field names are set, we can pass multiple values from each field,
        // However, even though we are using the same symbol, we must create a UniqueValue for each value from the same field
        // When the FeatureLayer is rendered, all features with a matching value in the specified FieldNames will appear with the defined UniqueValue
        for (const QString &category : categoriesMap[symbolLabel])
        {
          m_uniqueValueRenderer->uniqueValues()->append(new UniqueValue(symbolLabel, "", {category}, symbol, this));
        }
      });
    }

  });
}

CreateSymbolStylesFromWebStyles::~CreateSymbolStylesFromWebStyles() = default;

void CreateSymbolStylesFromWebStyles::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<CreateSymbolStylesFromWebStyles>("Esri.Samples", 1, 0, "CreateSymbolStylesFromWebStylesSample");
  qmlRegisterUncreatableType<QAbstractListModel>("Esri.Samples", 1, 0, "AbstractListModel", "AbstractListModel is uncreateable");
}

MapQuickView* CreateSymbolStylesFromWebStyles::mapView() const
{
  return m_mapView;
}

void CreateSymbolStylesFromWebStyles::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  constexpr double x_longitude = -118.44186;
  constexpr double y_latitude = 34.28301;
  constexpr double scale = 7000;
  const Point centerPt(x_longitude, y_latitude, SpatialReference::wgs84());

  m_mapView->setViewpointCenterAsync(centerPt, scale);

  connect(m_mapView, &MapQuickView::mapScaleChanged, this, [this]()
  {
    // Set scale symbols to true when we zoom in so the symbols don't take up the entire view
    m_webStyleLayer->setScaleSymbols(m_mapView->mapScale() >= 80'000);
  });

  emit mapViewChanged();
}

QMap<QString,QStringList> CreateSymbolStylesFromWebStyles::createCategoriesMap()
{
  QMap<QString,QStringList> categories =
  {
    {"atm", {"Banking and Finance"}},
    {"beach", {"Beaches and Marinas"}},
    {"campground", {"Campgrounds"}},
    {"city-hall", {"City Halls", "Government Offices"}},
    {"hospital", {"Hospitals and Medical Centers", "Health Screening and Testing", "Health Centers", "Mental Health Centers"}},
    {"library", {"Libraries"}},
    {"park", {"Parks and Gardens"}},
    {"place-of-worship", {"Churches"}},
    {"police-station", {"Sheriff and Police Stations"}},
    {"post-office", {"DHL Locations", "Federal Express Locations"}},
    {"school", {"Public High Schools", "Public Elementary Schools", "Private and Charter Schools"}},
    {"trail", {"Trails"}}
  };

  return categories;
}
