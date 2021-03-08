// [WriteFile Name=CreateSymbolStylesFromWebStyles, Category=DisplayInformation]
// [Legal]
// Copyright 2021 Esri.

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

#include "CreateSymbolStylesFromWebStyles.h"

#include "FeatureLayer.h"
#include "ServiceFeatureTable.h"
#include "Map.h"
#include "MapQuickView.h"

#include "UniqueValueRenderer.h"
#include "SymbolStyle.h"
#include "SimpleMarkerSymbol.h"

using namespace Esri::ArcGISRuntime;

CreateSymbolStylesFromWebStyles::CreateSymbolStylesFromWebStyles(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISNavigation, this))
{
  QUrl webStyleLayerUrl = QUrl("http://services.arcgis.com/V6ZHFr6zdgNZuVG0/arcgis/rest/services/LA_County_Points_of_Interest/FeatureServer/0");
  m_webStyleLayer = new FeatureLayer(new ServiceFeatureTable(webStyleLayerUrl, this), this);

  m_uniqueValueRenderer = new UniqueValueRenderer(this);

  // The UniqueValueRenderer will affect specific features based on the values of the specified FieldName(s)
  m_uniqueValueRenderer->setFieldNames({"cat2"});

  // The UniqueValueRenderer defines how features of a FeatureLayer are stylized
  // Without an overriding UniqueValueRenderer, features from m_webStyleLayer will use the web layer's default gray circle style
  m_webStyleLayer->setRenderer(m_uniqueValueRenderer);

  m_map->operationalLayers()->append(m_webStyleLayer);

  // Populate the legend from the UniqueValuesRenderer's UniqueValuesList once all UniqueValues have been added
  connect(m_map->legendInfos(), &LegendInfoListModel::fetchLegendInfosCompleted, this, [this]()
  {
    m_legendInfoListModel = m_map->legendInfos();
    emit legendInfoListModelChanged();
  });

  // Set the scale at which feature symbols and text will appear at their default size
  m_map->setReferenceScale(100'000);

  createSymbolStyles();
}

void CreateSymbolStylesFromWebStyles::createSymbolStyles()
{
  m_categoriesMap = createCategoriesMap();
  m_symbolsFetchedCount = 0;

  for (const QString &symbolKey : m_categoriesMap.keys())
  {
    SymbolStyle* symbolStyle = new SymbolStyle("Esri2DPointSymbolsStyle", {}, this);

    connect(symbolStyle, &SymbolStyle::fetchSymbolCompleted, this, [this, symbolKey](QUuid /* taskId */, Symbol* symbol)
    {
      // If multiple field names are set, we can pass multiple values from each field,
      // However, even though we are using the same symbol, we must create a UniqueValue for each value from the same field
      // When the FeatureLayer is rendered, all features with a matching value in the specified FieldNames will appear with the defined UniqueValue
      for (const QString &category : m_categoriesMap[symbolKey])
      {
        // The resulting legend will use the order of UniqueValues in the UniqueValueRenderer, so we ensure that it is kept in alphabetical order
        addAUniqueValuesToRendererAndSort(new UniqueValue(symbolKey, "", {category}, symbol, this));
      }

      // fetchLegendInfosCompleted will only trigger once, so we need to ensure it triggers after all UniqueValues have been added
      m_symbolsFetchedCount++;
      if (m_categoriesMap.keys().size() == m_symbolsFetchedCount)
        m_map->setAutoFetchLegendInfos(true);
    });

    symbolStyle->fetchSymbol({symbolKey});
  }
}

void CreateSymbolStylesFromWebStyles::addAUniqueValuesToRendererAndSort(UniqueValue* newUniqueValue)
{
  int idx = m_uniqueValueRenderer->uniqueValues()->size();
  m_uniqueValueRenderer->uniqueValues()->append(newUniqueValue);

  // A simple bubble sort to alphabetize the UniqueValuesList by label
  while (true)
  {
    if (idx < 1 || m_uniqueValueRenderer->uniqueValues()->at(idx)->label() >= m_uniqueValueRenderer->uniqueValues()->at(idx-1)->label())
      break;

    m_uniqueValueRenderer->uniqueValues()->move(idx, idx-1);
    --idx;
  }
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

  const double x_longitude = -118.44186;
  const double y_latitude = 34.28301;
  const Point centerPt(x_longitude, y_latitude, SpatialReference::wgs84());
  const double scale = 7000;

  m_mapView->setViewpointCenter(centerPt, scale);

  connect(m_mapView, &MapQuickView::mapScaleChanged, this, [this]()
  {
    // Set scale symbols to true when we zoom in so the symbols don't take up the entire view
    m_webStyleLayer->setScaleSymbols(m_mapView->mapScale() >= 80'000);
  });

  emit mapViewChanged();
}

QMap<QString,QStringList> CreateSymbolStylesFromWebStyles::createCategoriesMap()
{
  QMap<QString,QStringList> categories;
  categories["atm"] << "Banking and Finance";
  categories["beach"] << "Beaches and Marinas";
  categories["campground"] << "Campgrounds";
  categories["city-hall"] << "City Halls" << "Government Offices";
  categories["hospital"] << "Hospitals and Medical Centers" << "Health Screening and Testing" << "Health Centers" << "Mental Health Centers";
  categories["library"] << "Libraries";
  categories["park"] << "Parks and Gardens";
  categories["place-of-worship"] << "Churches";
  categories["police-station"] << "Sheriff and Police Stations";
  categories["post-office"] << "DHL Locations" << "Federal Express Locations";
  categories["school"] << "Public High Schools" << "Public Elementary Schools" << "Private and Charter Schools";
  categories["trail"] << "Trails";
  return categories;
}
